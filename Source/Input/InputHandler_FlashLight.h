// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Core/MyPlayerCharacter.h"
#include "NinjaInputHandler.h"
#include "InputHandler_FlashLight.generated.h"

UCLASS(meta = (DisplayName = "Character: FlashLight"))
class PASSINGTHROUGH_API UInputHandler_FlashLight : public UNinjaInputHandler
{
	GENERATED_BODY()

public:
	UInputHandler_FlashLight();

	mutable bool bFlashLightOn = false;
	mutable AMyPlayerCharacter* MyPlayerCharacter;

	virtual void HandleStartedEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
	                                               const UInputAction* InputAction) const override;
	virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	                                                 const FInputActionValue& Value,
	                                                 const UInputAction* InputAction, float ElapsedTime) const override;

	void ToggleFlashLight() const;
};
