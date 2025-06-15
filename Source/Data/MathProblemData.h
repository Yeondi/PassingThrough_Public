#pragma once

#include "CoreMinimal.h"
#include "ClassroomTypes.h" // EClassroomType의 완전한 정의가 포함된 헤더
#include "Engine/DataTable.h"
#include "MathProblemData.generated.h"

UENUM(BlueprintType)
enum class EProblemDifficulty : uint8
{
    None    UMETA(DisplayName = "None"),
    Easy    UMETA(DisplayName = "Easy"),
    Medium  UMETA(DisplayName = "Medium"),
    Hard    UMETA(DisplayName = "Hard")
};

USTRUCT(BlueprintType)
struct FMathProblemData : public FTableRowBase
{
    GENERATED_BODY()

    FMathProblemData()
        : ClassroomType(EClassroomType::None),
        Difficulty(EProblemDifficulty::None),
        Question(TEXT("")),
        ChoiceA(TEXT("")),
        ChoiceB(TEXT("")),
        ChoiceC(TEXT("")),
        ChoiceD(TEXT("")),
        ChoiceE(TEXT("")),
        CorrectAnswerIndex(0),
        bIsMultipleChoice(false)
    {
    }

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EClassroomType ClassroomType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EProblemDifficulty Difficulty;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Question;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ChoiceA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ChoiceB;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ChoiceC;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ChoiceD;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ChoiceE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CorrectAnswerIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsMultipleChoice;
};
