// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../UI/GameUIManager.h"
#include "../Data/MathProblemData.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "CustomizedPlayerController.generated.h"

class UInteractionPromptWidget;
class UInteractMessageWidget;

/**
 * 
 */
UCLASS()
class PASSINGTHROUGH_API ACustomizedPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACustomizedPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 여러 발걸음 사운드 배열 (랜덤 재생용) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TArray<class USoundBase*> FootstepSounds;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess))
	TObjectPtr<UNinjaInputManagerComponent> InputManager;

	// Ray Trace 관련 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* HitComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	AActor* LastHitActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* LastHitComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	float InteractionDistance;
	
	// 마지막으로 상호작용한 칠판 객체
	UPROPERTY(BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	class AChalkBoardBase* LastInteractedChalkBoard;

	// 움직임, 사운드 관련
	UPROPERTY()
	bool bIsMoving;
	
	UPROPERTY()
	float MovementThreshold = 0.1f;

	UPROPERTY()
	USoundBase* FootstepSound;
	
	UFUNCTION()
	USoundBase* GetRandomFootstepSound();

	UPROPERTY()
	float FootstepInterval = 0.5f;

	UPROPERTY()
	float LastFootstepTime;

	UFUNCTION()
	void PlayFootstepSound(float InputMagnitude);


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

	// Ray Trace 처리 함수
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PerformRayTrace();

	// Ray Trace 결과 처리 함수
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ProcessRayTraceResult(const FHitResult& HitResult);

	// 상호작용 검출 함수
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void CheckForInteractables();
	
public:
	virtual void OnPossess(APawn* InPawn) override; 
	virtual void OnUnPossess() override; 

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable, Category = "Ninja Input")
	UNinjaInputManagerComponent* GetOwnerInputManager() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> UserWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	bool bLastAnswerCorrect = false;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowUI();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideUI();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleUI();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowQuestionUI();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HandleChoiceSelected(int32 ChoiceIndex);

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetUIInputMode();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetGameInputMode();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UPrimitiveComponent* GetHitComponent() const { return HitComponent; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetHitComponent(UPrimitiveComponent* InComponent) { HitComponent = InComponent; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetLastHitActor() const { return LastHitActor; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetLastHitActor(AActor* InActor) { LastHitActor = InActor; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UPrimitiveComponent* GetLastHitComponent() const { return LastHitComponent; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetLastHitComponent(UPrimitiveComponent* InComponent) { LastHitComponent = InComponent; }

	UFUNCTION()
	void OnUIReady();

	void DoNothing();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	UGameUIManager* GameUIManager;
};