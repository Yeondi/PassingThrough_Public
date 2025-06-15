#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorContainer.h"
#include "Classroom.h"
#include "ClassroomManager.generated.h"

UCLASS()
class PASSINGTHROUGH_API AClassroomManager : public AActor
{
	GENERATED_BODY()

public:
	AClassroomManager();

	UPROPERTY(EditAnywhere, Category = "Classroom")
	int32 MaxActiveContainers = 4;

	UFUNCTION(BlueprintCallable, Category="Classroom")
	void NextFloor();
	
	UFUNCTION(BlueprintCallable, Category="Classroom")
	void NextFloorButResetSystem();

	void NextFloorWithContainer(AFloorContainer* RequestingContainer);
	void NextFloorButResetSystemWithContainer(AFloorContainer* RequestingContainer);

	UFUNCTION(BlueprintCallable, Category="Classroom")
	void OnFloorTransitionComplete(AFloorContainer* Container);

	UFUNCTION(BlueprintCallable, Category="Classroom")
	void ResetToFirstGradeOnly(AFloorContainer* FirstGradeContainer);

	UFUNCTION(BlueprintCallable, Category="Classroom")
	void EnsureContainersBehindFirstGrade();

	/** 1학년으로 리셋하는 함수 */
	UFUNCTION(BlueprintCallable, Category="Classroom")
	void ResetToFirstFloor();

	/** 층 번호에 따른 월드 위치 계산 */
	UFUNCTION(BlueprintCallable, Category="Classroom")
	FVector GetFloorPosition(int32 FloorNumber) const;
	
	/** 실제 층 번호를 컨테이너 인덱스로 변환 */
	UFUNCTION(BlueprintCallable, Category="Classroom") 
	int32 GetContainerIndex(int32 FloorNumber) const;
	
	/** 가장 오래된 컨테이너 찾기 */
	UFUNCTION(BlueprintCallable, Category="Classroom")
	AFloorContainer* GetOldestContainer() const;

	UFUNCTION(BlueprintCallable, Category="Classroom")
	void ResetCurrentClassNumber() { CurrentClassNumber = 1; }

	/** 클래스 번호에 따른 교실 타입 결정 */
	UFUNCTION(BlueprintCallable, Category="Classroom")
	EClassroomType GetClassroomTypeForClass(int32 ClassNumber) const;

protected:
	virtual void BeginPlay() override;

	/** ClassroomBPMap 에서 랜덤으로 ClassroomType 뽑기 */
	EClassroomType SelectRandomClassroomType() const;
	
	/** 가장 오래된 컨테이너 재활용 */
	void RecycleOldestContainer();
	
	/** 다음 층 준비 */
	void PrepareNextFloor();

	/** 다음 구역에 새 교실 생성 */
	void CreateNextClassroom();

	/** 1학년용 교실 생성 (시스템 리셋용) */
	void CreateFirstGradeClassroom();
	
	/** 너무 멀리 떨어진 교실들 제거 */
	void CleanupDistantClassrooms();

	/** 안전한 TMap 업데이트 */
	void SafeUpdateContainerUsage(AFloorContainer* Container, int32 FloorNumber);

private:
	/** FloorContainers[0]: StartArea, [1-3]: Loop 프리팹들 (미리 배치됨) */
	UPROPERTY(EditAnywhere, Category="Classroom", meta=(Tooltip="FloorContainers[0]: StartArea, [1-3]: Loop 프리팹들 (미리 배치됨)"))
	TArray<AFloorContainer*> FloorContainers;

	/** 교실 타입 → BP 매핑 (General, Math, Science) */
	UPROPERTY(EditAnywhere, Category="Classroom")
	TMap<EClassroomType, TSubclassOf<AClassroom>> ClassroomBPMap;

	/** 이번에 선택된 랜덤 교실 타입 */
	EClassroomType CurrentClassroomType;

	/** 현재 클래스 번호 정답시 +1 / 오답시 -1 */
	UPROPERTY(BlueprintReadOnly, Category="Classroom", meta=(AllowPrivateAccess="true"))
	int32 CurrentClassNumber = 1;

	int32 ActualFloorNumber = 1;
	
	UPROPERTY()
	TMap<AFloorContainer*, int32> ContainerLastUsedFloor;

	int32 GetContainerIndexForRecycled(AFloorContainer* Container) const;

	void SetTriggerActive(UBoxComponent* TriggerComponent);
	void SetTriggerInactive(UBoxComponent* TriggerComponent);
	
	UFUNCTION(BlueprintCallable, Category="Debug")
	void CheckAllTriggerStates();

public:
	UFUNCTION(BlueprintCallable, Category="Classroom")
	int32 GetActualFloorNumber() const { return ActualFloorNumber; }

	UFUNCTION(BlueprintCallable, Category="Classroom")
	int32 GetCurrentClassNumber() const { return CurrentClassNumber; }
};