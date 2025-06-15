#include "ChalkBoardBase.h"
#include "../Core/CustomizedPlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "../Systems/LocalGlobalSubsystem.h"

AChalkBoardBase::AChalkBoardBase()
{
    PrimaryActorTick.bCanEverTick = true;

    ChalkBoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChalkBoardMesh"));
    RootComponent = ChalkBoardMesh;

    bOutlineActive = false;
}

void AChalkBoardBase::BeginPlay()
{
    Super::BeginPlay();

    if (!ChalkBoardMesh) return;

    ChalkBoardMesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Meshes/SM_ChalkBoard.SM_ChalkBoard")));
    // /Script/Engine.StaticMesh'/Game/Meshes/SM_ChalkBoard.SM_ChalkBoard'

    // Overlay 머티리얼 로딩
    UMaterialInterface* Mat = Cast<UMaterialInterface>(
        StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/Materials/Mat_Outline.Mat_Outline"))
    );

    if (!Mat)
    {
        UE_LOG(LogTemp, Error, TEXT("머티리얼 로드 실패"));
        return;
    }

    // 다이나믹 인스턴스 만들기
    DynamicOverlayMaterial = UMaterialInstanceDynamic::Create(Mat, this);
    if (!DynamicOverlayMaterial)
    {
        UE_LOG(LogTemp, Error, TEXT("다이나믹 머티리얼 생성 실패"));
        return;
    }

    // 슬롯 0에 강제 적용 (디버깅용)
    ChalkBoardMesh->SetMaterial(0, DynamicOverlayMaterial);

    // 시각화 강제 적용 -> 디버깅용!
    // DynamicOverlayMaterial->SetScalarParameterValue("OutlineIntensity", 1.0f);
    // DynamicOverlayMaterial->SetVectorParameterValue("BaseColor", FLinearColor::Green);
}

void AChalkBoardBase::SetOutlineActive(bool bActive)
{
    bOutlineActive = bActive;

    if (DynamicOverlayMaterial)
    {
        float Intensity = bActive ? 1.0f : 0.0f;
        DynamicOverlayMaterial->SetScalarParameterValue("OutlineIntensity", Intensity);
        // UE_LOG(LogTemp, Warning, TEXT("[ChalkBoard] 아웃라인 %s (강도 %.1f)"),
        //     bActive ? TEXT("ON") : TEXT("OFF"), Intensity);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ChalkBoard] DynamicOverlayMaterial 없음 - BeginPlay에서 세팅 안된듯 ㅇㅇ"));
    }
}

// void AChalkBoardBase::Tick(float DeltaTime)
// {
//     Super::Tick(DeltaTime);
// }

void AChalkBoardBase::SetProblemData(const FMathProblemData& NewData)
{
    ProblemData = NewData;

    FText QuestionText = FText::Format(
        FText::FromString(TEXT("Problem: {0} \n1. {1}\n2. {2}\n3. {3}\n4. {4}\n5. {5}\n")),
        FText::FromString(NewData.Question),
        FText::FromString(NewData.ChoiceA),
        FText::FromString(NewData.ChoiceB),
        FText::FromString(NewData.ChoiceC),
        FText::FromString(NewData.ChoiceD),
        FText::FromString(NewData.ChoiceE)
    );

    // 중앙 저장소에 저장
    if (ULocalGlobalSubsystem* GlobalSys = GetGameInstance()->GetSubsystem<ULocalGlobalSubsystem>())
    {
        GlobalSys->CurrentProblemData = NewData;  // 추가 필요
    }

    SetProblemText(QuestionText);
}

void AChalkBoardBase::SetProblemText(const FText& NewText)
{
    ProblemText = NewText;

    // 실제 TextRender 컴포넌트 찾아서 설정
    UTextRenderComponent* TextRender = FindComponentByClass<UTextRenderComponent>();
    if (TextRender)
    {
        TextRender->SetText(NewText);
        UE_LOG(LogTemp, Log, TEXT("칠판 텍스트 설정 완료: %s"), *NewText.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("TextRenderComponent를 찾을 수 없음!"));
    }

    // // 오브젝트 이름 로그 출력
    // UE_LOG(LogTemp, Warning, TEXT("현재 오브젝트 이름: %s"), *GetName());
    //
    // // 더 자세한 정보를 위해 클래스 이름도 함께 출력
    // UE_LOG(LogTemp, Warning, TEXT("현재 오브젝트: %s (클래스: %s)"), 
    //        *GetName(), *GetClass()->GetName());
}

void AChalkBoardBase::Interact(ACustomizedPlayerController* PC)
{
    if (PC)
    {
        PC->ShowQuestionUI();
    }
}
