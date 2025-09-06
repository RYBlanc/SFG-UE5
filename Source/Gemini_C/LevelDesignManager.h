// Project Visible - Level Design Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "LevelDesignTypes.h"
#include "LevelDesignManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelLoaded, const FLevelDefinition&, Level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelCompleted, const FLevelDefinition&, Level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelTransition, const FString&, FromLevel, const FString&, ToLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherChanged, EWeatherCondition, NewWeather);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightingChanged, const FLevelLightingConfig&, NewLighting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectInteracted, const FInteractionObject&, Object, const FString&, PlayerID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelGenerated, const FString&, LevelID);

/**
 * Central manager for Project Visible's level design and world generation
 * Manages level loading, transitions, environment, and interactive objects
 */
UCLASS()
class GEMINI_C_API ULevelDesignManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Level Management
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void InitializeLevelSystem();

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void LoadLevel(const FString& LevelID);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void UnloadLevel(const FString& LevelID);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void TransitionToLevel(const FString& FromLevelID, const FString& ToLevelID);

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    bool IsLevelLoaded(const FString& LevelID) const;

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    FString GetCurrentLevelID() const { return CurrentLevelID; }

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    bool GetCurrentLevel(FLevelDefinition& OutLevel) const;

    // Level Definition Management
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void RegisterLevel(const FLevelDefinition& Level);

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    bool GetLevelDefinition(const FString& LevelID, FLevelDefinition& OutLevel) const;

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    TArray<FLevelDefinition> GetAvailableLevels() const;

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    TArray<FLevelDefinition> GetLevelsByType(ELevelType LevelType) const;

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    TArray<FLevelDefinition> GetLevelsByStoryLocation(EStoryLocation Location) const;

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void UpdateLevelProgress(const FString& LevelID, float ProgressPercentage);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void CompleteLevelElement(const FString& LevelID, const FString& ElementID);

    // Procedural Generation
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void GenerateLevel(const FString& LevelID, const FLevelGenerationParams& Params);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void RegenerateLevel(const FString& LevelID);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void ApplyGenerationRules(const FString& LevelID, const TMap<FString, float>& Rules);

    // Environment Control
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void SetWeatherCondition(EWeatherCondition Weather);

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    EWeatherCondition GetCurrentWeather() const { return CurrentWeather; }

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void TransitionWeather(EWeatherCondition NewWeather, float TransitionTime = 5.0f);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void SetTimeOfDay(float Time);

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    float GetTimeOfDay() const;

    // Lighting System
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void ApplyLightingConfig(const FLevelLightingConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    FLevelLightingConfig GetCurrentLightingConfig() const { return CurrentLightingConfig; }

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void SetAmbientLighting(float Brightness, const FLinearColor& Color);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void ToggleDynamicLighting(bool bEnable);

    // Interactive Objects
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void SpawnInteractionObject(const FInteractionObject& Object);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void RemoveInteractionObject(const FString& ObjectID);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void InteractWithObject(const FString& ObjectID, const FString& PlayerID);

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    TArray<FInteractionObject> GetNearbyObjects(const FVector& Location, float Radius) const;

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    TArray<FInteractionObject> GetObjectsByType(EInteractionObjectType Type) const;

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    bool GetInteractionObject(const FString& ObjectID, FInteractionObject& OutObject) const;

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void UpdateObjectState(const FString& ObjectID, bool bIsInteractable, bool bIsCollected = false);

    // Level Transitions
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void AddLevelTransition(const FLevelTransition& Transition);

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    TArray<FLevelTransition> GetAvailableTransitions(const FString& FromLevelID) const;

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    bool CanTransitionToLevel(const FString& FromLevelID, const FString& ToLevelID) const;

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void SetTransitionCondition(const FString& FromLevelID, const FString& ToLevelID, 
                               const TArray<FString>& Conditions);

    // Performance & Optimization
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void OptimizeLevelPerformance(const FString& LevelID);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void SetLevelDetailLevel(float DetailLevel);

    UFUNCTION(BlueprintCallable, Category = "Level Design", BlueprintPure)
    float GetCurrentPerformanceMetric() const;

    // Integration with Story System
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void UpdateLevelsForStoryPhase(EStoryPhase Phase);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void UnlockLevelsForLocation(EStoryLocation Location);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void TriggerLevelEvent(const FString& LevelID, const FString& EventType, const FString& EventData);

    // Data Management
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void SaveLevelProgress();

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void LoadLevelProgress();

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void ResetLevel(const FString& LevelID);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void ResetAllLevels();

    // Analytics Integration
    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void RecordLevelAnalytics(const FString& EventName, const FString& LevelID, const FString& EventData);

    UFUNCTION(BlueprintCallable, Category = "Level Design")
    void AnalyzeLevelUsage();

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Level Events")
    FOnLevelLoaded OnLevelLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Level Events")
    FOnLevelCompleted OnLevelCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Level Events")
    FOnLevelTransition OnLevelTransition;

    UPROPERTY(BlueprintAssignable, Category = "Level Events")
    FOnWeatherChanged OnWeatherChanged;

    UPROPERTY(BlueprintAssignable, Category = "Level Events")
    FOnLightingChanged OnLightingChanged;

    UPROPERTY(BlueprintAssignable, Category = "Level Events")
    FOnObjectInteracted OnObjectInteracted;

    UPROPERTY(BlueprintAssignable, Category = "Level Events")
    FOnLevelGenerated OnLevelGenerated;

protected:
    // Core Level Data
    UPROPERTY(BlueprintReadOnly, Category = "Level Data")
    TArray<FLevelDefinition> RegisteredLevels;

    UPROPERTY(BlueprintReadOnly, Category = "Level Data")
    TArray<FInteractionObject> ActiveObjects;

    UPROPERTY(BlueprintReadOnly, Category = "Level Data")
    TArray<FLevelTransition> LevelTransitions;

    // Current State
    UPROPERTY(BlueprintReadOnly, Category = "Level State")
    FString CurrentLevelID;

    UPROPERTY(BlueprintReadOnly, Category = "Level State")
    EWeatherCondition CurrentWeather;

    UPROPERTY(BlueprintReadOnly, Category = "Level State")
    FLevelLightingConfig CurrentLightingConfig;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Settings")
    bool bAutoSaveProgress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Settings")
    float PerformanceTargetFPS;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Settings")
    bool bEnableProcedural;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Settings")
    bool bOptimizeAutomatically;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Settings")
    float LevelTransitionFadeTime;

    // Runtime State
    UPROPERTY()
    TArray<FString> LoadedLevels;

    UPROPERTY()
    TMap<FString, float> LevelLoadTimes;

    UPROPERTY()
    TMap<FString, FDateTime> LevelAccessTimes;

private:
    // Helper Functions
    void InitializeDefaultLevels();
    void InitializeDefaultTransitions();
    void SetupEnvironmentSystems();

    // Level Management Helpers
    FLevelDefinition* GetLevelByID(const FString& LevelID);
    const FLevelDefinition* GetLevelByID(const FString& LevelID) const;
    void ValidateLevelRequirements(const FString& LevelID);
    void ProcessLevelCompletion(const FString& LevelID);

    // Object Management Helpers
    FInteractionObject* GetObjectByID(const FString& ObjectID);
    const FInteractionObject* GetObjectByID(const FString& ObjectID) const;
    void ProcessObjectInteraction(const FInteractionObject& Object, const FString& PlayerID);
    void UpdateObjectVisibility();

    // Transition Helpers
    FLevelTransition* GetTransition(const FString& FromLevelID, const FString& ToLevelID);
    const FLevelTransition* GetTransition(const FString& FromLevelID, const FString& ToLevelID) const;
    bool ValidateTransitionConditions(const FLevelTransition& Transition) const;
    void ExecuteTransition(const FLevelTransition& Transition);

    // Generation Helpers
    void GenerateTerrain(const FString& LevelID, const FLevelGenerationParams& Params);
    void PlaceObjects(const FString& LevelID, const FLevelGenerationParams& Params);
    void ApplyTheme(const FString& LevelID, EEnvironmentTheme Theme);
    void OptimizeGeneration(const FString& LevelID);

    // Environment Helpers
    void UpdateWeatherEffects();
    void UpdateLightingEffects();
    void UpdateAtmosphericEffects();

    // Performance Helpers
    void MonitorPerformance();
    void ApplyPerformanceOptimizations();
    void AdjustLevelOfDetail();

    // Integration Helpers
    class UStoryManager* GetStoryManager();
    class UVirtueManager* GetVirtueManager();
    class UMemoryManager* GetMemoryManager();
    class USocialExperimentManager* GetSocialExperimentManager();
    class UProjectVisibleUIManager* GetUIManager();
};
