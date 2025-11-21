#include "PlayerCharacter.h"

// Engine Includes
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"

// Enhanced Input Includes
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

APlayerCharacter::APlayerCharacter()
{
    // --- Default Tick Stuff ---
    PrimaryActorTick.bCanEverTick = true;

    // --- Rotation Defaults ---
    bUseControllerRotationYaw = true;

    // --- Create Components ---
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->TargetArmLength = 0.0f;
    SpringArmComponent->bUsePawnControlRotation = true;
    SpringArmComponent->bInheritPitch = true;
    SpringArmComponent->bInheritRoll = true;
    SpringArmComponent->bInheritYaw = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;

    // --- Initialize Attribute Defaults ---
    BaseSpeed = 500.0f;
    SprintSpeedAdditive = 350.0f;
    
    BaseHealth = 3;
    
    BaseStamina = 100.0f;
    StaminaDrainRate = 15.0f;
    StaminaRegenRate = 10.0f;
    MinStaminaToJump = 10.0f; 

    // Initialize State
    bIsSprinting = false;

    Money = 350;
    Collectables = 0;
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    Speed = BaseSpeed;
    Stamina = BaseStamina;
    Health = BaseHealth;

    if (UCharacterMovementComponent* CMC = GetCharacterMovement())
    {
        CMC->MaxWalkSpeed = Speed;
    }

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (InputMapping)
            {
                Subsystem->ClearAllMappings();
                Subsystem->AddMappingContext(InputMapping, 0);
            }
        }
    }
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (InputMove)      InputComp->BindAction(InputMove, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
        if (InputLook)      InputComp->BindAction(InputLook, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
        if (InputSprint)    InputComp->BindAction(InputSprint, ETriggerEvent::Triggered, this, &APlayerCharacter::Sprint);
        if (InputInteract)  InputComp->BindAction(InputInteract, ETriggerEvent::Started, this, &APlayerCharacter::DoInteract);
        if (InputJump)      InputComp->BindAction(InputJump, ETriggerEvent::Triggered, this, &APlayerCharacter::DoJump);
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        
        if (MovementVector.Y != 0.0f)
        {
            AddMovementInput(GetActorForwardVector(), MovementVector.Y);
        }
        
        if (MovementVector.X != 0.0f)
        {
            AddMovementInput(GetActorRightVector(), MovementVector.X);
        }

        if ((MovementVector.X != 0.0f || MovementVector.Y != 0.0f) && bIsSprinting)
        {
            StopStaminaRegenTimer();
            StartStaminaDrainTimer();
        }
    }
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookDirection = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        if (LookDirection.X != 0.0f)
        {
            AddControllerYawInput(LookDirection.X);
        }

        if (LookDirection.Y != 0.0f)
        {
            AddControllerPitchInput(LookDirection.Y);
        }
    }
}

void APlayerCharacter::Sprint(const FInputActionValue& Value)
{
    const bool bSprintInput = Value.Get<bool>();

    if (bSprintInput)
    {
        if (Stamina > 0.0f && !bIsSprinting)
        {
            bIsSprinting = true;
            UpdateMovementSpeed(SprintSpeedAdditive);
            
            StopStaminaRegenTimer();
            StartStaminaDrainTimer();
        }
    }
    else
    {
        // Stop Sprinting
        if (bIsSprinting)
        {
            bIsSprinting = false;
            ResetMovementSpeed();

            StopStaminaDrainTimer();
            StartStaminaRegenTimer();
        }
    }
}

void APlayerCharacter::DoPush(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Log, TEXT("Player Push Triggered"));
}

void APlayerCharacter::DoJump(const FInputActionValue& Value)
{
    if (CanJump() && Stamina >= MinStaminaToJump)
    {
        Jump();

        Stamina = FMath::Max(0.0f, Stamina - MinStaminaToJump);
        
        StopStaminaRegenTimer();
        
        if (!bIsSprinting)
        {
             StartStaminaRegenTimer(); 
        }
    }
}

void APlayerCharacter::UpdateMovementSpeed(float SpeedAdditive)
{
    Speed = BaseSpeed + SpeedAdditive;
    if (UCharacterMovementComponent* CMC = GetCharacterMovement())
    {
        CMC->MaxWalkSpeed = Speed;
    }
}

void APlayerCharacter::ResetMovementSpeed()
{
    Speed = BaseSpeed;
    if (UCharacterMovementComponent* CMC = GetCharacterMovement())
    {
        CMC->MaxWalkSpeed = Speed;
    }
}

// --- Stamina Timers ---

void APlayerCharacter::StartStaminaDrainTimer()
{
    if (!GetWorld()->GetTimerManager().IsTimerActive(StaminaDrainTimerHandle))
    {
        GetWorld()->GetTimerManager().SetTimer(
            StaminaDrainTimerHandle,
            this,
            &APlayerCharacter::UpdateStaminaDrain,
            0.1f,
            true // Loop
        );
    }
}

void APlayerCharacter::StopStaminaDrainTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(StaminaDrainTimerHandle);
}

void APlayerCharacter::StartStaminaRegenTimer()
{
    if (!bIsSprinting && !GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimerHandle))
    {
        GetWorld()->GetTimerManager().SetTimer(
            StaminaRegenTimerHandle,
            this,
            &APlayerCharacter::UpdateStaminaRegen,
            0.1f,
            true // Loop
        );
    }
}

void APlayerCharacter::StopStaminaRegenTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
}

void APlayerCharacter::UpdateStaminaDrain()
{
    if (GetVelocity().IsNearlyZero())
    {
        return;
    }

    const float DrainAmount = StaminaDrainRate * 0.1f;
    Stamina = FMath::Max(0.0f, Stamina - DrainAmount);

    if (Stamina <= 0.0f)
    {
        // Stamina Empty
        Stamina = 0.0f;
        
        // Force stop sprint
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
    // Don't regen if we are supposed to be sprinting
    if (bIsSprinting) 
    {
        StopStaminaRegenTimer();
        return;
    }

    const float RegenAmount = StaminaRegenRate * 0.1f;
    Stamina = FMath::Min(BaseStamina, Stamina + RegenAmount);

    if (Stamina >= BaseStamina)
    {
        Stamina = BaseStamina;
        StopStaminaRegenTimer();
    }
}

int APlayerCharacter::GetMoney()
{
    return Money;
}

void APlayerCharacter::SetMoney(int change)
{
    Money = Money + change;
}

int APlayerCharacter::GetCollectables()
{
    return Collectables;
}

void APlayerCharacter::AddCollectable()
{
    Collectables += 1;
}

