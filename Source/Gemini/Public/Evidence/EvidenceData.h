#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EvidenceData.generated.h"

UENUM(BlueprintType)
enum class EEvidenceType : uint8
{
    Physical    UMETA(DisplayName = "物的証拠"),
    Testimony   UMETA(DisplayName = "証言"),
    Document    UMETA(DisplayName = "書類"),
    Audio       UMETA(DisplayName = "音声記録"),
    Visual      UMETA(DisplayName = "映像記録"),
    Abstract    UMETA(DisplayName = "抽象的概念")
};

UENUM(BlueprintType)
enum class EEvidenceReliability : uint8
{
    Certain     UMETA(DisplayName = "確実"),
    Probable    UMETA(DisplayName = "可能性高"),
    Uncertain   UMETA(DisplayName = "不確実"),
    Suspicious  UMETA(DisplayName = "疑わしい")
};

USTRUCT(BlueprintType)
struct GEMINI_API FEvidenceConnection
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
    FString ConnectedEvidenceID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
    FString RelationshipType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
    float ConnectionStrength = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
    bool bIsConfirmed = false;
};

USTRUCT(BlueprintType)
struct GEMINI_API FEvidenceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FString EvidenceID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FString Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    EEvidenceType Type = EEvidenceType::Physical;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Info")
    EEvidenceReliability Reliability = EEvidenceReliability::Uncertain;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TSoftObjectPtr<UTexture2D> EvidenceImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FLinearColor NodeColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metadata")
    FDateTime DiscoveryTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metadata")
    FString LocationFound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metadata")
    FString RelatedCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas Position")
    FVector2D CanvasPosition = FVector2D::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connections")
    TArray<FEvidenceConnection> Connections;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsOnCanvas = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsHighlighted = false;

    // 思考キャンバスの段階に応じた表示制御
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas Corruption")
    bool bShowInReality = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas Corruption")
    bool bShowInDream = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas Corruption")
    float CorruptionLevel = 0.0f;  // 0.0 = 正常, 1.0 = 完全に腐敗
};
