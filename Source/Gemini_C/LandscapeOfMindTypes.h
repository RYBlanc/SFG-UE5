// Project Visible - Landscape of Mind Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInterface.h"
#include "NiagaraSystem.h"
#include "LandscapeOfMindTypes.generated.h"

/**
 * Phase of the landscape computation process
 */
UENUM(BlueprintType)
enum class ELandscapePhase : uint8
{
    Exploration     UMETA(DisplayName = "Exploration & Gardening"),
    Observation     UMETA(DisplayName = "Observation & Selection"),
    Defense         UMETA(DisplayName = "Defense"),
    Completion      UMETA(DisplayName = "Computation Complete")
};

/**
 * Types of Node Stones that can be placed
 */
UENUM(BlueprintType)
enum class ENodeStoneType : uint8
{
    Stabilizer      UMETA(DisplayName = "Stabilizer"),
    Amplifier       UMETA(DisplayName = "Amplifier"),
    Redirector      UMETA(DisplayName = "Redirector"),
    Harmonizer      UMETA(DisplayName = "Harmonizer"),
    Catalyst        UMETA(DisplayName = "Catalyst")
};

/**
 * Patterns that quantum collapse can take
 */
UENUM(BlueprintType)
enum class ECollapsePattern : uint8
{
    None            UMETA(DisplayName = "None"),
    Valley          UMETA(DisplayName = "Valley"),
    Hill            UMETA(DisplayName = "Hill"),
    Plateau         UMETA(DisplayName = "Plateau"),
    Canyon          UMETA(DisplayName = "Canyon"),
    Lake            UMETA(DisplayName = "Lake"),
    Ridge           UMETA(DisplayName = "Ridge"),
    Crater          UMETA(DisplayName = "Crater")
};

/**
 * Types of terrain deformation
 */
UENUM(BlueprintType)
enum class EDeformationType : uint8
{
    Raise           UMETA(DisplayName = "Raise"),
    Lower           UMETA(DisplayName = "Lower"),
    Smooth          UMETA(DisplayName = "Smooth"),
    Roughen         UMETA(DisplayName = "Roughen"),
    Flatten         UMETA(DisplayName = "Flatten")
};

/**
 * Types of noise creatures
 */
UENUM(BlueprintType)
enum class ENoiseCreatureType : uint8
{
    Disruptor       UMETA(DisplayName = "Disruptor"),
    Corruptor       UMETA(DisplayName = "Corruptor"),
    Amplifier       UMETA(DisplayName = "Amplifier"),
    Swarm           UMETA(DisplayName = "Swarm"),
    Boss            UMETA(DisplayName = "Boss")
};

/**
 * Creature behavior states
 */
UENUM(BlueprintType)
enum class ECreatureBehavior : uint8
{
    Patrol          UMETA(DisplayName = "Patrol"),
    Attack          UMETA(DisplayName = "Attack"),
    Defend          UMETA(DisplayName = "Defend"),
    Retreat         UMETA(DisplayName = "Retreat"),
    Swarm           UMETA(DisplayName = "Swarm"),
    Ambush          UMETA(DisplayName = "Ambush")
};

/**
 * Configuration for the mental landscape
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FMentalLandscapeConfig
{
    GENERATED_BODY()

    FMentalLandscapeConfig()
    {
        LandscapeSize = FVector(8192.0f, 8192.0f, 2048.0f);
        InitialChaosLevel = 80;
        TargetStabilityLevel = 20;
        MaxNodeStones = 50;
        MaxSynapseRivers = 20;
        UncertainAreaCount = 8;
        NoiseCreatureSpawnRate = 0.1f;
        StabilityColorGradient = {
            FLinearColor::Red,      // High chaos
            FLinearColor::Yellow,   // Medium chaos
            FLinearColor::Green,    // Stable
            FLinearColor::Blue      // Very stable
        };
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Configuration")
    FVector LandscapeSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Configuration", meta = (ClampMin = "0", ClampMax = "100"))
    int32 InitialChaosLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Configuration", meta = (ClampMin = "0", ClampMax = "100"))
    int32 TargetStabilityLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Configuration", meta = (ClampMin = "1", ClampMax = "100"))
    int32 MaxNodeStones;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Configuration", meta = (ClampMin = "1", ClampMax = "50"))
    int32 MaxSynapseRivers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Configuration", meta = (ClampMin = "1", ClampMax = "20"))
    int32 UncertainAreaCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Configuration", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float NoiseCreatureSpawnRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Configuration")
    TArray<FLinearColor> StabilityColorGradient;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape Configuration")
    UMaterialInterface* TerrainMaterial;
};

/**
 * Current state of the mental landscape
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FMentalLandscapeState
{
    GENERATED_BODY()

    FMentalLandscapeState()
    {
        OverallStability = 0.0f;
        EnergyFlow = 0.0f;
        ActiveNodeStones = 0;
        ActiveRivers = 0;
        ObservedAreas = 0;
        ActiveCreatures = 0;
        ComputationProgress = 0.0f;
        CurrentPhase = ELandscapePhase::Exploration;
    }

    UPROPERTY(BlueprintReadOnly, Category = "Landscape State")
    float OverallStability;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape State")
    float EnergyFlow;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape State")
    int32 ActiveNodeStones;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape State")
    int32 ActiveRivers;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape State")
    int32 ObservedAreas;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape State")
    int32 ActiveCreatures;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape State")
    float ComputationProgress;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape State")
    ELandscapePhase CurrentPhase;
};

/**
 * Properties of a Node Stone
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FNodeStoneProperties
{
    GENERATED_BODY()

    FNodeStoneProperties()
    {
        Type = ENodeStoneType::Stabilizer;
        InfluenceRadius = 500.0f;
        StabilizationPower = 1.0f;
        EnergyDirection = FVector::ZeroVector;
        EnergyOutput = 1.0f;
        bIsActive = true;
        GlowColor = FLinearColor::Blue;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Stone")
    ENodeStoneType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Stone", meta = (ClampMin = "0"))
    float InfluenceRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Stone", meta = (ClampMin = "0"))
    float StabilizationPower;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Stone")
    FVector EnergyDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Stone", meta = (ClampMin = "0"))
    float EnergyOutput;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Stone")
    bool bIsActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Stone")
    FLinearColor GlowColor;
};

/**
 * Terrain possibility in quantum superposition
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FTerrainPossibility
{
    GENERATED_BODY()

    FTerrainPossibility()
    {
        Probability = 0.0f;
        VisualizationColor = FLinearColor::White;
        StabilityContribution = 0.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quantum Terrain")
    FString PossibilityName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quantum Terrain", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Probability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quantum Terrain")
    TArray<FVector> HeightmapData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quantum Terrain")
    FLinearColor VisualizationColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quantum Terrain")
    float StabilityContribution;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quantum Terrain")
    TArray<FVector> EnergyFlowPattern;
};

/**
 * Uncertain area in quantum superposition
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FUncertainArea
{
    GENERATED_BODY()

    FUncertainArea()
    {
        AreaID = 0;
        Center = FVector::ZeroVector;
        Radius = 1000.0f;
        bIsObserved = false;
        CollapsedPattern = ECollapsePattern::None;
        CoherenceLevel = 1.0f;
        CreationTime = FDateTime::Now();
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uncertain Area")
    int32 AreaID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uncertain Area")
    FVector Center;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uncertain Area", meta = (ClampMin = "0"))
    float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uncertain Area")
    TArray<FTerrainPossibility> Possibilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uncertain Area")
    bool bIsObserved;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uncertain Area")
    ECollapsePattern CollapsedPattern;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uncertain Area", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float CoherenceLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uncertain Area")
    FDateTime CreationTime;
};


