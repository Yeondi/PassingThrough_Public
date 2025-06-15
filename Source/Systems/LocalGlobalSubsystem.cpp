// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalGlobalSubsystem.h"
#include "../Classroom/ClassroomManager.h"

void ULocalGlobalSubsystem::RegisterClassroomManager(AClassroomManager* Manager)
{
	if (Manager)
	{
		ClassroomManagerRef = Manager;
		UE_LOG(LogTemp, Log, TEXT("ClassroomManager [%s] registered to LocalGlobalSubsystem"), *Manager->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to register a null ClassroomManager!"));
	}
}

void ULocalGlobalSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULocalGlobalSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UGeneralAnswerEvaluator* ULocalGlobalSubsystem::GetAnswerEvaluator()
{
	if (!AnswerEvaluatorRef)
	{
		InitEvaluator();
	}

	return AnswerEvaluatorRef;
}

void ULocalGlobalSubsystem::InitEvaluator()
{
	if (!AnswerEvaluatorRef)
	{
		AnswerEvaluatorRef = NewObject<UGeneralAnswerEvaluator>(this);
		UE_LOG(LogTemp, Log, TEXT("AnswerEvaluator Initialized"));
	}
}