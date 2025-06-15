#include "MyGameModeBase.h"
#include "Engine/World.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "PassingThrough/Systems/LocalGlobalSubsystem.h"

AMyGameModeBase::AMyGameModeBase()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMyGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> FoundManagers;
    UGameplayStatics::GetAllActorsOfClass(this, AClassroomManager::StaticClass(), FoundManagers);

    if (FoundManagers.Num() > 0)
    {
        ClassroomManagerInstance = Cast<AClassroomManager>(FoundManagers[0]);
        
        // GlobalSubsystem에 등록
        ULocalGlobalSubsystem* GlobalSys = GetGameInstance()->GetSubsystem<ULocalGlobalSubsystem>();
        if (GlobalSys)
        {
            GlobalSys->RegisterClassroomManager(ClassroomManagerInstance);
        }
    }

    // 모든 스태틱 메시 액터 찾기
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
    
    // 각 액터의 모든 스태틱 메시 최적화
    for (AActor* Actor : AllActors)
    {
        if (Actor)
        {
            TArray<UStaticMeshComponent*> MeshComponents;
            Actor->GetComponents<UStaticMeshComponent>(MeshComponents);
            
            for (UStaticMeshComponent* MeshComp : MeshComponents)
            {
                if (MeshComp)
                {
                    MeshComp->SetCullDistance(2000.0f);
                    MeshComp->bCastDynamicShadow = false;
                }
            }
        }
    }

    /* 임시로 !!! */
    // 콘솔 변수로 직접 렌더링 설정 조정
    GEngine->Exec(GetWorld(), TEXT("r.Shadow.MaxResolution=256"));
    GEngine->Exec(GetWorld(), TEXT("r.Shadow.MaxCSMResolution=256"));
    GEngine->Exec(GetWorld(), TEXT("r.Shadow.RadiusThreshold=0.05"));
    GEngine->Exec(GetWorld(), TEXT("r.Shadow.DistanceScale=0.5"));
    GEngine->Exec(GetWorld(), TEXT("r.Shadow.CSM.MaxCascades=1"));
    GEngine->Exec(GetWorld(), TEXT("r.DistanceFieldShadowing=0"));
    GEngine->Exec(GetWorld(), TEXT("r.DistanceFieldAO=0"));

    // 그래픽 옵션 대폭 감소
    GEngine->Exec(GetWorld(), TEXT("sg.ResolutionQuality=50"));
    GEngine->Exec(GetWorld(), TEXT("sg.ViewDistanceQuality=0"));
    GEngine->Exec(GetWorld(), TEXT("sg.AntiAliasingQuality=0"));
    GEngine->Exec(GetWorld(), TEXT("sg.ShadowQuality=0"));
    GEngine->Exec(GetWorld(), TEXT("sg.PostProcessQuality=0"));
    GEngine->Exec(GetWorld(), TEXT("sg.TextureQuality=0"));
    GEngine->Exec(GetWorld(), TEXT("sg.EffectsQuality=0"));
    GEngine->Exec(GetWorld(), TEXT("sg.FoliageQuality=0"));

    StartPlay();
}

void AMyGameModeBase::StartPlay()
{
    Super::StartPlay();
}
