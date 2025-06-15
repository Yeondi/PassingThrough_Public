#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../Classroom/ClassroomManager.h"
#include "../Classroom/RestArea.h"
#include "MyGameModeBase.generated.h"

UCLASS()
class PASSINGTHROUGH_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameModeBase();

protected:
	virtual void BeginPlay() override;

	void StartPlay();

private:

	UPROPERTY()
	AClassroomManager* ClassroomManagerInstance = nullptr;
};