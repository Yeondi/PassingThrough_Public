#include "LightFlickerManager.h"
#include "FluorescentLightActor.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

ALightFlickerManager::ALightFlickerManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALightFlickerManager::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	GetWorldTimerManager().SetTimer(FlickerTimerHandle, this, &ALightFlickerManager::CheckNearbyLights, CheckInterval, true);
}

void ALightFlickerManager::CheckNearbyLights()
{
	if (!PlayerPawn) return;

	for (TActorIterator<AFluorescentLightActor> It(GetWorld()); It; ++It)
	{
		AFluorescentLightActor* Light = *It;
		if (!Light || Light->IsFlickering()) continue;

		float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), Light->GetActorLocation());

		if (Distance < FlickerRange)
		{
			// 일정 확률로 깜빡이게 함 (예: 33%)
			if (FMath::FRand() < 0.33f)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Manager Triggering %s"), *Light->GetName());
				Light->StartFlicker();
			}
		}
	}
}
