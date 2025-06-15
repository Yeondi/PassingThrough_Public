// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlickerManager.generated.h"

UCLASS()
class PASSINGTHROUGH_API AFlickerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlickerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
