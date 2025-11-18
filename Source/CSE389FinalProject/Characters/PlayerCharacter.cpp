#include "PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter()
{

    // Default Tick Stuff
    PrimaryActorTick.bCanEverTick = true;

    // Allow FPS Turning
    bUseControllerRotationYaw = true;
    
    // Create spring arm
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->TargetArmLength = 0.0f;
    SpringArmComponent->bUsePawnControlRotation = true; 
    SpringArmComponent->bInheritPitch = true;
    SpringArmComponent->bInheritRoll = true;
    SpringArmComponent->bInheritYaw = true;

    // Create camera
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;

    // Set Player Defaults
    BaseSpeed = 500;
    BaseStamina = 100;
    BaseHealth = 3;
    StaminaDrainRate = 15.0f;
    StaminaRegenRate = 10.0f;
    SprintSpeedAdditive = 350.0f;
    MinStaminaToJump = 5.0f;
    
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Init Player Defaults
    Speed = BaseSpeed;
    GetCharacterMovement()->MaxWalkSpeed = Speed;
    Stamina = BaseStamina;
    Health = BaseHealth;
}


// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    APlayerController* PC = Cast<APlayerController>(GetController());

    if (PC)
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
        if (Subsystem)
        {
            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(InputMapping, 0);
        }
    }

    UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    
    if (InputComp)
    {
        InputComp->BindAction(InputMove, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
        InputComp->BindAction(InputLook, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
        InputComp->BindAction(InputSprint, ETriggerEvent::Triggered, this, &APlayerCharacter::Sprint);
        InputComp->BindAction(InputInteract, ETriggerEvent::Triggered, this, &APlayerCharacter::DoInteract);
        InputComp->BindAction(InputJump, ETriggerEvent::Triggered, this, &APlayerCharacter::DoJump);
    }
}

/**
 * Character Actions
 * Including: Move, Look, Jump, Sprint, Interact
 */

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    // Take Movement Value
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (MovementVector.X != 0.0f || MovementVector.Y != 0.0f)
    {
        if (bIsSprinting)
        {
            StopStaminaRegenTimer();
            StartStaminaDrainTimer();
        }
    }
    
    
    if (MovementVector.X != 0)
    {
       AddMovementInput(GetActorRightVector(), MovementVector.X);
    }

    if (MovementVector.Y != 0)
    {
       AddMovementInput(GetActorForwardVector(), MovementVector.Y);
    }
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookDirection = Value.Get<FVector2D>();

    if (LookDirection.X != 0)
    {
       AddControllerYawInput(LookDirection.X);
    }

    if (LookDirection.Y != 0)
    {
       AddControllerPitchInput(LookDirection.Y);
    }
}

void APlayerCharacter::Sprint(const FInputActionValue& Value)
{

    bool sprintValue = Value.Get<bool>();

    // If Key is Down
    if (sprintValue)
    {
        if (Stamina > 0)
        {
            bIsSprinting = true;
            UpdateMovementSpeed(SprintSpeedAdditive);
            StopStaminaRegenTimer();
            StartStaminaDrainTimer();
        }
    }

    // If Key is up
    else
    {
        if (bIsSprinting)
        {
            bIsSprinting = false;
            ResetMovementSpeed();

            StopStaminaDrainTimer();
            StartStaminaRegenTimer();
        }
    }
}

void APlayerCharacter::DoInteract(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Display, TEXT("DoInteract"));
}

void APlayerCharacter::DoJump(const FInputActionValue& Value)
{
    bool JumpValue = Value.Get<bool>();
    if (JumpValue && CanJump() && Stamina >= MinStaminaToJump)
    {
       Jump();
        Stamina -= MinStaminaToJump;
        if (!bIsSprinting)
        {
            StartStaminaRegenTimer();
        }
        
    }
}

void APlayerCharacter::UpdateMovementSpeed(float speedAdditive)
{
    Speed += speedAdditive;
    GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void APlayerCharacter::ResetMovementSpeed()
{
    Speed = BaseSpeed;
    GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void APlayerCharacter::StartStaminaDrainTimer()
{
    
    if (!GetWorldTimerManager().IsTimerActive(StaminaDrainTimerHandle))
    {
        GetWorldTimerManager().SetTimer(
            StaminaDrainTimerHandle,
            this,
            &APlayerCharacter::UpdateStaminaDrain,
            0.1f,
            true
        );
    }    
}

void APlayerCharacter::StopStaminaDrainTimer()
{
    GetWorldTimerManager().ClearTimer(StaminaDrainTimerHandle);
}

void APlayerCharacter::StartStaminaRegenTimer()
{
    if (!bIsSprinting && !GetWorldTimerManager().IsTimerActive(StaminaRegenTimerHandle))
    {
        GetWorldTimerManager().SetTimer(
            StaminaRegenTimerHandle,
            this,
            &APlayerCharacter::UpdateStaminaRegen,
            0.1f,
            true
        );
    }
}

void APlayerCharacter::StopStaminaRegenTimer()
{
    GetWorldTimerManager().ClearTimer(StaminaRegenTimerHandle);
}

void APlayerCharacter::UpdateStaminaDrain()
{

    if (GetVelocity().Length() <= 0)
    {
        StopStaminaDrainTimer();
        StartStaminaRegenTimer();

        return;
    }
    
    float DrainAmount = StaminaDrainRate * 0.1f;
    Stamina -= DrainAmount;
    if (Stamina <= 0.0f)
    {
        Stamina = 0.0f;
        if (bIsSprinting)
        {
            bIsSprinting = false;
            ResetMovementSpeed();
        }
        StopStaminaDrainTimer();
        StartStaminaRegenTimer();
    }
}

void APlayerCharacter::UpdateStaminaRegen()
{
    float RegenAmount = StaminaRegenRate * 0.1f;
    Stamina += RegenAmount;
    if (Stamina >= BaseStamina)
    {
        Stamina = BaseStamina;
        StopStaminaRegenTimer();
    }
}