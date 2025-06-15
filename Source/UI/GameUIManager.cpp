// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUIManager.h"
#include "../Core/CustomizedPlayerController.h"
#include "Components/TextBlock.h"

bool UGameUIManager::Initialize()
{
	AnswerEvaluator = NewObject<UGeneralAnswerEvaluator>(this, UGeneralAnswerEvaluator::StaticClass());

	
	return Super::Initialize();
}

void UGameUIManager::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);

	OwnerPC = Cast<ACustomizedPlayerController>(GetOwningPlayer());

	// if (OwnerPC.IsValid())
	// {
	// 	// Slate 위젯 완전히 생성된 이후 알려줌
	// 	OwnerPC->OnUIReady();
	// }
}

void UGameUIManager::ShowQuestionUI(const FMathProblemData& Data)
{
	CurrentProblemData = Data;

	SetQuestionText(FText::FromString(Data.Question));
	SetChoiceTexts({
		Data.ChoiceA,
		Data.ChoiceB,
		Data.ChoiceC,
		Data.ChoiceD,
		Data.ChoiceE
	});

	SetVisibility(ESlateVisibility::Visible);

}

void UGameUIManager::SetChoiceTexts(const TArray<FString>& InChoices)
{
	if (InChoices.Num() < 5)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough choices! Expected 5, got %d"), InChoices.Num());
		return;
	}

	TArray<UTextBlock*> TextBlocks = {
		TextBlock_ChoiceA,
		TextBlock_ChoiceB,
		TextBlock_ChoiceC,
		TextBlock_ChoiceD,
		TextBlock_ChoiceE
	};

	for (int32 i = 0; i < TextBlocks.Num(); ++i)
	{
		if (TextBlocks[i])
		{
			const FString& Choice = InChoices[i];

			// 탭처럼 띄우기 위해 스페이스 여러 개 사용
			FString Formatted = FString::Printf(TEXT("(%d).    %s"), i + 1, *Choice);

			TextBlocks[i]->SetText(FText::FromString(Formatted));
		}
	}
}

void UGameUIManager::SetQuestionText(const FText& InText)
{
	if (QuestionText)
	{
		QuestionText->SetText(InText);
	}
}

void UGameUIManager::HandleChoiceSelected(int32 Index)
{
	CurrentSelectedIndex = Index;

	// 항상 새로 생성
	if (ConfirmDialogClass)
	{
		ConfirmDialog = CreateWidget<UChoiceConfirmDialog>(GetWorld(), ConfirmDialogClass);

		// ConfirmDialog->OnConfirmed.AddDynamic(this, &UGameUIManager::OnConfirmYes);
		// ConfirmDialog->OnCancelled.AddDynamic(this, &UGameUIManager::OnConfirmNo);
		ConfirmDialog->SetOwnerUIManager(this);

		FString ConfirmText = FString::Printf(TEXT("%d번을 선택하셨습니다. 맞습니까?"), Index + 1);
		UE_LOG(LogTemp, Log, TEXT("ConfirmText: %s"), *ConfirmText);
		ConfirmDialog->SetDialogText(FText::FromString(ConfirmText));

		ConfirmDialog->AddToViewport(999);
	}
}



void UGameUIManager::OnConfirmYes()
{
	UE_LOG(LogTemp, Warning, TEXT("OnConfirmYes 호출됨, 선택한 인덱스: %d"), CurrentSelectedIndex);

	if (!AnswerEvaluator) return;

	bool bCorrect = AnswerEvaluator->EvaluateAnswer(CurrentSelectedIndex, &CurrentProblemData);

	if (bCorrect)
	{
		UE_LOG(LogTemp, Log, TEXT("정답입니다!"));
		SetVisibility(ESlateVisibility::Collapsed);
		OwnerPC->SetGameInputMode();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("오답입니다!"));
		SetVisibility(ESlateVisibility::Collapsed);
		OwnerPC->SetGameInputMode();
	}

	if (ConfirmDialog)
	{
		ConfirmDialog->RemoveFromParent();
		ConfirmDialog = nullptr; // 완전 제거
	}

	if (OwnerPC.IsValid())
	{
		OwnerPC->bLastAnswerCorrect = bCorrect;
		UE_LOG(LogTemp, Log, TEXT("AnswerCorrect: %hs"), bCorrect ? "true" : "false");
	}
}



void UGameUIManager::OnConfirmNo()
{
	if (ConfirmDialog)
	{
		ConfirmDialog->RemoveFromParent();
	}
}

