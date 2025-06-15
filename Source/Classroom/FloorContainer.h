#pragma once

#include "CoreMinimal.h"
#include "../Data/ClassroomTypes.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Classroom.h"
#include "FloorContainer.generated.h"

UCLASS()
class PASSINGTHROUGH_API AFloorContainer : public AActor
{
	GENERATED_BODY()

public:
	AFloorContainer();
	virtual void BeginPlay() override;

	/** 교실 스폰 위치 (Marking 액터) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	AActor* MarkingActor;

	/** 층 전체 접근 차단용 Barrier */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier")
	UBoxComponent* FloorBarrier;

	/** 문 바로 앞 차단용 Barrier */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Barrier")
	UBoxComponent* DoorBarrier;

	/** 다음층 이동 트리거 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trigger")
	UBoxComponent* DoorTrigger;
	
	/** Spawn된 퍼즐 액터 */
	UPROPERTY()
	AActor* SpawnedPuzzle;

	/** 현재 이 층의 교실 타입 */
	UPROPERTY(BlueprintReadOnly, Category="Classroom")
	EClassroomType CurrentClassroomType = EClassroomType::None;
	
	UPROPERTY()
	class AClassroomManager* ClassroomManager;

	// 개별 트리거 상태 관리
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere, Category="Trigger")
	bool bTriggerProcessing = false;

	// 트리거 상태 관리 함수들
	UFUNCTION(BlueprintCallable, Category="Trigger")
	void SetTriggerProcessing(bool bProcessing);
	
	UFUNCTION(BlueprintCallable, Category="Trigger")
	bool IsTriggerProcessing() const { return bTriggerProcessing; }

	// 기존 함수들
	void ClearPuzzle();

	// 배리어 관련
	UPROPERTY(BlueprintReadOnly, Category="Barrier")
	bool bBarriersEnabled = false;
	
	void EnableBarriers();
	void DisableBarriers();

	UFUNCTION(BlueprintImplementableEvent,Category="Events")
	void OnBarrierStateChanged();

	UFUNCTION(BlueprintCallable, Category="Classroom")
	void SetClassroom(EClassroomType NewType, const TMap<EClassroomType, TSubclassOf<AClassroom>>& ClassroomBPMap);
	
	UFUNCTION(BlueprintCallable, Category="Trigger")
	void ResetTrigger();

	UFUNCTION()
	void OnPlayerEnterTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 문제 데이터 테이블 */
	UPROPERTY(EditAnywhere, Category="Problem Data")
	UDataTable* MathProblemsDataTable;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootScene;

	// 안전장치: 트리거 타임아웃 타이머
	FTimerHandle TriggerTimeoutTimer;
	
	UFUNCTION()
	void OnTriggerTimeout();
};