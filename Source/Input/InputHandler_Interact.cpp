// Fill out your copyright notice in the Description page of Project Settings.

#include "InputHandler_Interact.h"
#include "../Data/InteractInterface.h"
#include "Components/NinjaInputManagerComponent.h"
// #include "../UI/ChalkBoardBase.h"
// #include "Components/TextBlock.h"

UInputHandler_Interact::UInputHandler_Interact(): MinimumMagnitudeToJump(0)
{
}

void UInputHandler_Interact::HandleStartedEvent_Implementation(
    UNinjaInputManagerComponent* Manager,
    const FInputActionValue& Value,
    const UInputAction* InputAction) const
{
    Super::HandleStartedEvent_Implementation(Manager, Value, InputAction);

    // UE_LOG(LogTemp, Warning, TEXT("[Interact] Key Pressed"));
    if (!Manager) return;

    UWorld* World = Manager->GetWorld();
    if (!World) return;

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return;

    ACustomizedPlayerController* CustomPC = Cast<ACustomizedPlayerController>(PC);
    if (!CustomPC)
    {
        UE_LOG(LogTemp, Error, TEXT("[Interact] Failed to cast PlayerController"));
        return;
    }

    // 마지막 히트된 액터 가져오기
    AActor* HitActor = CustomPC->GetLastHitActor();
    if (!HitActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Interact] No HitActor"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Interact] Target: %s"), *HitActor->GetName());

    // 인터페이스 구현 여부 검사 후 호출
    if (HitActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
    {
        // UE_LOG(LogTemp, Warning, TEXT("[Interact] Calling InteractInterface"));
        IInteractInterface::Execute_Interact_ForceFix(HitActor, CustomPC);
    }
    // else
    // {
        // UE_LOG(LogTemp, Warning, TEXT("[Interact] Actor has no Interact implementation: %s"), *HitActor->GetName());
    // }
}

void UInputHandler_Interact::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
                                                                 const FInputActionValue& Value,
                                                                 const UInputAction* InputAction,
                                                                 float ElapsedTime) const
{
    // UE_LOG(LogTemp, Warning, TEXT("Interact Triggered"));
    Super::HandleTriggeredEvent_Implementation(Manager, Value, InputAction, ElapsedTime);
}

void UInputHandler_Interact::HandleCompletedEvent_Implementation(UNinjaInputManagerComponent* Manager,
                                                                 const FInputActionValue& Value,
                                                                 const UInputAction* InputAction) const
{
    Super::HandleCompletedEvent_Implementation(Manager, Value, InputAction);
}
