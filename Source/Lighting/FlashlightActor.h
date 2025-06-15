#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlashlightActor.generated.h"

UCLASS()
class PASSINGTHROUGH_API AFlashlightActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlashlightActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 라이트 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USpotLightComponent* SpotLight;

	// 라이트 토글
	void ToggleLight();

private:
	bool bIsOn;
};
