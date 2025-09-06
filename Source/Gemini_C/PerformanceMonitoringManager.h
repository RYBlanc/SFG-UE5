// Project Visible - Performance Monitoring Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "PerformanceMonitoringTypes.h"
#include "PerformanceMonitoringManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformanceAlert, const FPerformanceAlert&, Alert);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPerformanceMetricsUpdated, const FPerformanceMetrics&, NewMetrics, const FPerformanceMetrics&, PreviousMetrics);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQualityLevelChanged, EPerformanceLevel, OldLevel, EPerformanceLevel, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOptimizationApplied, const FString&, OptimizationType, float, EffectivenessScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSystemStabilityChanged, float, NewStabilityScore);

/**
 * Central manager for Project Visible's performance monitoring and optimization system
 * Monitors system performance, triggers optimizations, and manages quality settings
 */
UCLASS()
class GEMINI_C_API UPerformanceMonitoringManager : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // FTickableGameObject Interface
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual bool IsTickable() const override;

    // Core Monitoring Functions
    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void StartPerformanceMonitoring();

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void StopPerformanceMonitoring();

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void PauseMonitoring();

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void ResumeMonitoring();

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring", BlueprintPure)
    bool IsMonitoringActive() const { return bIsMonitoring; }

    // Metrics Collection
    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    FPerformanceMetrics CollectCurrentMetrics();

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring", BlueprintPure)
    FPerformanceMetrics GetCurrentMetrics() const { return CurrentMetrics; }

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring", BlueprintPure)
    TArray<FPerformanceSnapshot> GetPerformanceHistory(int32 MaxEntries = 100) const;

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void ClearPerformanceHistory();

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void TakePerformanceSnapshot(const FString& EventType = TEXT("Manual"));

    // Alert Management
    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void CheckPerformanceThresholds();

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void TriggerCustomAlert(EPerformanceAlertType AlertType, float Severity, const FString& Description);

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring", BlueprintPure)
    TArray<FPerformanceAlert> GetActiveAlerts() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void ResolveAlert(const FPerformanceAlert& Alert);

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void DismissAlert(int32 AlertIndex);

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void ClearAllAlerts();

    // Threshold Management
    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void SetPerformanceThresholds(const FPerformanceThresholds& NewThresholds);

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring", BlueprintPure)
    FPerformanceThresholds GetPerformanceThresholds() const { return Thresholds; }

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    void ResetThresholdsToDefault();

    // Optimization Control
    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void ApplyOptimizations();

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void SetOptimizationSettings(const FOptimizationSettings& NewSettings);

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization", BlueprintPure)
    FOptimizationSettings GetOptimizationSettings() const { return OptimizationSettings; }

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void SetAutoOptimizationEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization", BlueprintPure)
    bool IsAutoOptimizationEnabled() const { return OptimizationSettings.bAutoOptimizationEnabled; }

    // Quality Level Management
    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void SetPerformanceLevel(EPerformanceLevel NewLevel);

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization", BlueprintPure)
    EPerformanceLevel GetCurrentPerformanceLevel() const { return CurrentPerformanceLevel; }

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void AutoAdjustQualityLevel();

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void RecommendOptimalSettings();

    // Specific Optimizations
    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void OptimizeMemoryUsage();

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void OptimizeRenderingPerformance();

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void OptimizeAudioPerformance();

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void OptimizePhysicsPerformance();

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void OptimizeLODSettings();

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void TriggerGarbageCollection();

    UFUNCTION(BlueprintCallable, Category = "Performance Optimization")
    void FlushRenderingCommands();

    // System Analysis
    UFUNCTION(BlueprintCallable, Category = "Performance Analysis")
    float CalculateOverallPerformanceScore() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Analysis")
    float CalculateSystemStabilityScore() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Analysis")
    void AnalyzePerformanceTrends();

    UFUNCTION(BlueprintCallable, Category = "Performance Analysis")
    FString GeneratePerformanceReport() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Analysis")
    TMap<FString, float> GetPerformanceBottlenecks() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Analysis")
    TArray<FString> GetOptimizationRecommendations() const;

    // Resource Monitoring
    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    float GetMemoryUsage() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    float GetCPUUsage() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    float GetGPUUsage() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    float GetNetworkLatency() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    int32 GetActiveDrawCalls() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Monitoring")
    int32 GetActiveTriangles() const;

    // Game Integration
    UFUNCTION(BlueprintCallable, Category = "Performance Integration")
    void OnLevelTransition(const FString& NewLevelName);

    UFUNCTION(BlueprintCallable, Category = "Performance Integration")
    void OnPlayerCountChanged(int32 NewPlayerCount);

    UFUNCTION(BlueprintCallable, Category = "Performance Integration")
    void OnGamePhaseChanged(const FString& NewPhase);

    UFUNCTION(BlueprintCallable, Category = "Performance Integration")
    void OnSignificantGameEvent(const FString& EventType, const FString& EventData);

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "Performance Configuration")
    void SetMonitoringFrequency(float FrequencyHz);

    UFUNCTION(BlueprintCallable, Category = "Performance Configuration", BlueprintPure)
    float GetMonitoringFrequency() const { return MonitoringFrequency; }

    UFUNCTION(BlueprintCallable, Category = "Performance Configuration")
    void SetHistoryRetentionTime(float RetentionTimeSeconds);

    UFUNCTION(BlueprintCallable, Category = "Performance Configuration")
    void EnableDetailedProfiling(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Performance Configuration")
    void SetBenchmarkMode(bool bEnabled);

    // Data Export/Import
    UFUNCTION(BlueprintCallable, Category = "Performance Data")
    FString ExportPerformanceData() const;

    UFUNCTION(BlueprintCallable, Category = "Performance Data")
    bool ImportPerformanceData(const FString& JsonData);

    UFUNCTION(BlueprintCallable, Category = "Performance Data")
    void SavePerformanceProfile(const FString& ProfileName);

    UFUNCTION(BlueprintCallable, Category = "Performance Data")
    bool LoadPerformanceProfile(const FString& ProfileName);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnPerformanceAlert OnPerformanceAlert;

    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnPerformanceMetricsUpdated OnPerformanceMetricsUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnQualityLevelChanged OnQualityLevelChanged;

    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnOptimizationApplied OnOptimizationApplied;

    UPROPERTY(BlueprintAssignable, Category = "Performance Events")
    FOnSystemStabilityChanged OnSystemStabilityChanged;

protected:
    // Core Data
    UPROPERTY(BlueprintReadOnly, Category = "Performance Data")
    FPerformanceMetrics CurrentMetrics;

    UPROPERTY(BlueprintReadOnly, Category = "Performance Data")
    FPerformanceMetrics PreviousMetrics;

    UPROPERTY(BlueprintReadOnly, Category = "Performance Data")
    TArray<FPerformanceSnapshot> PerformanceHistory;

    UPROPERTY(BlueprintReadOnly, Category = "Performance Data")
    TArray<FPerformanceAlert> ActiveAlerts;

    UPROPERTY(BlueprintReadOnly, Category = "Performance Data")
    FPerformanceThresholds Thresholds;

    UPROPERTY(BlueprintReadOnly, Category = "Performance Data")
    FOptimizationSettings OptimizationSettings;

    // State
    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    bool bIsMonitoring;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    bool bIsPaused;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    bool bDetailedProfilingEnabled;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    bool bBenchmarkMode;

    UPROPERTY(BlueprintReadOnly, Category = "Performance State")
    EPerformanceLevel CurrentPerformanceLevel;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    float MonitoringFrequency; // Hz

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    float HistoryRetentionTime; // Seconds

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    int32 MaxHistoryEntries;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    int32 MaxActiveAlerts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Settings")
    bool bAutoCleanupAlerts;

    // Runtime State
    UPROPERTY()
    float TimeSinceLastUpdate;

    UPROPERTY()
    float TimeSinceLastSnapshot;

    UPROPERTY()
    FDateTime LastOptimizationTime;

    UPROPERTY()
    TMap<FString, FDateTime> LastAlertTimes;

    UPROPERTY()
    TMap<FString, float> OptimizationEffectiveness;

private:
    // Internal Functions
    void UpdatePerformanceMetrics();
    void ProcessAlerts();
    void ManagePerformanceHistory();
    void ApplyAutoOptimizations();
    void CalculatePerformanceScores();
    void CleanupExpiredAlerts();
    void UpdateSystemStability();

    // Metrics Collection Helpers
    float CollectFrameRateMetrics();
    void CollectMemoryMetrics(FPerformanceMetrics& Metrics);
    void CollectRenderingMetrics(FPerformanceMetrics& Metrics);
    void CollectCPUMetrics(FPerformanceMetrics& Metrics);
    void CollectGPUMetrics(FPerformanceMetrics& Metrics);
    void CollectNetworkMetrics(FPerformanceMetrics& Metrics);
    void CollectAudioMetrics(FPerformanceMetrics& Metrics);
    void CollectSystemMetrics(FPerformanceMetrics& Metrics);

    // Alert System Helpers
    void CheckFrameRateThreshold();
    void CheckMemoryThreshold();
    void CheckCPUThreshold();
    void CheckGPUThreshold();
    void CheckStabilityThreshold();
    void CreateAlert(EPerformanceAlertType AlertType, float Severity, const FString& Description, const FString& RecommendedAction);
    bool ShouldCreateAlert(EPerformanceAlertType AlertType) const;

    // Optimization Helpers
    void ApplyMemoryOptimizations();
    void ApplyRenderingOptimizations();
    void ApplyCPUOptimizations();
    void ApplyGPUOptimizations();
    void ApplyQualityOptimizations(EPerformanceLevel TargetLevel);
    float EvaluateOptimizationEffectiveness(const FString& OptimizationType);

    // Analysis Helpers
    void AnalyzeFrameRateTrends();
    void AnalyzeMemoryTrends();
    void AnalyzeStabilityTrends();
    TArray<FString> GenerateRecommendations() const;

    // Integration Helpers
    class UStoryManager* GetStoryManager();
    class ULevelDesignManager* GetLevelDesignManager();
    class UAudioSystemManager* GetAudioSystemManager();
    class UGameProgressionManager* GetProgressionManager();
    class USocialExperimentManager* GetExperimentManager();
};
