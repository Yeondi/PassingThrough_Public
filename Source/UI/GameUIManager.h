// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Systems/AnswerEvaluator.h"
// #include "CustomizedPlayerController.h"
#include "ChoiceConfirmDialog.h"
#include "../Systems/GeneralAnswerEvaluator.h"
#include "Blueprint/UserWidget.h"
#include "GameUIManager.generated.h"

/**
 * 
 */
class ACustomizedPlayerController;

UCLASS()
class PASSINGTHROUGH_API UGameUIManager : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void ShowQuestionUI(const FMathProblemData& Data);

	// UFUNCTION(BlueprintImplementableEvent)
	void SetChoiceTexts(const TArray<FString>& InChoices);

	void SetQuestionText(const FText& InText);

	UFUNCTION(BlueprintCallable)
	void HandleChoiceSelected(int32 Index);

	UPROPERTY()
	UGeneralAnswerEvaluator* AnswerEvaluator;

	UPROPERTY()
	FMathProblemData CurrentProblemData;

	ACustomizedPlayerController* GetOwnerPC() const {return OwnerPC.Get(); }

	int32 CurrentSelectedIndex = -1;

	UPROPERTY(editanywhere,Category="UI")
	TSubclassOf<UChoiceConfirmDialog> ConfirmDialogClass;
	
	UPROPERTY()
	UChoiceConfirmDialog* ConfirmDialog;

	UFUNCTION()
	void OnConfirmYes();
	UFUNCTION()
	void OnConfirmNo();
	
protected:
	TWeakObjectPtr<ACustomizedPlayerController> OwnerPC;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestionText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ChoiceA;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ChoiceB;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ChoiceC;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ChoiceD;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_ChoiceE;
};