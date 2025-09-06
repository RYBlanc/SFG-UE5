// Project Visible - Level Design Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StorySystemTypes.h"
#include "LevelDesignTypes.generated.h"

/**
 * Level types for different game areas
 */
UENUM(BlueprintType)
enum class ELevelType : uint8
{
    RealityExploration       UMETA(DisplayName = "Reality Exploration"),      // 現実世界探索
    DreamLandscape          UMETA(DisplayName = "Dream Landscape"),           // 夢の精神地形
    BoundaryTransition      UMETA(DisplayName = "Boundary Transition"),       // 境界移行エリア
    InvestigationZone       UMETA(DisplayName = "Investigation Zone"),        // 調査特化エリア
    DialogueSpace           UMETA(DisplayName = "Dialogue Space"),            // 対話専用空間
    MemoryVault             UMETA(DisplayName = "Memory Vault"),              // 記憶保管庫
    VirtueTest              UMETA(DisplayName = "Virtue Test"),               // 徳試験エリア
    ExperimentChamber       UMETA(DisplayName = "Experiment Chamber"),        // 実験室
    Tutorial                UMETA(DisplayName = "Tutorial")                   // チュートリアル
};

/**
 * Environment themes for different locations
 */
UENUM(BlueprintType)
enum class EEnvironmentTheme : uint8
{
    ModernTokyo             UMETA(DisplayName = "Modern Tokyo"),              // 現代東京
    SereneLakeKaruizawa     UMETA(DisplayName = "Serene Lake Karuizawa"),     // 軽井沢の湖
    TechnicalKagawa         UMETA(DisplayName = "Technical Kagawa"),          // 香川の技術施設
    MysticYamagata          UMETA(DisplayName = "Mystic Yamagata"),           // 神秘的な山形
    QuantumDreamscape       UMETA(DisplayName = "Quantum Dreamscape"),        // 量子夢世界
    BoundaryDissolution     UMETA(DisplayName = "Boundary Dissolution"),      // 境界崩壊空間
    NicomacoLab             UMETA(DisplayName = "Nicomaco Lab"),              // ニコマコ研究所
    MemoryFragment          UMETA(DisplayName = "Memory Fragment"),           // 記憶の断片
    VirtueGarden            UMETA(DisplayName = "Virtue Garden"),             // 徳の庭園
    ExperimentRoom          UMETA(DisplayName = "Experiment Room")            // 実験室
};

/**
 * Level layout patterns
 */
UENUM(BlueprintType)
enum class ELevelLayout : uint8
{
    Linear                  UMETA(DisplayName = "Linear"),                    // 直線的
    OpenWorld               UMETA(DisplayName = "Open World"),               // オープンワールド
    Hub                     UMETA(DisplayName = "Hub"),                      // ハブ型
    Maze                    UMETA(DisplayName = "Maze"),                     // 迷路型
    Vertical                UMETA(DisplayName = "Vertical"),                 // 垂直型
    Procedural              UMETA(DisplayName = "Procedural"),               // 手続き型
    Asymmetric              UMETA(DisplayName = "Asymmetric"),               // 非対称型
    Circular                UMETA(DisplayName = "Circular"),                 // 円形
    Grid                    UMETA(DisplayName = "Grid"),                     // グリッド型
    Organic                 UMETA(DisplayName = "Organic")                   // 有機的
};

/**
 * Interaction objects for level design
 */
UENUM(BlueprintType)
enum class EInteractionObjectType : uint8
{
    Evidence                UMETA(DisplayName = "Evidence"),                 // 証拠品
    Document                UMETA(DisplayName = "Document"),                 // 文書
    Terminal                UMETA(DisplayName = "Terminal"),                 // 端末
    MemoryOrb               UMETA(DisplayName = "Memory Orb"),               // 記憶オーブ
    VirtueStone             UMETA(DisplayName = "Virtue Stone"),             // 徳の石
    QuantumNode             UMETA(DisplayName = "Quantum Node"),             // 量子ノード
    DialogueTrigger         UMETA(DisplayName = "Dialogue Trigger"),         // 対話トリガー
    Experiment              UMETA(DisplayName = "Experiment"),               // 実験装置
    Portal                  UMETA(DisplayName = "Portal"),                   // ポータル
    Collectible             UMETA(DisplayName = "Collectible"),              // 収集アイテム
    Clue                    UMETA(DisplayName = "Clue"),                     // 手がかり
    Switch                  UMETA(DisplayName = "Switch"),                   // スイッチ
    Door                    UMETA(DisplayName = "Door"),                     // ドア
    Elevator                UMETA(DisplayName = "Elevator")                  // エレベーター
};

/**
 * Weather and atmospheric conditions
 */
UENUM(BlueprintType)
enum class EWeatherCondition : uint8
{
    Clear                   UMETA(DisplayName = "Clear"),                    // 晴天
    Cloudy                  UMETA(DisplayName = "Cloudy"),                   // 曇り
    Rainy                   UMETA(DisplayName = "Rainy"),                    // 雨
    Stormy                  UMETA(DisplayName = "Stormy"),                   // 嵐
    Foggy                   UMETA(DisplayName = "Foggy"),                    // 霧
    Snowy                   UMETA(DisplayName = "Snowy"),                    // 雪
    Surreal                 UMETA(DisplayName = "Surreal"),                  // 超現実的
    Dreamlike               UMETA(DisplayName = "Dreamlike"),                // 夢のような
    Ethereal                UMETA(DisplayName = "Ethereal"),                 // 幻想的
    Quantum                 UMETA(DisplayName = "Quantum"),                  // 量子的
    Glitched                UMETA(DisplayName = "Glitched"),                 // グリッチ
    Dissolving              UMETA(DisplayName = "Dissolving")                // 溶解中
};

/**
 * Level lighting configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FLevelLightingConfig
{
    GENERATED_BODY()

    FLevelLightingConfig()
    {
        TimeOfDay = 12.0f;
        LightIntensity = 1.0f;
        AmbientBrightness = 0.3f;
        ShadowIntensity = 0.7f;
        bUseDynamicLighting = true;
        bUseVolumetricFog = false;
        FogDensity = 0.1f;
        AtmosphericPerspective = 1.0f;
        ColorTemperature = 6500.0f;
        SaturationMultiplier = 1.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float TimeOfDay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float LightIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float AmbientBrightness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float ShadowIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    bool bUseDynamicLighting;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    bool bUseVolumetricFog;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float FogDensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float AtmosphericPerspective;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float ColorTemperature;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float SaturationMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    FLinearColor AmbientColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    FLinearColor FogColor;
};

/**
 * Interaction object definition
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FInteractionObject
{
    GENERATED_BODY()

    FInteractionObject()
    {
        ObjectID = TEXT("");
        ObjectName = TEXT("");
        ObjectType = EInteractionObjectType::Evidence;
        bIsInteractable = true;
        bIsCollected = false;
        bRequiresSpecialCondition = false;
        InteractionRange = 200.0f;
        ImportanceLevel = 1;
        RewardPoints = 10;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    FString ObjectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    FString ObjectName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    EInteractionObjectType ObjectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    bool bIsInteractable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    bool bIsCollected;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    bool bRequiresSpecialCondition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    float InteractionRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    int32 ImportanceLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    int32 RewardPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    FRotator Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    FVector Scale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    TArray<FString> RequiredConditions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    TArray<FString> UnlockedClues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
    TMap<FString, FString> ObjectData;
};

/**
 * Level generation parameters
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FLevelGenerationParams
{
    GENERATED_BODY()

    FLevelGenerationParams()
    {
        LevelSize = FVector(2000.0f, 2000.0f, 500.0f);
        TerrainComplexity = 0.5f;
        ObjectDensity = 0.3f;
        InteractionObjectCount = 10;
        bUseProcedural = false;
        bUseSymmetry = false;
        RandomSeed = 12345;
        DetailLevel = 1.0f;
        PerformanceTarget = 60.0f;
        LevelLayout = ELevelLayout::OpenWorld;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    FVector LevelSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    float TerrainComplexity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    float ObjectDensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    int32 InteractionObjectCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    bool bUseProcedural;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    bool bUseSymmetry;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    int32 RandomSeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    float DetailLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    float PerformanceTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    ELevelLayout LevelLayout;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    TArray<EInteractionObjectType> AllowedObjectTypes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    TMap<FString, float> GenerationRules;
};

/**
 * Level definition structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FLevelDefinition
{
    GENERATED_BODY()

    FLevelDefinition()
    {
        LevelID = TEXT("");
        LevelName = TEXT("");
        LevelType = ELevelType::RealityExploration;
        Theme = EEnvironmentTheme::ModernTokyo;
        StoryLocation = EStoryLocation::Tokyo;
        bIsLocked = true;
        bIsCompleted = false;
        CompletionPercentage = 0.0f;
        MinimumPlayTime = 15.0f;
        RecommendedPlayTime = 30.0f;
        DifficultyLevel = 1;
        RequiredStoryProgress = 0.0f;
        Weather = EWeatherCondition::Clear;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FString LevelID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FString LevelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    ELevelType LevelType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    EEnvironmentTheme Theme;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    EStoryLocation StoryLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    bool bIsLocked;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    bool bIsCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    float CompletionPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    float MinimumPlayTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    float RecommendedPlayTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    int32 DifficultyLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    float RequiredStoryProgress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    EWeatherCondition Weather;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FLevelLightingConfig LightingConfig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FLevelGenerationParams GenerationParams;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    TArray<FInteractionObject> InteractionObjects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    TArray<FString> RequiredItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    TArray<FString> UnlockedFeatures;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    TMap<FString, FString> LevelData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FString LevelAssetPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    TArray<FString> AssociatedCharacters;
};

/**
 * Level transition configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FLevelTransition
{
    GENERATED_BODY()

    FLevelTransition()
    {
        FromLevelID = TEXT("");
        ToLevelID = TEXT("");
        TransitionType = TEXT("Fade");
        Duration = 2.0f;
        bRequiresCondition = false;
        bIsReversible = true;
        bPreloadDestination = true;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    FString FromLevelID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    FString ToLevelID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    FString TransitionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    bool bRequiresCondition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    bool bIsReversible;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    bool bPreloadDestination;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    TArray<FString> RequiredConditions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    FString TriggerLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    TMap<FString, FString> TransitionData;
};
