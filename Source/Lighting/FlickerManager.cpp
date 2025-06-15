// Fill out your copyright notice in the Description page of Project Settings.


#include "FlickerManager.h"

// Sets default values
AFlickerManager::AFlickerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UE_LOG(LogTemp, Warning, TEXT("!!!!!! !!! !! [DEPRECATED] FlickerManager - 사용되지 않음, 삭제 예정"));
}

// Called when the game starts or when spawned
void AFlickerManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlickerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

