#pragma once

#include "CoreMinimal.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"
#include "FluorescentLightActor.generated.h"

UCLASS()
class PASSINGTHROUGH_API AFluorescentLightActor : public AActor
{
	GENERATED_BODY()

public:
	AFluorescentLightActor();

	virtual void Tick(float DeltaTime) override;

	void StartFlicker();

	bool IsFlickering() const { return bIsFlickering; }

	

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void StopFlicker();

private:
	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* Light;

	UPROPERTY(EditAnywhere, Category="Flicker")
	UCurveFloat* FlickerCurve;

	UPROPERTY(EditAnywhere)
	TArray<UCurveFloat*> FlickerPatterns;


	UPROPERTY(EditAnywhere, Category="Light")
	float BaseIntensity = 1.0f;
	
	UPROPERTY(EditAnywhere, Category="Light")
	float InitialTimeOffset = 0.0f;

	UPROPERTY(EditAnywhere, Category="Light")
	bool bLightOnByDefault = false;


	UPROPERTY(EditAnywhere, Category="Flicker")
	float FlickerDuration = 3.5f;

	FTimerHandle FlickerRestartTimer;

	float FlickerTimeElapsed = 0.0f;
	bool bIsFlickering = false;
};
