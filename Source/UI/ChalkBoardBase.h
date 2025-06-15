#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Data/MathProblemData.h"
#include "ChalkBoardBase.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class ACustomizedPlayerController;

UCLASS()
class PASSINGTHROUGH_API AChalkBoardBase : public AActor
{
    GENERATED_BODY()

public:
    AChalkBoardBase();

    virtual void BeginPlay() override;
    // virtual void Tick(float DeltaTime) override;

    // 수학 문제 데이터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Math Problem")
    FMathProblemData ProblemData;

    UFUNCTION(BlueprintCallable, Category = "Math Problem")
    FMathProblemData& GetProblemData() { return ProblemData; }

    UFUNCTION(BlueprintCallable, Category = "Math Problem")
    void SetProblemData(const FMathProblemData& NewData);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Math Problem")
    FText ProblemText;

    UFUNCTION(BlueprintCallable, Category = "Math Problem")
    void SetProblemText(const FText& NewText);

    UFUNCTION(BlueprintCallable, Category = "Math Problem")
    FText& GetProblemText() { return ProblemText; }

    // 상호작용
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void Interact(ACustomizedPlayerController* PC);

    // 칠판 메시
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ChalkBoardMesh;

    // 아웃라인 표시
    UFUNCTION(BlueprintCallable, Category = "Appearance")
    void SetOutlineActive(bool bActive);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Appearance")
    bool bOutlineActive;

    // 머티리얼
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    UMaterialInterface* OverlayMaterial;

private:
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicOverlayMaterial;
};
