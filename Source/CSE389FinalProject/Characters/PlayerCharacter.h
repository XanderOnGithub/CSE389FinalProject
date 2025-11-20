#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

// Forward Declarations
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class CSE389FINALPROJECT_API APlayerCharacter : public ABaseCharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

#pragma region Components
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components")
    UCameraComponent* CameraComponent;

#pragma endregion

#pragma region Input

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
    UInputMappingContext* InputMapping;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
    UInputAction* InputMove;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
    UInputAction* InputLook;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
    UInputAction* InputSprint;   
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
    UInputAction* InputJump;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
    UInputAction* InputInteract;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Input")
    UInputAction* InputPush;

    // Input Handler Functions
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Sprint(const FInputActionValue& Value);
    void DoInteract(const FInputActionValue& Value);
    void DoJump(const FInputActionValue& Value);
    void DoPush(const FInputActionValue& Value);

#pragma endregion

#pragma region Attributes

protected:
    // --- Health ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Health")
    int32 BaseHealth;

    // --- Stamina Config ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Stamina")
    float BaseStamina;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Stamina")
    float StaminaDrainRate; // Per second

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Stamina")
    float StaminaRegenRate; // Per second

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Stamina")
    float MinStaminaToJump;

    // --- Movement Config ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement")
    float BaseSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement")
    float SprintSpeedAdditive;

#pragma endregion

#pragma region State

protected:
    // Current Values
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Character|State")
    int32 Health;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Character|State")
    float Stamina;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Character|State")
    float Speed;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Character|State")
    bool bIsSprinting;

    // Money variable
    int Money;


#pragma endregion

#pragma region Mechanics

protected:
    UFUNCTION(BlueprintCallable, Category = "Character|Movement")
    void UpdateMovementSpeed(float SpeedAdditive);

    UFUNCTION(BlueprintCallable, Category = "Character|Movement")
    void ResetMovementSpeed();

    // --- Stamina System Internals ---
    
    void StartStaminaDrainTimer();
    void StopStaminaDrainTimer();
    
    // UFUNCTION ensures safe binding for timers
    UFUNCTION() 
    void UpdateStaminaDrain();

    void StartStaminaRegenTimer();
    void StopStaminaRegenTimer();
    
    UFUNCTION()
    void UpdateStaminaRegen();

    // Timers
    FTimerHandle StaminaDrainTimerHandle;
    FTimerHandle StaminaRegenTimerHandle;

    // Get/Set Money
    int GetMoney();
    void SetMoney(int change);

#pragma endregion
};