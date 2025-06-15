#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightFlickerManager.generated.h"

class AFluorescentLightActor;

UCLASS()
class PASSINGTHROUGH_API ALightFlickerManager : public AActor
{
	GENERATED_BODY()

public:
	ALightFlickerManager();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category="Flicker")
	float CheckInterval = 2.0f;

	UPROPERTY(EditAnywhere, Category="Flicker")
	float FlickerRange = 150.0f;

	FTimerHandle FlickerTimerHandle;

	void CheckNearbyLights();

	APawn* PlayerPawn;
};
