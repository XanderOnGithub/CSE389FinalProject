#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h" 

class ABaseCharacter;

UINTERFACE(Blueprintable, BlueprintType) 
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class CSE389FINALPROJECT_API IInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	void OnInteract(ABaseCharacter* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	void OnHover(bool bHovered);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	FString ReturnInteractText();
	
};