// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Classroom/ClassroomManager.h"
#include "GeneralAnswerEvaluator.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LocalGlobalSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PASSINGTHROUGH_API ULocalGlobalSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AClassroomManager* ClassroomManagerRef;

	UFUNCTION(BlueprintCallable, Category = "GlobalSubsystem")
	void RegisterClassroomManager(AClassroomManager* Manager);

	UFUNCTION(BlueprintCallable, Category = "GlobalSubsystem")
	AClassroomManager* GetClassroomManager() const { return ClassroomManagerRef; }

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY()
	UGeneralAnswerEvaluator* AnswerEvaluatorRef;
	
	UFUNCTION(BlueprintCallable)
	UGeneralAnswerEvaluator* GetAnswerEvaluator();
	
	UFUNCTION(BlueprintCallable)
	void InitEvaluator();

	UPROPERTY(BlueprintReadOnly, Category = "Problem")
	FMathProblemData CurrentProblemData;
};