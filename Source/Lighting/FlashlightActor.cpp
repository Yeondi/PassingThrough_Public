#include "FlashlightActor.h"
#include "Components/SpotLightComponent.h"

AFlashlightActor::AFlashlightActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// 라이트 생성
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	RootComponent = SpotLight;

	// 기본 설정
	SpotLight->Intensity = 3000.0f;
	SpotLight->AttenuationRadius = 1000.0f;
	SpotLight->InnerConeAngle = 20.0f;
	SpotLight->OuterConeAngle = 40.0f;
	SpotLight->SetVisibleFlag(false);

	bIsOn = false;
}

void AFlashlightActor::BeginPlay()
{
	Super::BeginPlay();
}

void AFlashlightActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlashlightActor::ToggleLight()
{
	bIsOn = !bIsOn;
	SpotLight->SetVisibility(bIsOn);
}
