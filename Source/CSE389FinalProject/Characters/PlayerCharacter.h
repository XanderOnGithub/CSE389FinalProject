// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
struct FTimerHandle;

UCLASS()
class CSE389FINALPROJECT_API APlayerCharacter : public ABaseCharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

    // --- Core Overrides ---
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    
protected:
    virtual void BeginPlay() override;

    // --- Components ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComponent;

    // --- Enhanced Input Configuration ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputMappingContext* InputMapping;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* InputMove;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* InputLook;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* InputSprint;   
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* InputJump;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* InputInteract;

    // --- Input Handlers ---
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Sprint(const FInputActionValue& Value);
    void DoInteract(const FInputActionValue& Value);
    void DoJump(const FInputActionValue& Value);

    // --- Attributes: Health ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Health")
    int BaseHealth;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Health")
    int Health;

    // --- Attributes: Stamina ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Stamina")
    float BaseStamina;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Stamina")
    float Stamina;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Stamina|Rate")
    float StaminaDrainRate; // Stamina drained per second
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Stamina|Rate")
    float StaminaRegenRate; // Stamina regenerated per second

    
    // --- Attributes: Speed ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Speed")
    float BaseSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Speed")
    float Speed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Speed|Sprint")
    float SprintSpeedAdditive;  

    // --- Attributes: State ---
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attributes|State")
    bool bIsSprinting;

    // --- Gameplay Functions ---
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void UpdateMovementSpeed(float SpeedAdditive);

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void ResetMovementSpeed();
    
    // --- Stamina Functions ---
    void StartStaminaDrainTimer();
    void StopStaminaDrainTimer();
    void UpdateStaminaDrain();

    void StartStaminaRegenTimer();
    void StopStaminaRegenTimer();
    void UpdateStaminaRegen();

    // --- Timers ---
    FTimerHandle StaminaDrainTimerHandle;
    FTimerHandle StaminaRegenTimerHandle;
    
};