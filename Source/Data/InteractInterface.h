// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/ObjectMacros.h"
#include "InteractInterface.generated.h"

//UINTERFACE(MinimalAPI)
UINTERFACE(Blueprintable)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class PASSINGTHROUGH_API IInteractInterface
{
	GENERATED_BODY()

public:

	// Text when interact (ex. "Open Door", "Write" )
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionText();
	
	//// Logic for Interact
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact_ForceFix(AActor* Interactor);
};
