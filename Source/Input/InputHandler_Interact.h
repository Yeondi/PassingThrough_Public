// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Core/CustomizedPlayerController.h"
#include "NinjaInputHandler.h"
#include "InputHandler_Interact.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Character: Interact"))
class PASSINGTHROUGH_API UInputHandler_Interact : public UNinjaInputHandler
{
	GENERATED_BODY()
	
public:
	UInputHandler_Interact();

	UPROPERTY()
	mutable ACustomizedPlayerController * CustomizedPlayerController;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interact")
	float MinimumMagnitudeToJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interact")
	FGameplayTagContainer BlockJumpTags;
	
	virtual void HandleStartedEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const override;	
	
	virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction, float ElapsedTime) const override;
	
	virtual void HandleCompletedEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const override;
};
