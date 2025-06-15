// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NinjaInputHandler.h"
#include "InputHandlers/InputHandler_HUDEvent.h"
#include "InputHandler_Menu.generated.h"

class ACustomizedPlayerController;
/**
 * 
 */
UCLASS()
class PASSINGTHROUGH_API UInputHandler_Menu : public UInputHandler_HUDEvent
{
	GENERATED_BODY()

public:
	UInputHandler_Menu();

	UPROPERTY()
	mutable ACustomizedPlayerController * CustomizedPlayerController;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interact")
	float MinimumMagnitudeToJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interact")
	FGameplayTagContainer BlockJumpTags;
	
	virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction, float ElapsedTime) const override;
};