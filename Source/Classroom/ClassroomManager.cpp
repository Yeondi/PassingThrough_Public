#include "ClassroomManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "PassingThrough/Systems/LocalGlobalSubsystem.h"

AClassroomManager::AClassroomManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AClassroomManager::SafeUpdateContainerUsage(AFloorContainer* Container, int32 FloorNumber)
{
	if (!IsValid(Container))
	{
		UE_LOG(LogTemp, Error, TEXT("SafeUpdateContainerUsage: Container가 nullptr이거나 유효하지 않음"));
		return;
	}
	
	// 기존 키가 있으면 값만 업데이트, 없으면 새로 추가
	if (ContainerLastUsedFloor.Contains(Container))
	{
		ContainerLastUsedFloor[Container] = FloorNumber;
	}
	else
	{
		ContainerLastUsedFloor.Add(Container, FloorNumber);
	}
	
	UE_LOG(LogTemp, Verbose, TEXT("컨테이너 사용 기록 업데이트: [%s] = %d층"), 
		*Container->GetName(), FloorNumber);
}

// 재활용된 컨테이너의 인덱스를 찾는 헬퍼 함수 추가
int32 AClassroomManager::GetContainerIndexForRecycled(AFloorContainer* Container) const
{
	for (int32 i = 0; i < FloorContainers.Num(); ++i)
	{
		if (FloorContainers[i] == Container)
		{
			return i;
		}
	}
	return -1; // 찾지 못함
}

// 트리거 활성화 헬퍼 함수들 추가
void AClassroomManager::SetTriggerActive(UBoxComponent* TriggerComponent)
{
	if (!TriggerComponent) return;
	
	// 🔥 강제 설정 - 프로파일 무시하고 직접 설정
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerComponent->SetCollisionObjectType(ECC_WorldStatic);
	TriggerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerComponent->SetGenerateOverlapEvents(true);
}

void AClassroomManager::SetTriggerInactive(UBoxComponent* TriggerComponent)
{
	if (!TriggerComponent) return;
	
	// 완전 비활성화
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerComponent->SetGenerateOverlapEvents(false);
}

void AClassroomManager::CheckAllTriggerStates()
{
	UE_LOG(LogTemp, Error, TEXT("🔍 ==========트리거 상태 체크========== (ActualFloorNumber: %d)"), ActualFloorNumber);
	
	for (int32 i = 0; i < FloorContainers.Num(); ++i)
	{
		if (FloorContainers[i] && FloorContainers[i]->DoorTrigger)
		{
			UBoxComponent* Trigger = FloorContainers[i]->DoorTrigger;
			
			ECollisionEnabled::Type CollisionType = Trigger->GetCollisionEnabled();
			bool bGeneratesOverlap = Trigger->GetGenerateOverlapEvents();
			FVector Location = Trigger->GetComponentLocation();
			
			// 콜리젼 응답 확인
			ECollisionResponse PawnResponse = Trigger->GetCollisionResponseToChannel(ECC_Pawn);
			
			FString StatusText;
			if (CollisionType == ECollisionEnabled::NoCollision)
			{
				StatusText = TEXT("INACTIVE");
			}
			else if (CollisionType == ECollisionEnabled::QueryOnly && PawnResponse == ECR_Overlap && bGeneratesOverlap)
			{
				StatusText = TEXT("ACTIVE");
			}
			
			UE_LOG(LogTemp, Error, TEXT("🔍 [%d] %s: %s (CollisionType=%d, PawnResponse=%d, Overlap=%s)"), 
				i, *FloorContainers[i]->GetName(), *StatusText, (int32)CollisionType, (int32)PawnResponse, 
				bGeneratesOverlap ? TEXT("YES") : TEXT("NO"));
			
			UE_LOG(LogTemp, Error, TEXT("위치: %s"), *Location.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[%d] 컨테이너 또는 트리거 없음"), i);
		}
	}
	
	UE_LOG(LogTemp, Error, TEXT("🔍 ====================================="));
}

void AClassroomManager::BeginPlay()
{
    Super::BeginPlay();

    // TMap 초기화 - Access Violation 방지
    ContainerLastUsedFloor.Empty();

    // 글로벌 서브시스템 등록
    ULocalGlobalSubsystem* GlobalSys = GetGameInstance()->GetSubsystem<ULocalGlobalSubsystem>();
    if (GlobalSys)
    {
        GlobalSys->RegisterClassroomManager(this);
        UE_LOG(LogTemp, Log, TEXT("✅ ClassroomManager 등록 완료"));
    }

    ActualFloorNumber = 1;
    CurrentClassNumber = 1;
    CurrentClassroomType = SelectRandomClassroomType();
    
    UE_LOG(LogTemp, Error, TEXT("🏁 게임 시작: ActualFloorNumber=%d, CurrentClassNumber=%d"), 
        ActualFloorNumber, CurrentClassNumber);

    if (FloorContainers.IsValidIndex(0) && FloorContainers[0])
    {
        FloorContainers[0]->SetClassroom(CurrentClassroomType, ClassroomBPMap);
        FloorContainers[0]->DisableBarriers();
        SafeUpdateContainerUsage(FloorContainers[0], 1);
        
        UE_LOG(LogTemp, Warning, TEXT("🏫 1층 교실 생성 완료: %s"), *UEnum::GetValueAsString(CurrentClassroomType));
    }
    
    // [1],[2],[3] 컨테이너들 초기 상태 설정 (교실 없이 대기, 위치는 건드리지 않음)
    for (int32 i = 2; i < 4 && i < FloorContainers.Num(); ++i)
    {
        if (FloorContainers[i])
        {
            FloorContainers[i]->ClearPuzzle();
            FloorContainers[i]->EnableBarriers();
            FloorContainers[i]->ResetTrigger();
            
            if (FloorContainers[i]->DoorTrigger)
            {
                SetTriggerInactive(FloorContainers[i]->DoorTrigger);
            }
            
        }
    }
    
    // 첫 번째 Loop 컨테이너[1]의 트리거만 활성화 (1층→2층 트리거)
    if (FloorContainers.IsValidIndex(1) && FloorContainers[1] && FloorContainers[1]->DoorTrigger)
    {
        SetTriggerActive(FloorContainers[1]->DoorTrigger);
    }
    
    // Start 컨테이너[0]의 트리거는 영구 비활성화
    if (FloorContainers.IsValidIndex(0) && FloorContainers[0] && FloorContainers[0]->DoorTrigger)
    {
        SetTriggerInactive(FloorContainers[0]->DoorTrigger);
    }

	// FVector Temp = FVector(0.f,-17955.f,0.f);
	// UE_LOG(LogTemp, Error, TEXT("Containers[1] 위치 변경: %s"), *Temp.ToString());
	// FloorContainers[1]->SetActorLocation(Temp);
	
}

// 작업 완료 알림 함수
void AClassroomManager::OnFloorTransitionComplete(AFloorContainer* Container)
{
	if (!Container || !IsValid(Container))
	{
		return;
	}
	
	// 해당 컨테이너의 트리거 플래그 해제
	Container->SetTriggerProcessing(false);
}

// 기존 함수 (호환성 유지) - 원래 로직 그대로
void AClassroomManager::NextFloor()
{
	// 1) 현재 구역 Barrier 활성화
	int32 CurrentContainerIdx = GetContainerIndex(ActualFloorNumber);
	if (FloorContainers.IsValidIndex(CurrentContainerIdx))
	{
		AFloorContainer* CurrentContainer = FloorContainers[CurrentContainerIdx];
		if (CurrentContainer && IsValid(CurrentContainer))
		{
			CurrentContainer->EnableBarriers();
		}
	}

	// 2) 다음 구역 번호 증가
	ActualFloorNumber++;
	CurrentClassNumber++;
	
	// 3) 다음 구역에 새 교실 생성
	CreateNextClassroom();

	// 4) 다음 층 트리거 활성화 - 수정!
	int32 NextFloorForTrigger = ActualFloorNumber + 1;
	int32 NextTriggerContainerIdx = GetContainerIndex(NextFloorForTrigger);
	
	UE_LOG(LogTemp, Error, TEXT("🔍 트리거 활성화 대상: %d층→%d층 트리거 → 컨테이너[%d]"), 
		ActualFloorNumber, NextFloorForTrigger, NextTriggerContainerIdx);
	
	for (int32 i = 0; i < FloorContainers.Num(); ++i)
	{
		if (FloorContainers[i] && FloorContainers[i]->DoorTrigger)
		{
			if (i == 0)
			{
				SetTriggerInactive(FloorContainers[i]->DoorTrigger);
			}
			else if (i == NextTriggerContainerIdx)
			{
				SetTriggerActive(FloorContainers[i]->DoorTrigger);
			}
			else
			{
				SetTriggerInactive(FloorContainers[i]->DoorTrigger);
			}
		}
	}

	CleanupDistantClassrooms();
}

void AClassroomManager::NextFloorWithContainer(AFloorContainer* RequestingContainer)
{
	if (!RequestingContainer || !IsValid(RequestingContainer))
	{
		UE_LOG(LogTemp, Error, TEXT("NextFloorWithContainer: RequestingContainer가 nullptr"));
		return;
	}

	ActualFloorNumber++;
	CurrentClassNumber++;
	
	UE_LOG(LogTemp, Error, TEXT("📊 층수 업데이트: ActualFloorNumber=%d, CurrentClassNumber=%d"), 
		ActualFloorNumber, CurrentClassNumber);
	
	CreateNextClassroom();

	int32 NextTriggerFloor = ActualFloorNumber + 1;
	
	UE_LOG(LogTemp, Error, TEXT("트리거 활성화 대상: 현재=%d층, 다음트리거=%d층"), 
		ActualFloorNumber, NextTriggerFloor);
	
	for (int32 i = 0; i < FloorContainers.Num(); ++i)
	{
		if (FloorContainers[i] && FloorContainers[i]->DoorTrigger)
		{
			if (i == 0)
			{
				SetTriggerInactive(FloorContainers[i]->DoorTrigger);
				continue;
			}
			
			// 해당 층 컨테이너 찾기
			int32 TargetContainerIdx = GetContainerIndex(NextTriggerFloor);
			
			if (i == TargetContainerIdx)
			{
				SetTriggerActive(FloorContainers[i]->DoorTrigger);
			}
			else
			{
				SetTriggerInactive(FloorContainers[i]->DoorTrigger);
			}
		}
	}

	CleanupDistantClassrooms();

	OnFloorTransitionComplete(RequestingContainer);
}

void AClassroomManager::CreateNextClassroom()
{
	int32 NextContainerIdx = GetContainerIndex(ActualFloorNumber);
    
	if (ActualFloorNumber >= 4)
	{
		RecycleOldestContainer();
		return;
	}
    
	if (!FloorContainers.IsValidIndex(NextContainerIdx))
	{
		return;
	}
    
	AFloorContainer* TargetContainer = FloorContainers[NextContainerIdx];
	if (!TargetContainer || !IsValid(TargetContainer))
	{
		return;
	}
    
	if (TargetContainer->CurrentClassroomType != EClassroomType::None)
	{
		TargetContainer->ClearPuzzle();
	}
    
	// 새로운 교실 타입 선택 및 생성
	EClassroomType NewClassroomType = SelectRandomClassroomType();
	TargetContainer->SetClassroom(NewClassroomType, ClassroomBPMap);
	// TargetContainer->DisableBarriers();
	TargetContainer->EnableBarriers();
    
	// 사용 기록 업데이트
	SafeUpdateContainerUsage(TargetContainer, ActualFloorNumber);
}

void AClassroomManager::CleanupDistantClassrooms()
{
	int32 MinValidFloor = ActualFloorNumber - 2; // 2구역 이전까지는 유지
	
	TArray<AFloorContainer*> ContainersToClean;
	
	for (auto& Pair : ContainerLastUsedFloor)
	{
		AFloorContainer* Container = Pair.Key;
		int32 FloorNumber = Pair.Value;
		
		if (!Container || !IsValid(Container))
		{
			ContainersToClean.Add(Container);
			continue;
		}
		
		if (FloorNumber > 0 && FloorNumber < MinValidFloor && Container->CurrentClassroomType != EClassroomType::None)
		{
			Container->ClearPuzzle();
		}
	}
	
	for (AFloorContainer* InvalidContainer : ContainersToClean)
	{
		ContainerLastUsedFloor.Remove(InvalidContainer);
	}
}

void AClassroomManager::RecycleOldestContainer()
{
	// 가장 오래된 컨테이너 찾기
	AFloorContainer* OldestContainer = GetOldestContainer();
	if (!OldestContainer)
	{
		return;
	}
	
	UE_LOG(LogTemp, Error, TEXT("🔄 컨테이너 [%s] 를 %d층으로 재활용"), *OldestContainer->GetActorLabel(), ActualFloorNumber);
	
	FVector OldPosition = OldestContainer->GetActorLocation();
	
	FVector NewPosition = GetFloorPosition(ActualFloorNumber);
	
	UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(OldestContainer->GetRootComponent());
	bool bWasSimulatingPhysics = false;
	if (RootPrimitive)
	{
		bWasSimulatingPhysics = RootPrimitive->IsSimulatingPhysics();
		if (bWasSimulatingPhysics)
		{
			RootPrimitive->SetSimulatePhysics(false);
		}
	}
	
	TArray<UPrimitiveComponent*> PrimitiveComponents;
	OldestContainer->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	TArray<ECollisionEnabled::Type> OriginalCollisionStates;
	
	for (UPrimitiveComponent* Comp : PrimitiveComponents)
	{
		if (Comp)
		{
			OriginalCollisionStates.Add(Comp->GetCollisionEnabled());
			Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	bool bMoveSuccess = OldestContainer->TeleportTo(NewPosition, OldestContainer->GetActorRotation());
	

	for (int32 i = 0; i < PrimitiveComponents.Num(); ++i)
	{
		if (PrimitiveComponents[i] && OriginalCollisionStates.IsValidIndex(i))
		{
			PrimitiveComponents[i]->SetCollisionEnabled(OriginalCollisionStates[i]);
		}
	}
	
	if (RootPrimitive && bWasSimulatingPhysics)
	{
		RootPrimitive->SetSimulatePhysics(true);
	}
	
	FVector FinalPosition = OldestContainer->GetActorLocation();
	float Distance = FVector::Dist(NewPosition, FinalPosition);
	bool bActuallyMoved = Distance < 100.0f;
	
	if (!bActuallyMoved)
	{
		return;
	}
	
	EClassroomType NewClassroomType = SelectRandomClassroomType();
	UE_LOG(LogTemp, Error, TEXT("Oldest Container Name :: %s"), *OldestContainer->GetActorLabel());
	OldestContainer->SetClassroom(NewClassroomType, ClassroomBPMap);
	
	UE_LOG(LogTemp, Error, TEXT("배리어 : RecycleOldestContainer"));
	OldestContainer->DisableBarriers();
	OldestContainer->ResetTrigger();
	
	SafeUpdateContainerUsage(OldestContainer, ActualFloorNumber);
}

void AClassroomManager::PrepareNextFloor()
{
	// 1) ActualFloorNumber가 이미 ++ 된 상태
	int32 idx = GetContainerIndex(ActualFloorNumber);
	if (FloorContainers.IsValidIndex(idx))
	{
		AFloorContainer* TC = FloorContainers[idx];
		TC->ClearPuzzle();
		TC->SetActorLocation(GetFloorPosition(ActualFloorNumber));
		TC->CurrentClassroomType =  EClassroomType::Math;
		TC->SetClassroom(TC->CurrentClassroomType, ClassroomBPMap);
		UE_LOG(LogTemp, Error, TEXT("배리어 : PrepareNextFloor"));
		TC->DisableBarriers();
		TC->ResetTrigger();
	}

	int32 nextFloor = ActualFloorNumber + 1;
	int32 nextIdx = GetContainerIndex(nextFloor);
	if (FloorContainers.IsValidIndex(nextIdx))
	{
		AFloorContainer* NextC = FloorContainers[nextIdx];
		NextC->ClearPuzzle();
		NextC->SetActorLocation(GetFloorPosition(nextFloor));
		NextC->CurrentClassroomType = EClassroomType::Math;
		NextC->SetClassroom(NextC->CurrentClassroomType, ClassroomBPMap);
		UE_LOG(LogTemp, Error, TEXT("배리어 : PrepareNextFloor2"));
		NextC->DisableBarriers();
		NextC->ResetTrigger();
	}

	CleanupDistantClassrooms();
}

void AClassroomManager::NextFloorButResetSystem()
{
	for (AFloorContainer* Container : FloorContainers)
	{
		if (Container && Container->DoorTrigger)
		{
			Container->DoorTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
	// 현재 구역 Barrier 활성화
	int32 CurrentContainerIdx = GetContainerIndex(ActualFloorNumber);
	if (FloorContainers.IsValidIndex(CurrentContainerIdx) && FloorContainers[CurrentContainerIdx])
	{
		FloorContainers[CurrentContainerIdx]->EnableBarriers();
	}
	
	ActualFloorNumber++;
	CurrentClassNumber = 1;
	
	CreateFirstGradeClassroom();
	
	CleanupDistantClassrooms();

	for (int32 i = 0; i < FloorContainers.Num(); ++i)
	{
		if (FloorContainers[i] && FloorContainers[i]->DoorTrigger)
		{
			int32 NewContainerIdx = GetContainerIndex(ActualFloorNumber);
			
			if (i == 0)
			{
				FloorContainers[i]->DoorTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			else if (i == NewContainerIdx)
			{
				FloorContainers[i]->DoorTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			else
			{
				// 나머지는 모두 비활성화 유지
				FloorContainers[i]->DoorTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

void AClassroomManager::NextFloorButResetSystemWithContainer(AFloorContainer* RequestingContainer)
{
	if (!RequestingContainer || !IsValid(RequestingContainer))
	{
		return;
	}
	
	for (AFloorContainer* Container : FloorContainers)
	{
		if (Container && Container->DoorTrigger)
		{
			SetTriggerInactive(Container->DoorTrigger);
		}
	}
	
	// 현재 구역 Barrier 활성화
	int32 CurrentContainerIdx = GetContainerIndex(ActualFloorNumber);
	if (FloorContainers.IsValidIndex(CurrentContainerIdx) && FloorContainers[CurrentContainerIdx])
	{
		FloorContainers[CurrentContainerIdx]->EnableBarriers();
	}
	
	// 물리적으론 다음 구역으로
	ActualFloorNumber++;
	// 값 초기화
	CurrentClassNumber = 1;
	
	CreateFirstGradeClassroom();
	
	CleanupDistantClassrooms();

	int32 NextFloorForTrigger = ActualFloorNumber + 1; 
	int32 NextTriggerContainerIdx = GetContainerIndex(NextFloorForTrigger);
	
	for (int32 i = 0; i < FloorContainers.Num(); ++i)
	{
		if (FloorContainers[i] && FloorContainers[i]->DoorTrigger)
		{
			if (i == 0)
			{
				SetTriggerInactive(FloorContainers[i]->DoorTrigger);
			}
			else if (i == NextTriggerContainerIdx)
			{
				SetTriggerActive(FloorContainers[i]->DoorTrigger);
			}
			else
			{
				SetTriggerInactive(FloorContainers[i]->DoorTrigger);
			}
		}
	}

	OnFloorTransitionComplete(RequestingContainer);
}

void AClassroomManager::ResetToFirstGradeOnly(AFloorContainer* FirstGradeContainer)
{
	if (!IsValid(FirstGradeContainer))
	{
		return;
	}

	FirstGradeContainer->ClearPuzzle();

	EClassroomType NewType = SelectRandomClassroomType();
	FirstGradeContainer->SetClassroom(NewType, ClassroomBPMap);

	UE_LOG(LogTemp, Error, TEXT("배리어 : ResetToFirstGradeOnly"));
	FirstGradeContainer->DisableBarriers();

	FirstGradeContainer->ResetTrigger();

	ContainerLastUsedFloor.Add(FirstGradeContainer, 1);
}

void AClassroomManager::EnsureContainersBehindFirstGrade()
{
	if (FloorContainers.IsValidIndex(1))
	{
		AFloorContainer* C2 = FloorContainers[1];
		if (IsValid(C2))
		{
			C2->SetActorLocation(GetFloorPosition(2)); // (0, -5985, 0)
			C2->ClearPuzzle();
			C2->EnableBarriers();
			C2->ResetTrigger();
			ContainerLastUsedFloor.Add(C2, 2);
		}
	}

	if (FloorContainers.IsValidIndex(2))
	{
		AFloorContainer* C3 = FloorContainers[2];
		if (IsValid(C3))
		{
			C3->SetActorLocation(GetFloorPosition(3)); // (0, -5985*2, 0)
			C3->ClearPuzzle();
			C3->EnableBarriers();
			C3->ResetTrigger();
			ContainerLastUsedFloor.Add(C3, 3);
		}
	}
}

void AClassroomManager::CreateFirstGradeClassroom()
{
	int32 NextContainerIdx = GetContainerIndex(ActualFloorNumber);
	
	if (!FloorContainers.IsValidIndex(NextContainerIdx))
	{
		return;
	}
	else
	{
	}
	
	AFloorContainer* TargetContainer = FloorContainers[NextContainerIdx];
	if (!TargetContainer)
	{
		return;
	}
	
	if (TargetContainer->CurrentClassroomType != EClassroomType::None)
	{
		TargetContainer->ClearPuzzle();
	}
	
	if (ActualFloorNumber >= 5)
	{
		FVector NewPosition = GetFloorPosition(ActualFloorNumber);
		TargetContainer->SetActorLocation(NewPosition);
	}
	
	EClassroomType FirstGradeType = EClassroomType::Math;
	TargetContainer->SetClassroom(FirstGradeType, ClassroomBPMap);
	UE_LOG(LogTemp, Error, TEXT("배리어 : CreateFirstGradeClassroom"));
	TargetContainer->DisableBarriers();
	TargetContainer->ResetTrigger();
	
	SafeUpdateContainerUsage(TargetContainer, ActualFloorNumber);
	
		ActualFloorNumber, NextContainerIdx, *UEnum::GetValueAsString(FirstGradeType));
}

FVector AClassroomManager::GetFloorPosition(int32 FloorNumber) const
{
	FVector Result;
	
	if (FloorNumber <= 1) 
	{
		Result = FVector(0, 0, 0); // 1층 고정
	}
	else
	{
		// 2층부터: Y는 -5985씩 감소, Z는 0 고정 (기존 배치 패턴 유지)
		Result = FVector(0.f, -5985.f * (FloorNumber - 1), 0.f);
	}
	
	return Result;
}

int32 AClassroomManager::GetContainerIndex(int32 FloorNumber) const
{
	if (FloorNumber <= 1) return 0; // 1층 → [0] StartArea
    
	if (FloorNumber <= 4)
	{
		// 2층→[1], 3층→[2], 4층→[3]
		return FloorNumber - 1;
	}
	else
	{
		// 5층부터는 [1],[2],[3] 순환 재활용
		// 5층→[1], 6층→[2], 7층→[3], 8층→[1], ...
		return 1 + ((FloorNumber - 5) % 3);
	}
}

AFloorContainer* AClassroomManager::GetOldestContainer() const
{
	
	AFloorContainer* OldestContainer = nullptr;
	int32 OldestFloorNumber = ActualFloorNumber;
	
	for (auto& Pair : ContainerLastUsedFloor)
	{
		if (!Pair.Key || !IsValid(Pair.Key))
		{
			continue;
		}
		
		int32 ContainerIndex = GetContainerIndexForRecycled(Pair.Key);
		if (ContainerIndex == 0)
		{
			continue;
		}
		
		if (Pair.Value < OldestFloorNumber)
		{
			OldestFloorNumber = Pair.Value;
			OldestContainer = Pair.Key;
		}
	}
	
	return OldestContainer;
}

EClassroomType AClassroomManager::GetClassroomTypeForClass(int32 ClassNumber) const
{
	// 클래스 3,6,9,12... (3의 배수)는 랜덤
	if (ClassNumber % 3 == 0)
	{
		return SelectRandomClassroomType();
	}
	
	// 나머지는 고정 (현재는 수학만)
	return EClassroomType::Math;
}

// SelectRandomClassroomType 함수도 수정
EClassroomType AClassroomManager::SelectRandomClassroomType() const
{
	// 테스트용: 수학교실만 반환
	return EClassroomType::Math;
	
	// if (ClassroomBPMap.Num() == 0)
	// {
	// 	return EClassroomType::Math;
	// }
	//
	// TArray<EClassroomType> Keys;
	// ClassroomBPMap.GetKeys(Keys);
	// int32 RandomIndex = FMath::RandRange(0, Keys.Num() - 1);
	// EClassroomType SelectedType = Keys[RandomIndex];
	//
	// return SelectedType;
}

void AClassroomManager::ResetToFirstFloor()
{
	ActualFloorNumber = 1;
	
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerCharacter)
	{
		FVector FirstFloorPos = FVector(0, 0, 100); // 1층 + 오프셋
		PlayerCharacter->SetActorLocation(FirstFloorPos);
	}
	
	// 모든 컨테이너 초기 위치로 리셋
	for (int32 i = 0; i < FloorContainers.Num(); ++i)
	{
		if (!FloorContainers[i]) continue;
		
		FVector InitialPos = GetFloorPosition(i + 1);
		FloorContainers[i]->SetActorLocation(InitialPos);
		// 사용 기록 업데이트 (안전하게)
		SafeUpdateContainerUsage(FloorContainers[i], i + 1);
		
		if (i == 0) // Start 컨테이너: 교실 활성화하지만 트리거는 비활성화
		{
			EClassroomType NewType = SelectRandomClassroomType();
			FloorContainers[i]->SetClassroom(NewType, ClassroomBPMap);
			UE_LOG(LogTemp, Error, TEXT("배리어 : ResetToFirstFloor"));
			FloorContainers[i]->DisableBarriers();
			if (FloorContainers[i]->DoorTrigger)
			{
				FloorContainers[i]->DoorTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
		else if (i == 1) // 첫 번째 Loop 컨테이너만 트리거 활성화
		{
			FloorContainers[i]->ClearPuzzle();
			FloorContainers[i]->EnableBarriers();
			FloorContainers[i]->ResetTrigger();
			if (FloorContainers[i]->DoorTrigger)
			{
				FloorContainers[i]->DoorTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
		}
		else // 나머지 Loop 컨테이너들은 비활성화
		{
			FloorContainers[i]->ClearPuzzle();
			FloorContainers[i]->EnableBarriers();
			FloorContainers[i]->ResetTrigger();
			if (FloorContainers[i]->DoorTrigger)
			{
				FloorContainers[i]->DoorTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
	
}
