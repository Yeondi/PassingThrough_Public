#include "FloorContainer.h"
#include "../UI/ChalkBoardBase.h"
#include "Classroom.h"
#include "../Core/MyPlayerCharacter.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "PassingThrough/Core/CustomizedPlayerController.h"
#include "PassingThrough/Systems/LocalGlobalSubsystem.h"

AFloorContainer::AFloorContainer()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = RootScene;

	FloorBarrier = CreateDefaultSubobject<UBoxComponent>(TEXT("FloorBarrier"));
	FloorBarrier->SetupAttachment(RootComponent);
	FloorBarrier->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FloorBarrier->SetHiddenInGame(true);

	DoorBarrier = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorBarrier"));
	DoorBarrier->SetupAttachment(RootComponent);
	DoorBarrier->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DoorBarrier->SetHiddenInGame(true);

	DoorTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorTrigger"));
	DoorTrigger->SetupAttachment(RootComponent);
	DoorTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DoorTrigger->SetGenerateOverlapEvents(true);
	DoorTrigger->SetCollisionProfileName(TEXT("Trigger"));
	DoorTrigger->SetRelativeLocation(FVector(-57.f, -4295.f, 0.f));
	DoorTrigger->OnComponentBeginOverlap.AddDynamic(this, &AFloorContainer::OnPlayerEnterTrigger);
}

void AFloorContainer::BeginPlay()
{
	Super::BeginPlay();
}

// 트리거 상태 관리 함수
void AFloorContainer::SetTriggerProcessing(bool bProcessing)
{
	bTriggerProcessing = bProcessing;

	if (bProcessing)
	{
		GetWorldTimerManager().SetTimer(
			TriggerTimeoutTimer,
			this,
			&AFloorContainer::OnTriggerTimeout,
			5.0f,
			false
		);
		UE_LOG(LogTemp, Log, TEXT("[%s] 트리거 처리 시작 (5초 타임아웃)"), *GetName());
	}
	else
	{
		// 타이머 제거
		GetWorldTimerManager().ClearTimer(TriggerTimeoutTimer);
		UE_LOG(LogTemp, Log, TEXT("[%s] 트리거 처리 완료"), *GetName());
	}
}

// 타임아웃 안전장치
void AFloorContainer::OnTriggerTimeout()
{
	bTriggerProcessing = false;
}

void AFloorContainer::OnPlayerEnterTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 플레이어만 처리
	AMyPlayerCharacter* PlayerCharacter = Cast<AMyPlayerCharacter>(OtherActor);
	if (!PlayerCharacter)
	{
		return;
	}

	// 개별 트리거 중복 방지 체크
	if (bTriggerProcessing)
	{
		return;
	}

	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector ContainerLocation = GetActorLocation();
	FVector TriggerWorldLocation = DoorTrigger->GetComponentLocation();

	SetTriggerProcessing(true);

	// 정답/오답 확인
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	bool bIsCorrect = false;
	if (PlayerPawn)
	{
		ACustomizedPlayerController* PC = Cast<ACustomizedPlayerController>(PlayerPawn->GetController());
		if (PC)
		{
			bIsCorrect = PC->bLastAnswerCorrect;
		}
	}

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (GameInstance)
	{
		ULocalGlobalSubsystem* Subsystem = GameInstance->GetSubsystem<ULocalGlobalSubsystem>();
		if (Subsystem && Subsystem->ClassroomManagerRef)
		{
			// 트리거를 즉시 비활성화 (중복 방지)
			if (DoorTrigger)
			{
				DoorTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				// EnableBarriers();
			}

			// 정답/오답 모두 동일한 로직 사용으로 변경
			Subsystem->ClassroomManagerRef->NextFloorWithContainer(this);

			if (!bIsCorrect)
			{
				Subsystem->ClassroomManagerRef->ResetCurrentClassNumber();
			}

			return;
		}
		else
		{
			// 실패 시 플래그 해제
			SetTriggerProcessing(false);
		}
	}
}

void AFloorContainer::ClearPuzzle()
{
	if (IsValid(SpawnedPuzzle))
	{
		// 액터가 이미 파괴 중인지 확인
		if (!SpawnedPuzzle->IsPendingKillPending())
		{
			SpawnedPuzzle->Destroy();
		}
		SpawnedPuzzle = nullptr;
	}
	CurrentClassroomType = EClassroomType::None;
}

void AFloorContainer::SetClassroom(EClassroomType NewType, const TMap<EClassroomType, TSubclassOf<AClassroom>>& ClassroomBPMap)
{
	if (IsValid(SpawnedPuzzle))
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 기존 교실 강제 삭제: %s"), *GetName(), *SpawnedPuzzle->GetName());
		SpawnedPuzzle->Destroy();
		SpawnedPuzzle = nullptr;
	}
	CurrentClassroomType = EClassroomType::None;

	CurrentClassroomType = NewType;

	if (!ClassroomBPMap.Contains(NewType))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] 교실 타입 %s을 ClassroomBPMap에서 찾을 수 없음"),
			*GetName(), *UEnum::GetValueAsString(NewType));

		// 🔍 디버깅: 현재 맵에 있는 키들 출력
		UE_LOG(LogTemp, Warning, TEXT("현재 ClassroomBPMap에 등록된 타입들:"));
		for (auto& Pair : ClassroomBPMap)
		{
			UE_LOG(LogTemp, Warning, TEXT("   - %s"), *UEnum::GetValueAsString(Pair.Key));
		}
		return;
	}

	TSubclassOf<AClassroom> ClassroomClass = ClassroomBPMap[NewType];
	if (!ClassroomClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] ClassroomClass가 nullptr"), *GetName());
		return;
	}

	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	if (MarkingActor && IsValid(MarkingActor))
	{
		SpawnLocation = MarkingActor->GetActorLocation();
		SpawnRotation = MarkingActor->GetActorRotation();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 타임스탬프로 완전 고유한 이름 생성
	FString UniqueName = FString::Printf(TEXT("%s_Classroom_%s_%lld"),
		*GetName(),
		*UEnum::GetValueAsString(NewType),
		FDateTime::Now().GetTicks());
	SpawnParams.Name = FName(*UniqueName);

	// SpawnParams.Owner 설정으로 추가 안전성
	SpawnParams.Owner = this;


	SpawnedPuzzle = GetWorld()->SpawnActor<AClassroom>(ClassroomClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpawnedPuzzle)
	{
		*GetName(), * UEnum::GetValueAsString(NewType), * SpawnLocation.ToString());

		// 칠판 찾기 및 문제 설정
		TArray<AActor*> ChalkBoards;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChalkBoardBase::StaticClass(), ChalkBoards);

		AChalkBoardBase* FoundChalkBoard = nullptr;
		float MinDistance = 1000.0f; // 최대 검색 거리

		for (AActor* Actor : ChalkBoards)
		{
			AChalkBoardBase* ChalkBoard = Cast<AChalkBoardBase>(Actor);
			if (ChalkBoard)
			{
				AActor* ChalkBoardParent = ChalkBoard->GetAttachParentActor();
				float Distance = FVector::Dist(ChalkBoard->GetActorLocation(), SpawnLocation);

				// 부모가 스폰된 교실이면 즉시 선택
				if (ChalkBoardParent == SpawnedPuzzle)
				{
					FoundChalkBoard = ChalkBoard;
					break;
				}

				// 그렇지 않으면 가장 가까운 칠판 선택
				if (Distance < MinDistance && !FoundChalkBoard)
				{
					FoundChalkBoard = ChalkBoard;
					MinDistance = Distance;
				}
			}
		}

		if (FoundChalkBoard && MathProblemsDataTable)
		{
			TArray<FMathProblemData*> AllRows;
			MathProblemsDataTable->GetAllRows<FMathProblemData>(TEXT(""), AllRows);

			if (AllRows.Num() > 0)
			{
				// 랜덤 선택
				int32 RandomIndex = FMath::RandRange(0, AllRows.Num() - 1);
				FMathProblemData* SelectedProblem = AllRows[RandomIndex];

				FoundChalkBoard->SetProblemData(*SelectedProblem);
			}
		}
	}
	else
	{
		CurrentClassroomType = EClassroomType::None; // 실패 시 타입 리셋
	}
}

void AFloorContainer::EnableBarriers()
{
	UE_LOG(LogTemp, Error, TEXT("\n %s 배리어 활성화\n"), *GetActorLabel());

	// FloorBarrier->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// DoorBarrier->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OnBarrierStateChanged();

	// DoorBarrier->SetCollisionProfileName("BlockAll");
}

void AFloorContainer::DisableBarriers()
{
	UE_LOG(LogTemp, Error, TEXT("\n %s 배리어 비활성화\n"), *GetActorLabel());
	// FloorBarrier->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// DoorBarrier->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// DoorBarrier->SetCollisionProfileName("NoCollision");
}

void AFloorContainer::ResetTrigger()
{
	if (!ClassroomManager)
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			if (ULocalGlobalSubsystem* Subsystem = GameInstance->GetSubsystem<ULocalGlobalSubsystem>())
			{
				ClassroomManager = Subsystem->GetClassroomManager();
			}
		}
	}

	if (!ClassroomManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ClassroomManager가 존재하지 않음!"));
		return;
	}

	SetTriggerProcessing(false);
}
