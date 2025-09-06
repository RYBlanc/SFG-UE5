// Project Visible - Noise Creature AI System
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/ActorComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LandscapeOfMindTypes.h"
#include "NoiseCreatureAI.generated.h"

// Forward declarations removed for now

/**
 * Data structure for individual noise creatures
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FNoiseCreatureData
{
    GENERATED_BODY()

    FNoiseCreatureData()
    {
        CreatureID = 0;
        Type = ENoiseCreatureType::Disruptor;
        Location = FVector::ZeroVector;
        Velocity = FVector::ZeroVector;
        TargetLocation = FVector::ZeroVector;
        CurrentBehavior = ECreatureBehavior::Patrol;
        Health = 100.0f;
        DisruptionPower = 1.0f;
        SwarmID = -1;
        bIsActive = true;
        SpawnTime = FDateTime::Now();
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    int32 CreatureID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    ENoiseCreatureType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    FVector Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    FVector Velocity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    FVector TargetLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    ECreatureBehavior CurrentBehavior;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    float DisruptionPower;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    int32 SwarmID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    bool bIsActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature Data")
    FDateTime SpawnTime;
};

/**
 * Creature actions they can perform
 */
UENUM(BlueprintType)
enum class ECreatureAction : uint8
{
    Move            UMETA(DisplayName = "Move"),
    DisruptTerrain  UMETA(DisplayName = "Disrupt Terrain"),
    AttackNode      UMETA(DisplayName = "Attack Node"),
    BlockRiver      UMETA(DisplayName = "Block River"),
    CreateChaos     UMETA(DisplayName = "Create Chaos"),
    Multiply        UMETA(DisplayName = "Multiply")
};

/**
 * Swarm behavior types
 */
UENUM(BlueprintType)
enum class ESwarmBehavior : uint8
{
    Coordinated     UMETA(DisplayName = "Coordinated"),
    Scattered       UMETA(DisplayName = "Scattered"),
    Formation       UMETA(DisplayName = "Formation"),
    Chaotic         UMETA(DisplayName = "Chaotic")
};

/**
 * Swarm data structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FCreatureSwarm
{
    GENERATED_BODY()

    FCreatureSwarm()
    {
        SwarmID = 0;
        SwarmCenter = FVector::ZeroVector;
        SwarmTarget = FVector::ZeroVector;
        SwarmRadius = 500.0f;
        SwarmBehavior = ESwarmBehavior::Coordinated;
        CoordinationStrength = 1.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm")
    int32 SwarmID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm")
    TArray<int32> CreatureIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm")
    FVector SwarmCenter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm")
    FVector SwarmTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm")
    float SwarmRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm")
    ESwarmBehavior SwarmBehavior;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm")
    float CoordinationStrength;
};

/**
 * Player action data for AI learning
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FPlayerActionData
{
    GENERATED_BODY()

    FPlayerActionData()
    {
        ActionDuration = 0.0f;
        Timestamp = FDateTime::Now();
        PlayerStressLevel = 0.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Action")
    FString ActionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Action")
    FString ActionContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Action")
    float ActionDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Action")
    TMap<FString, FString> ActionParameters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Action")
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Action")
    float PlayerStressLevel;
};

/**
 * Adaptive difficulty system
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FAdaptiveDifficulty
{
    GENERATED_BODY()

    FAdaptiveDifficulty()
    {
        PlayerSkillLevel = 0.5f;
        DifficultyScaling = 1.0f;
        AdaptationRate = 0.1f;
        MinDifficulty = 0.2f;
        MaxDifficulty = 2.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adaptive Difficulty")
    float PlayerSkillLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adaptive Difficulty")
    float DifficultyScaling;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adaptive Difficulty")
    float AdaptationRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adaptive Difficulty")
    float MinDifficulty;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adaptive Difficulty")
    float MaxDifficulty;
};

/**
 * AI configuration data
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FNoiseCreatureAIConfig
{
    GENERATED_BODY()

    FNoiseCreatureAIConfig()
    {
        MaxCreatures = 50;
        SpawnRate = 0.1f;
        AggressionLevel = 0.5f;
        IntelligenceLevel = 0.7f;
        SwarmCoordination = 0.8f;
        bEnableAdaptiveDifficulty = true;
        bEnableLearning = true;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
    int32 MaxCreatures;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
    float SpawnRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
    float AggressionLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
    float IntelligenceLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
    float SwarmCoordination;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
    bool bEnableAdaptiveDifficulty;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
    bool bEnableLearning;
};

/**
 * Noise Creature AI System - Manages decoherence defense gameplay
 * Implements adaptive AI that learns from player behavior
 */
UCLASS(BlueprintType, Blueprintable)
class GEMINI_C_API UNoiseCreatureAI : public UObject
{
    GENERATED_BODY()

public:
    UNoiseCreatureAI();

    // AI Management
    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void InitializeAI(const FNoiseCreatureAIConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void UpdateAllCreatures(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void SpawnCreature(const FVector& Location, ENoiseCreatureType Type);

    // Behavior Control
    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void SetCreatureBehavior(int32 CreatureID, ECreatureBehavior Behavior);

    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void UpdateCreatureTarget(int32 CreatureID, const FVector& TargetLocation);

    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void ExecuteCreatureAction(int32 CreatureID, ECreatureAction Action);

    // Swarm Behavior
    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void FormSwarm(const TArray<int32>& CreatureIDs);

    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void UpdateSwarmBehavior(int32 SwarmID, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void DissolveSwarm(int32 SwarmID);

    // Adaptive Learning
    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void LearnFromPlayerBehavior(const FPlayerActionData& ActionData);

    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void AdaptToPlayerStrategy();

    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    void UpdateDifficultyScaling(float PlayerSkillLevel);

    // State Access
    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI", BlueprintPure)
    const TArray<FNoiseCreatureData>& GetAllCreatures() const { return Creatures; }

    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI")
    bool GetCreatureData(int32 CreatureID, FNoiseCreatureData& OutCreatureData);

    UFUNCTION(BlueprintCallable, Category = "Noise Creature AI", BlueprintPure)
    const FAdaptiveDifficulty& GetDifficultySystem() const { return DifficultySystem; }

protected:
    // Creature Management
    UPROPERTY(BlueprintReadOnly, Category = "Noise Creature AI")
    TArray<FNoiseCreatureData> Creatures;

    UPROPERTY(BlueprintReadOnly, Category = "Noise Creature AI")
    TArray<FCreatureSwarm> Swarms;

    // AI Components
    UPROPERTY(BlueprintReadOnly, Category = "Noise Creature AI")
    UBehaviorTreeComponent* AIBehaviorTree;

    UPROPERTY(BlueprintReadOnly, Category = "Noise Creature AI")
    UBlackboardComponent* AIBlackboard;

    // Configuration and Systems
    UPROPERTY(BlueprintReadOnly, Category = "Noise Creature AI")
    FNoiseCreatureAIConfig AIConfig;

    UPROPERTY(BlueprintReadOnly, Category = "Noise Creature AI")
    UObject* BehaviorAnalyzer;

    UPROPERTY(BlueprintReadOnly, Category = "Noise Creature AI")
    FAdaptiveDifficulty DifficultySystem;

    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreatureSpawned, int32, CreatureID, const FVector&, Location);
    UPROPERTY(BlueprintAssignable, Category = "Noise Creature AI")
    FOnCreatureSpawned OnCreatureSpawned;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreatureDestroyed, int32, CreatureID);
    UPROPERTY(BlueprintAssignable, Category = "Noise Creature AI")
    FOnCreatureDestroyed OnCreatureDestroyed;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwarmFormed, int32, SwarmID, const TArray<int32>&, CreatureIDs);
    UPROPERTY(BlueprintAssignable, Category = "Noise Creature AI")
    FOnSwarmFormed OnSwarmFormed;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDifficultyChanged, float, NewDifficulty);
    UPROPERTY(BlueprintAssignable, Category = "Noise Creature AI")
    FOnDifficultyChanged OnDifficultyChanged;

private:
    // Internal AI Logic
    void UpdateCreatureAI(int32 CreatureID, float DeltaTime);
    void CalculateCreatureMovement(int32 CreatureID);
    void ProcessCreatureInteractions();
    void UpdateSwarmCoordination(int32 SwarmID);
    void OptimizeAIPerformance();

    // Behavior Implementation
    void ExecutePatrolBehavior(FNoiseCreatureData& Creature, float DeltaTime);
    void ExecuteAttackBehavior(FNoiseCreatureData& Creature, float DeltaTime);
    void ExecuteDefendBehavior(FNoiseCreatureData& Creature, float DeltaTime);
    void ExecuteRetreatBehavior(FNoiseCreatureData& Creature, float DeltaTime);
    void ExecuteSwarmBehavior(FNoiseCreatureData& Creature, float DeltaTime);
    void ExecuteAmbushBehavior(FNoiseCreatureData& Creature, float DeltaTime);

    // Type-specific AI
    void UpdateDisruptorAI(FNoiseCreatureData& Creature, float DeltaTime);
    void UpdateCorruptorAI(FNoiseCreatureData& Creature, float DeltaTime);
    void UpdateAmplifierAI(FNoiseCreatureData& Creature, float DeltaTime);
    void UpdateSwarmAI(FNoiseCreatureData& Creature, float DeltaTime);
    void UpdateBossAI(FNoiseCreatureData& Creature, float DeltaTime);

    // Target Selection
    FVector FindBestTarget(const FNoiseCreatureData& Creature) const;
    FVector FindNearestNodeStone(const FVector& Location) const;
    FVector FindNearestSynapseRiver(const FVector& Location) const;
    FVector FindPlayerLocation() const;

    // Learning and Adaptation
    void AnalyzePlayerPattern(const FPlayerActionData& ActionData);
    void UpdateAIStrategy();
    void AdjustCreatureParameters();

    // Utility Functions
    float CalculateCreatureEffectiveness(int32 CreatureID) const;
    bool IsCreatureInCombat(int32 CreatureID) const;
    float GetDistanceToPlayer(const FVector& Location) const;

    // ID Management
    int32 NextCreatureID;
    int32 NextSwarmID;

    // Learning Data
    TArray<FPlayerActionData> PlayerActionHistory;
    TMap<FString, float> PlayerStrategyWeights;

    // Performance Optimization
    float LastOptimizationTime;
    static constexpr float OPTIMIZATION_INTERVAL = 5.0f;
    static constexpr float AI_UPDATE_INTERVAL = 0.1f;
    static constexpr float SWARM_UPDATE_INTERVAL = 0.2f;
};
