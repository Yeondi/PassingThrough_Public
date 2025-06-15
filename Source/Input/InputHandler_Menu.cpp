// Fill out your copyright notice in the Description page of Project Settings.


#include "InputHandler_Menu.h"

#include "../Core/CustomizedPlayerController.h"
#include "Components/NinjaInputManagerComponent.h"

UInputHandler_Menu::UInputHandler_Menu()
{
}

void UInputHandler_Menu::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction, float ElapsedTime) const
{
	Super::HandleTriggeredEvent_Implementation(Manager, Value, InputAction, ElapsedTime);

	if (!Manager)
		return;

	UWorld* World = Manager->GetWorld();
	if (!World)
		return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController)
	{
		ACustomizedPlayerController* CustomPC = Cast<ACustomizedPlayerController>(PlayerController);
		if (CustomPC)
		{
			CustomPC->HideUI();
		}
	}
	
	UE_LOG(LogTemp,Warning,TEXT("UInputHandler_Menu::HandleTriggeredEvent_Implementation"));
}