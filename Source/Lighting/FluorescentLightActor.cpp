#include "FluorescentLightActor.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"

AFluorescentLightActor::AFluorescentLightActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	Light->SetupAttachment(RootComponent);
}

void AFluorescentLightActor::BeginPlay()
{
	Super::BeginPlay();

	if (FlickerPatterns.Num() > 0)
	{
		FlickerCurve = FlickerPatterns[FMath::RandRange(0, FlickerPatterns.Num() - 1)];
	}

	InitialTimeOffset = FMath::FRandRange(0.0f, 1.0f);
	FlickerDuration = 2.0f; // 고정값 유지

	if (!bLightOnByDefault && Light)
	{
		Light->SetIntensity(0.0f);
	}
}


void AFluorescentLightActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFlickering && FlickerCurve)
	{
		FlickerTimeElapsed += DeltaTime;
		float TimeRatio = FlickerTimeElapsed / FlickerDuration;
		float IntensityFactor = FlickerCurve->GetFloatValue(TimeRatio);

		// UE_LOG(LogTemp, Warning, TEXT("Flickering [%s] - Time: %.2f, Ratio: %.2f, Factor: %.2f"),
			// *GetName(), FlickerTimeElapsed, TimeRatio, IntensityFactor);

		if (TimeRatio >= 1.0f)
		{
			StopFlicker();
		}
		else
		{
			Light->SetIntensity(IntensityFactor * BaseIntensity); // Max intensity 설정
		}
	}
}

void AFluorescentLightActor::StartFlicker()
{
	if (bIsFlickering || !FlickerCurve) return;

	bIsFlickering = true;
	FlickerTimeElapsed = 0.0f;
	SetActorTickEnabled(true);
}

void AFluorescentLightActor::StopFlicker()
{
	bIsFlickering = false;
	SetActorTickEnabled(false);

	if (Light)
	{
		Light->SetIntensity(0.0f); // 다시 꺼짐
	}

	float NextDelay = FMath::FRandRange(4.0f, 10.0f);
	GetWorldTimerManager().SetTimer(
		FlickerRestartTimer,
		this,
		&AFluorescentLightActor::StartFlicker,
		NextDelay,
		false
	);
}
