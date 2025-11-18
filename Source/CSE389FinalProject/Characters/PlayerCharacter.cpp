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
    baseSpeed = 500;
    baseStamina = 100;
    baseHealth = 3;
    
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Init Player Defaults
    speed = baseSpeed;
    GetCharacterMovement()->MaxWalkSpeed = speed;
    stamina = baseStamina;
    health = baseHealth;
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
    UE_LOG(LogTemp, Display, TEXT("sprint"));
}

void APlayerCharacter::DoInteract(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Display, TEXT("DoInteract"));
}

void APlayerCharacter::DoJump(const FInputActionValue& Value)
{
    bool JumpValue = Value.Get<bool>();
    if (JumpValue && CanJump())
    {
       Jump();
    }
}

void APlayerCharacter::UpdateMovementSpeed(float speedModifier)
{
    speed += speedModifier;
    GetCharacterMovement()->MaxWalkSpeed = speed;
}

void APlayerCharacter::ResetMovementSpeed()
{
    speed = baseSpeed;
    GetCharacterMovement()->MaxWalkSpeed = speed;
}
