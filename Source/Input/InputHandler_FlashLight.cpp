// Fill out your copyright notice in the Description page of Project Settings.


#include "InputHandler_FlashLight.h"

#include "Components/NinjaInputManagerComponent.h"
#include "Kismet/GameplayStatics.h"

UInputHandler_FlashLight::UInputHandler_FlashLight()
{
}

void UInputHandler_FlashLight::HandleStartedEvent_Implementation(UNinjaInputManagerComponent* Manager,
																 const FInputActionValue& Value,
																 const UInputAction* InputAction) const
{
	Super::HandleStartedEvent_Implementation(Manager, Value, InputAction);
	// UE_LOG(LogTemp, Warning, TEXT("Flash Light Started"));

	// Manager에서 World 가져오기 (더 안전함)
	if (!MyPlayerCharacter || !IsValid(MyPlayerCharacter))
	{
		if (Manager && Manager->GetWorld())
		{
			MyPlayerCharacter = Cast<AMyPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(Manager->GetWorld(), 0));
		}
	}

	ToggleFlashLight();
}

void UInputHandler_FlashLight::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
                                                                   const FInputActionValue& Value,
                                                                   const UInputAction* InputAction,
                                                                   float ElapsedTime) const
{
	Super::HandleTriggeredEvent_Implementation(Manager, Value, InputAction, ElapsedTime);
	// UE_LOG(LogTemp, Warning, TEXT("Flash Light Triggered"));
}

void UInputHandler_FlashLight::ToggleFlashLight() const
{
	MyPlayerCharacter = Cast<AMyPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    
	if (!MyPlayerCharacter || !IsValid(MyPlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("MyPlayerCharacter is invalid!"));
		return;
	}

	if (!MyPlayerCharacter->FlashLight || !IsValid(MyPlayerCharacter->FlashLight))
	{
		UE_LOG(LogTemp, Error, TEXT("FlashLight component is invalid!"));
		return;
	}

	bFlashLightOn = !bFlashLightOn;
	MyPlayerCharacter->FlashLight->SetVisibility(bFlashLightOn);

	// UE_LOG(LogTemp, Warning, TEXT("Flash Light %s"), bFlashLightOn ? TEXT("On") : TEXT("Off"));
}
