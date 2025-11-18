// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CSE389FINALPROJECT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

protected:
	
	
	public:
    	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
