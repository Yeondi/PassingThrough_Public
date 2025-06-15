// CustomizedPlayerController.cpp

// #include "Data/NinjaInputSetupDataAsset.h"
// #include "Components/TextBlock.h"
// #include "../UI/InteractionPromptWidget.h"
// #include "../UI/InteractMessageWidget.h"
#include "CustomizedPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/NinjaInputManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "../Data/InteractInterface.h"
#include "../UI/ChalkBoardBase.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "../Systems/LocalGlobalSubsystem.h"
#include "Kismet/GameplayStatics.h"

ACustomizedPlayerController::ACustomizedPlayerController(const FObjectInitializer& ObjectInitializer)
{
	static const FName EquipmentComponentName = TEXT("InputManager");
	InputManager = CreateDefaultSubobject<UNinjaInputManagerComponent>(EquipmentComponentName);

	// Ray Trace 관련 초기화
	HitComponent = nullptr;
	LastHitActor = nullptr;
	LastHitComponent = nullptr;
	InteractionDistance = 500.0f; // 기본값 500 유닛 (5미터)

	// 마지막 상호작용 칠판 초기화
	LastInteractedChalkBoard = nullptr;

	// 매 프레임 호출되도록 설정
	PrimaryActorTick.bCanEverTick = true;
}

void ACustomizedPlayerController::PlayFootstepSound(float InputMagnitude)
{
	USoundBase* SoundToPlay = GetRandomFootstepSound();
	
	if (!SoundToPlay || !GetPawn())
		return;
		
	float CurrentTime = GetWorld()->GetTimeSeconds();
	
	if (CurrentTime - LastFootstepTime >= FootstepInterval)
	{
		// 실제 발 위치 계산
		FVector PawnLocation = GetPawn()->GetActorLocation();
		FVector FootLocation = PawnLocation + FVector(0, 0, -88.0f);
		
		// 입력 강도에 따른 볼륨 조절
		float Volume = FMath::Clamp(InputMagnitude * 0.7f, 0.4f, 1.0f);
		
		// 자연스러운 랜덤 피치
		float RandomPitch = FMath::RandRange(0.9f, 1.1f);
		
		// Location에서 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			SoundToPlay,
			FootLocation,
			Volume,
			RandomPitch
		);
		
		LastFootstepTime = CurrentTime;
		
		UE_LOG(LogTemp, Log, TEXT("👟 Footstep: %s (Vol: %.2f)"), 
			*SoundToPlay->GetName(), Volume);
	}
}

USoundBase* ACustomizedPlayerController::GetRandomFootstepSound()
{
	// 우선순위: 배열 > 단일 사운드 > nullptr
	if (FootstepSounds.Num() > 0)
	{
		// 최근에 재생한 사운드 제외 (연속 반복 방지)
		static int32 LastPlayedIndex = -1;
		int32 RandomIndex;
		
		if (FootstepSounds.Num() > 1)
		{
			do {
				RandomIndex = FMath::RandRange(0, FootstepSounds.Num() - 1);
			} while (RandomIndex == LastPlayedIndex);
			
			LastPlayedIndex = RandomIndex;
		}
		else
		{
			RandomIndex = 0;
		}
		
		return FootstepSounds[RandomIndex];
	}
	
	return FootstepSound; // 폴백
}


void ACustomizedPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 위젯 인스턴스가 없을 때만 생성
	if (UserWidgetClass)
	{
		if (!GameUIManager) // 이건 그냥 BP에서 할당 안 했을 때
		{
			GameUIManager = CreateWidget<UGameUIManager>(this, UserWidgetClass);
		}
	
		if (GameUIManager && !GameUIManager->IsInViewport())
		{
			GameUIManager->AddToViewport();
			GameUIManager->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ACustomizedPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACustomizedPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// 추가 입력 바인딩은 여기에 구현
}

void ACustomizedPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	// 매 프레임마다 Ray Trace 수행
	PerformRayTrace();

	// 상호작용 가능한 객체 검출
	CheckForInteractables();

	// 🎵 이동 감지 및 발걸음 사운드 재생
	if (InputManager)
	{
		FVector LastInputVector = InputManager->GetLastInputVector();
		float InputMagnitude = LastInputVector.Size();
		
		// 이동 중일 때 발걸음 사운드 재생
		if (InputMagnitude > MovementThreshold)
		{
			PlayFootstepSound(InputMagnitude);
		}
	}

}

void ACustomizedPlayerController::PerformRayTrace()
{
	if (!GetPawn())
		return;

	UCameraComponent* PlayerCamera = nullptr;
	TArray<UCameraComponent*> Cameras;
	GetPawn()->GetComponents<UCameraComponent>(Cameras);

	if (Cameras.Num() > 0)
	{
		PlayerCamera = Cameras[0];
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("카메라 컴포넌트를 찾을 수 없습니다."));
		return;
	}

	FVector CameraLocation = PlayerCamera->GetComponentLocation();
	FRotator CameraRotation = PlayerCamera->GetComponentRotation();
	FVector ForwardVector = CameraRotation.Vector();
	FVector Start = CameraLocation;
	FVector End = Start + (ForwardVector * InteractionDistance);

	// 디버그 라인
	// DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.0f, 0, 1.0f);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetPawn());

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

	if (bHit)
	{
		ProcessRayTraceResult(HitResult);
	}
	else
	{
		// 히트가 없을 경우 이전 히트 정보 초기화
		HitComponent = nullptr;
		LastHitActor = nullptr;
		LastHitComponent = nullptr;

		// 이전에 하이라이트한 칠판이 있다면 하이라이트 해제
		if (LastInteractedChalkBoard)
		{
			LastInteractedChalkBoard->SetOutlineActive(false);
			LastInteractedChalkBoard = nullptr;
		}
	}
}


void ACustomizedPlayerController::ProcessRayTraceResult(const FHitResult& HitResult)
{
	if (HitResult.GetComponent())
	{
		// 이전 칠판 아웃라인 비활성화
		if (LastInteractedChalkBoard && LastInteractedChalkBoard != Cast<AChalkBoardBase>(HitResult.GetActor()))
		{
			LastInteractedChalkBoard->SetOutlineActive(false);
		}

		// 히트 컴포넌트 저장
		HitComponent = HitResult.GetComponent();
		LastHitActor = HitResult.GetActor();
		LastHitComponent = HitResult.GetComponent();

		// 칠판인 경우 아웃라인 활성화
		AChalkBoardBase* ChalkBoard = Cast<AChalkBoardBase>(LastHitActor);
		if (ChalkBoard)
		{
			ChalkBoard->SetOutlineActive(true);
			LastInteractedChalkBoard = ChalkBoard;

			// InteractMessageWidget->AddToViewport();
		}
	}
}

void ACustomizedPlayerController::CheckForInteractables()
{
	// Ray Trace에서 히트된 액터가 있는지 확인
	if (LastHitActor && HitComponent)
	{
		AChalkBoardBase* CurrentChalkBoard = Cast<AChalkBoardBase>(LastHitActor);
		if (LastInteractedChalkBoard && LastInteractedChalkBoard != CurrentChalkBoard)
		{
			LastInteractedChalkBoard->SetOutlineActive(false);
		}

		if (LastHitActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
		{
			FText InteractionText = IInteractInterface::Execute_GetInteractionText(LastHitActor);
		}
		//else
		//{
			// 상호작용 불가능한 객체인 경우 메시지 숨김
			// ShowInteractMessage(false);
			// UE_LOG(LogTemp,Warning,TEXT("상호작용 불가!"))
		//}

		// 특정 클래스인지 확인 (칠판인 경우 추가 처리)
		UClass* ChalkBoardClass = StaticLoadClass(AActor::StaticClass(), nullptr,
		                                          TEXT("/Game/Blueprints/BP_ChalkBoard.BP_ChalkBoard_C"));

		if (ChalkBoardClass && LastHitActor->IsA(ChalkBoardClass))
		{
			AChalkBoardBase* ChalkBoard = Cast<AChalkBoardBase>(LastHitActor);
			if (ChalkBoard)
			{
				ChalkBoard->SetOutlineActive(true);
				LastInteractedChalkBoard = ChalkBoard;

			}
		}
	}
}

void ACustomizedPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Log, TEXT("Possessed Pawn: %s"), *GetNameSafe(InPawn));
}

void ACustomizedPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	UE_LOG(LogTemp, Log, TEXT("UnPossessed Pawn"));
}

void ACustomizedPlayerController::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other,
                                            class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
                                            FVector HitNormal,
                                            FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	UE_LOG(LogTemp, Warning, TEXT("Hit with: %s at %s"), *Other->GetName(), *HitLocation.ToString());
}

UNinjaInputManagerComponent* ACustomizedPlayerController::GetOwnerInputManager() const
{
	return InputManager;
}

void ACustomizedPlayerController::ShowUI()
{
	if (GameUIManager)
	{
		GameUIManager->SetVisibility(ESlateVisibility::Visible);
		SetUIInputMode();
	}
}

void ACustomizedPlayerController::HideUI()
{
	if (GameUIManager && GameUIManager->GetVisibility() == ESlateVisibility::Visible)
	{
		GameUIManager->SetVisibility(ESlateVisibility::Collapsed);
		SetGameInputMode();
	}
}

void ACustomizedPlayerController::ToggleUI()
{
	if (GameUIManager)
	{
		if (GameUIManager->GetVisibility() == ESlateVisibility::Visible)
		{
			HideUI();
		}
		else
		{
			ShowUI();
		}
	}
}

void ACustomizedPlayerController::ShowQuestionUI()
{
	if (GameUIManager)
	{
		// 여기 Data더미임. 나중에 지울것
		ULocalGlobalSubsystem* GlobalSys = GetGameInstance()->GetSubsystem<ULocalGlobalSubsystem>();
		FMathProblemData& newData = GlobalSys->CurrentProblemData;

		GameUIManager->ShowQuestionUI(newData);
		//GameUIManager->ShowQuestionUI(Data);
		SetUIInputMode();
	}
}

void ACustomizedPlayerController::HandleChoiceSelected(int32 ChoiceIndex)
{
	if (GameUIManager)
	{
		GameUIManager->HandleChoiceSelected(ChoiceIndex);
	}
}

void ACustomizedPlayerController::SetUIInputMode()
{
	if (!GameUIManager)
		return;

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(GameUIManager->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void ACustomizedPlayerController::SetGameInputMode()
{
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

void ACustomizedPlayerController::OnUIReady()
{
	// Slate 위젯이 살아있을 때만 포커싱
	if (!GameUIManager) return;

	TSharedPtr<SWidget> FocusWidget = GameUIManager->TakeWidget();

	if (FocusWidget.IsValid())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(FocusWidget.ToSharedRef());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
}

void ACustomizedPlayerController::DoNothing()
{
	UE_LOG(LogTemp, Log, TEXT("말 그대로 아무것도 안하는 함수."));
}
