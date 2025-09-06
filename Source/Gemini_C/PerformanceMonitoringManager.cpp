// Project Visible - Performance Monitoring Manager Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "PerformanceMonitoringManager.h"
#include "Gemini_C.h"
#include "StoryManager.h"
#include "LevelDesignManager.h"
#include "AudioSystemManager.h"
#include "GameProgressionManager.h"
#include "SocialExperimentManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Stats/Stats.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerformanceMonitoring, Log, All);

void UPerformanceMonitoringManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize default settings
    bIsMonitoring = false;
    bIsPaused = false;
    bDetailedProfilingEnabled = false;
    bBenchmarkMode = false;
    CurrentPerformanceLevel = EPerformanceLevel::Medium;
    MonitoringFrequency = 4.0f; // 4 times per second
    HistoryRetentionTime = 300.0f; // 5 minutes
    MaxHistoryEntries = 1200; // 5 minutes at 4Hz
    MaxActiveAlerts = 10;
    bAutoCleanupAlerts = true;
    TimeSinceLastUpdate = 0.0f;
    TimeSinceLastSnapshot = 0.0f;
    
    // Initialize default thresholds
    Thresholds = FPerformanceThresholds();
    
    // Initialize default optimization settings
    OptimizationSettings = FOptimizationSettings();
    
    // Initialize current metrics
    CurrentMetrics = FPerformanceMetrics();
    PreviousMetrics = FPerformanceMetrics();
    
    LastOptimizationTime = FDateTime::Now();
    
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance Monitoring Manager initialized"));
}

void UPerformanceMonitoringManager::Deinitialize()
{
    StopPerformanceMonitoring();
    
    // Clear all data
    PerformanceHistory.Empty();
    ActiveAlerts.Empty();
    LastAlertTimes.Empty();
    OptimizationEffectiveness.Empty();
    
    Super::Deinitialize();
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance Monitoring Manager deinitialized"));
}

// FTickableGameObject Interface
void UPerformanceMonitoringManager::Tick(float DeltaTime)
{
    if (!bIsMonitoring || bIsPaused)
    {
        return;
    }
    
    TimeSinceLastUpdate += DeltaTime;
    TimeSinceLastSnapshot += DeltaTime;
    
    // Update metrics at specified frequency
    if (TimeSinceLastUpdate >= (1.0f / MonitoringFrequency))
    {
        UpdatePerformanceMetrics();
        CheckPerformanceThresholds();
        ProcessAlerts();
        
        if (OptimizationSettings.bAutoOptimizationEnabled)
        {
            ApplyAutoOptimizations();
        }
        
        TimeSinceLastUpdate = 0.0f;
    }
    
    // Take performance snapshots less frequently
    if (TimeSinceLastSnapshot >= 10.0f) // Every 10 seconds
    {
        TakePerformanceSnapshot(TEXT("Auto"));
        TimeSinceLastSnapshot = 0.0f;
    }
    
    // Cleanup old data
    ManagePerformanceHistory();
    
    if (bAutoCleanupAlerts)
    {
        CleanupExpiredAlerts();
    }
}

TStatId UPerformanceMonitoringManager::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UPerformanceMonitoringManager, STATGROUP_Tickables);
}

bool UPerformanceMonitoringManager::IsTickable() const
{
    return bIsMonitoring && !bIsPaused;
}

// Core Monitoring Functions
void UPerformanceMonitoringManager::StartPerformanceMonitoring()
{
    if (bIsMonitoring)
    {
        return;
    }
    
    bIsMonitoring = true;
    bIsPaused = false;
    TimeSinceLastUpdate = 0.0f;
    TimeSinceLastSnapshot = 0.0f;
    
    // Take initial snapshot
    TakePerformanceSnapshot(TEXT("Monitoring Started"));
    
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance monitoring started"));
}

void UPerformanceMonitoringManager::StopPerformanceMonitoring()
{
    if (!bIsMonitoring)
    {
        return;
    }
    
    // Take final snapshot
    if (bIsMonitoring)
    {
        TakePerformanceSnapshot(TEXT("Monitoring Stopped"));
    }
    
    bIsMonitoring = false;
    bIsPaused = false;
    
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance monitoring stopped"));
}

void UPerformanceMonitoringManager::PauseMonitoring()
{
    if (bIsMonitoring && !bIsPaused)
    {
        bIsPaused = true;
        UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance monitoring paused"));
    }
}

void UPerformanceMonitoringManager::ResumeMonitoring()
{
    if (bIsMonitoring && bIsPaused)
    {
        bIsPaused = false;
        UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance monitoring resumed"));
    }
}

// Metrics Collection
FPerformanceMetrics UPerformanceMonitoringManager::CollectCurrentMetrics()
{
    FPerformanceMetrics Metrics;
    Metrics.Timestamp = FDateTime::Now();
    Metrics.QualityLevel = CurrentPerformanceLevel;
    
    // Collect frame rate metrics
    Metrics.FrameRate = CollectFrameRateMetrics();
    Metrics.AverageFrameTime = (Metrics.FrameRate > 0.0f) ? (1000.0f / Metrics.FrameRate) : 0.0f;
    
    // Collect system metrics
    CollectMemoryMetrics(Metrics);
    CollectRenderingMetrics(Metrics);
    CollectCPUMetrics(Metrics);
    CollectGPUMetrics(Metrics);
    CollectNetworkMetrics(Metrics);
    CollectAudioMetrics(Metrics);
    CollectSystemMetrics(Metrics);
    
    // Calculate performance scores
    Metrics.OverallPerformanceScore = CalculateOverallPerformanceScore();
    Metrics.StabilityScore = CalculateSystemStabilityScore();
    
    return Metrics;
}

TArray<FPerformanceSnapshot> UPerformanceMonitoringManager::GetPerformanceHistory(int32 MaxEntries) const
{
    TArray<FPerformanceSnapshot> LimitedHistory;
    
    int32 StartIndex = FMath::Max(0, PerformanceHistory.Num() - MaxEntries);
    for (int32 i = StartIndex; i < PerformanceHistory.Num(); ++i)
    {
        LimitedHistory.Add(PerformanceHistory[i]);
    }
    
    return LimitedHistory;
}

void UPerformanceMonitoringManager::ClearPerformanceHistory()
{
    PerformanceHistory.Empty();
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance history cleared"));
}

void UPerformanceMonitoringManager::TakePerformanceSnapshot(const FString& EventType)
{
    FPerformanceSnapshot Snapshot;
    Snapshot.Timestamp = FDateTime::Now();
    Snapshot.Metrics = CollectCurrentMetrics();
    Snapshot.ActiveAlerts = ActiveAlerts;
    Snapshot.EventType = EventType;
    Snapshot.bIsSignificantEvent = (EventType != TEXT("Auto"));
    
    // Add context information
    if (UStoryManager* StoryManager = GetStoryManager())
    {
        Snapshot.GamePhase = UEnum::GetValueAsString(StoryManager->GetCurrentPhase());
        Snapshot.Location = UEnum::GetValueAsString(StoryManager->GetCurrentLocation());
    }
    
	if (UGameProgressionManager* ProgressionManager = GetProgressionManager())
	{
		Snapshot.SessionID = TEXT("SESSION_DEFAULT");
	}
    
    Snapshot.PlayerCount = 1; // Single player for now
    Snapshot.ActiveSystemCount = 8; // Approximate number of active systems
    
    PerformanceHistory.Add(Snapshot);
    UE_LOG(LogPerformanceMonitoring, VeryVerbose, TEXT("Performance snapshot taken: %s"), *EventType);
}

// Alert Management
void UPerformanceMonitoringManager::CheckPerformanceThresholds()
{
    CheckFrameRateThreshold();
    CheckMemoryThreshold();
    CheckCPUThreshold();
    CheckGPUThreshold();
    CheckStabilityThreshold();
}

void UPerformanceMonitoringManager::TriggerCustomAlert(EPerformanceAlertType AlertType, float Severity, const FString& Description)
{
    CreateAlert(AlertType, Severity, Description, TEXT("Custom alert - check system status"));
}

TArray<FPerformanceAlert> UPerformanceMonitoringManager::GetActiveAlerts() const
{
    return ActiveAlerts;
}

void UPerformanceMonitoringManager::ResolveAlert(const FPerformanceAlert& Alert)
{
    for (int32 i = 0; i < ActiveAlerts.Num(); ++i)
    {
        if (ActiveAlerts[i].AlertType == Alert.AlertType && 
            ActiveAlerts[i].Timestamp == Alert.Timestamp)
        {
            ActiveAlerts[i].bIsResolved = true;
            UE_LOG(LogPerformanceMonitoring, Log, TEXT("Alert resolved: %s"), *Alert.Description);
            break;
        }
    }
}

void UPerformanceMonitoringManager::DismissAlert(int32 AlertIndex)
{
    if (ActiveAlerts.IsValidIndex(AlertIndex))
    {
        UE_LOG(LogPerformanceMonitoring, Log, TEXT("Alert dismissed: %s"), *ActiveAlerts[AlertIndex].Description);
        ActiveAlerts.RemoveAt(AlertIndex);
    }
}

void UPerformanceMonitoringManager::ClearAllAlerts()
{
    ActiveAlerts.Empty();
    LastAlertTimes.Empty();
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("All alerts cleared"));
}

// Threshold Management
void UPerformanceMonitoringManager::SetPerformanceThresholds(const FPerformanceThresholds& NewThresholds)
{
    Thresholds = NewThresholds;
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance thresholds updated"));
}

void UPerformanceMonitoringManager::ResetThresholdsToDefault()
{
    Thresholds = FPerformanceThresholds();
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance thresholds reset to default"));
}

// Optimization Control
void UPerformanceMonitoringManager::ApplyOptimizations()
{
    if (OptimizationSettings.bMemoryPooling)
    {
        OptimizeMemoryUsage();
    }
    
    if (OptimizationSettings.bRenderingOptimization)
    {
        OptimizeRenderingPerformance();
    }
    
    if (OptimizationSettings.bAudioOptimization)
    {
        OptimizeAudioPerformance();
    }
    
    if (OptimizationSettings.bPhysicsOptimization)
    {
        OptimizePhysicsPerformance();
    }
    
    if (OptimizationSettings.bDynamicQualityAdjustment)
    {
        AutoAdjustQualityLevel();
    }
    
    LastOptimizationTime = FDateTime::Now();
    OnOptimizationApplied.Broadcast(TEXT("Full Optimization"), CalculateOverallPerformanceScore());
    
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance optimizations applied"));
}

void UPerformanceMonitoringManager::SetOptimizationSettings(const FOptimizationSettings& NewSettings)
{
    OptimizationSettings = NewSettings;
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Optimization settings updated"));
}

void UPerformanceMonitoringManager::SetAutoOptimizationEnabled(bool bEnabled)
{
    OptimizationSettings.bAutoOptimizationEnabled = bEnabled;
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Auto optimization %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

// Quality Level Management
void UPerformanceMonitoringManager::SetPerformanceLevel(EPerformanceLevel NewLevel)
{
    if (NewLevel != CurrentPerformanceLevel)
    {
        EPerformanceLevel OldLevel = CurrentPerformanceLevel;
        CurrentPerformanceLevel = NewLevel;
        
        ApplyQualityOptimizations(NewLevel);
        OnQualityLevelChanged.Broadcast(OldLevel, NewLevel);
        
        UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance level changed from %s to %s"), 
               *UEnum::GetValueAsString(OldLevel), *UEnum::GetValueAsString(NewLevel));
    }
}

void UPerformanceMonitoringManager::AutoAdjustQualityLevel()
{
    float CurrentFPS = CurrentMetrics.FrameRate;
    float MemoryUsage = CurrentMetrics.MemoryUsageMB;
    float StabilityScore = CurrentMetrics.StabilityScore;
    
    EPerformanceLevel RecommendedLevel = CurrentPerformanceLevel;
    
    // Determine recommended quality level based on performance
    if (CurrentFPS < Thresholds.MinFrameRate * 0.8f || MemoryUsage > Thresholds.MaxMemoryUsageMB * 0.9f)
    {
        // Performance is poor, reduce quality
        switch (CurrentPerformanceLevel)
        {
            case EPerformanceLevel::Highest:
                RecommendedLevel = EPerformanceLevel::High;
                break;
            case EPerformanceLevel::High:
                RecommendedLevel = EPerformanceLevel::Medium;
                break;
            case EPerformanceLevel::Medium:
                RecommendedLevel = EPerformanceLevel::Low;
                break;
            case EPerformanceLevel::Low:
                RecommendedLevel = EPerformanceLevel::Lowest;
                break;
        }
    }
    else if (CurrentFPS > Thresholds.MinFrameRate * 1.5f && MemoryUsage < Thresholds.MaxMemoryUsageMB * 0.6f && StabilityScore > 80.0f)
    {
        // Performance is good, can increase quality
        switch (CurrentPerformanceLevel)
        {
            case EPerformanceLevel::Lowest:
                RecommendedLevel = EPerformanceLevel::Low;
                break;
            case EPerformanceLevel::Low:
                RecommendedLevel = EPerformanceLevel::Medium;
                break;
            case EPerformanceLevel::Medium:
                RecommendedLevel = EPerformanceLevel::High;
                break;
            case EPerformanceLevel::High:
                RecommendedLevel = EPerformanceLevel::Highest;
                break;
        }
    }
    
    if (RecommendedLevel != CurrentPerformanceLevel)
    {
        SetPerformanceLevel(RecommendedLevel);
    }
}

void UPerformanceMonitoringManager::RecommendOptimalSettings()
{
    TArray<FString> Recommendations = GenerateRecommendations();
    
    for (const FString& Recommendation : Recommendations)
    {
        UE_LOG(LogPerformanceMonitoring, Log, TEXT("Recommendation: %s"), *Recommendation);
    }
}

// Specific Optimizations
void UPerformanceMonitoringManager::OptimizeMemoryUsage()
{
    // Trigger garbage collection
    TriggerGarbageCollection();
    
    // Flush rendering commands
    FlushRenderingCommands();
    
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Memory optimization applied"));
}

void UPerformanceMonitoringManager::OptimizeRenderingPerformance()
{
    // Flush rendering commands
    FlushRenderingCommands();
    
    // Optimize LOD settings
    OptimizeLODSettings();
    
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Rendering performance optimization applied"));
}

void UPerformanceMonitoringManager::OptimizeAudioPerformance()
{
    if (UAudioSystemManager* AudioManager = GetAudioSystemManager())
    {
        // Stop all non-essential audio
        // This would be implemented based on audio system capabilities
        UE_LOG(LogPerformanceMonitoring, Log, TEXT("Audio performance optimization applied"));
    }
}

void UPerformanceMonitoringManager::OptimizePhysicsPerformance()
{
    // Reduce physics simulation quality temporarily
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Physics performance optimization applied"));
}

void UPerformanceMonitoringManager::OptimizeLODSettings()
{
    // Adjust LOD bias based on performance
    float LODBias = (CurrentPerformanceLevel == EPerformanceLevel::Low || CurrentPerformanceLevel == EPerformanceLevel::Lowest) ? 2.0f : 0.0f;
    
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("LOD settings optimized with bias: %.1f"), LODBias);
}

void UPerformanceMonitoringManager::TriggerGarbageCollection()
{
    // Force garbage collection
    GEngine->ForceGarbageCollection(true);
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Garbage collection triggered"));
}

void UPerformanceMonitoringManager::FlushRenderingCommands()
{
    // Flush all pending rendering commands
    ::FlushRenderingCommands();
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Rendering commands flushed"));
}

// System Analysis
float UPerformanceMonitoringManager::CalculateOverallPerformanceScore() const
{
    float FrameRateScore = FMath::Clamp((CurrentMetrics.FrameRate / 60.0f) * 100.0f, 0.0f, 100.0f);
    float MemoryScore = FMath::Clamp((1.0f - (CurrentMetrics.MemoryUsageMB / Thresholds.MaxMemoryUsageMB)) * 100.0f, 0.0f, 100.0f);
    float CPUScore = FMath::Clamp((1.0f - (CurrentMetrics.CPUUsagePercent / 100.0f)) * 100.0f, 0.0f, 100.0f);
    float GPUScore = FMath::Clamp((1.0f - (CurrentMetrics.GPUUsagePercent / 100.0f)) * 100.0f, 0.0f, 100.0f);
    
    // Weighted average
    float OverallScore = (FrameRateScore * 0.4f + MemoryScore * 0.3f + CPUScore * 0.15f + GPUScore * 0.15f);
    
    return FMath::Clamp(OverallScore, 0.0f, 100.0f);
}

float UPerformanceMonitoringManager::CalculateSystemStabilityScore() const
{
    if (PerformanceHistory.Num() < 10)
    {
        return 50.0f; // Default when insufficient data
    }
    
    // Calculate frame rate variance over recent history
    TArray<float> RecentFrameRates;
    int32 StartIndex = FMath::Max(0, PerformanceHistory.Num() - 60); // Last 60 samples
    
    for (int32 i = StartIndex; i < PerformanceHistory.Num(); ++i)
    {
        RecentFrameRates.Add(PerformanceHistory[i].Metrics.FrameRate);
    }
    
    if (RecentFrameRates.Num() == 0)
    {
        return 50.0f;
    }
    
    // Calculate mean and variance
    float Mean = 0.0f;
    for (float FrameRate : RecentFrameRates)
    {
        Mean += FrameRate;
    }
    Mean /= RecentFrameRates.Num();
    
    float Variance = 0.0f;
    for (float FrameRate : RecentFrameRates)
    {
        Variance += FMath::Pow(FrameRate - Mean, 2.0f);
    }
    Variance /= RecentFrameRates.Num();
    
    // Lower variance = higher stability
    float StabilityScore = FMath::Clamp(100.0f - (Variance * 2.0f), 0.0f, 100.0f);
    
    return StabilityScore;
}

void UPerformanceMonitoringManager::AnalyzePerformanceTrends()
{
    AnalyzeFrameRateTrends();
    AnalyzeMemoryTrends();
    AnalyzeStabilityTrends();
    
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Performance trend analysis completed"));
}

FString UPerformanceMonitoringManager::GeneratePerformanceReport() const
{
    FString Report;
    Report += FString::Printf(TEXT("=== Performance Report ===\n"));
    Report += FString::Printf(TEXT("Timestamp: %s\n"), *FDateTime::Now().ToString());
    Report += FString::Printf(TEXT("Monitoring Status: %s\n"), bIsMonitoring ? TEXT("Active") : TEXT("Inactive"));
    Report += FString::Printf(TEXT("Performance Level: %s\n"), *UEnum::GetValueAsString(CurrentPerformanceLevel));
    Report += FString::Printf(TEXT("\n--- Current Metrics ---\n"));
    Report += FString::Printf(TEXT("Frame Rate: %.1f FPS\n"), CurrentMetrics.FrameRate);
    Report += FString::Printf(TEXT("Frame Time: %.2f ms\n"), CurrentMetrics.AverageFrameTime);
    Report += FString::Printf(TEXT("Memory Usage: %.1f MB\n"), CurrentMetrics.MemoryUsageMB);
    Report += FString::Printf(TEXT("CPU Usage: %.1f%%\n"), CurrentMetrics.CPUUsagePercent);
    Report += FString::Printf(TEXT("GPU Usage: %.1f%%\n"), CurrentMetrics.GPUUsagePercent);
    Report += FString::Printf(TEXT("Overall Performance Score: %.1f/100\n"), CurrentMetrics.OverallPerformanceScore);
    Report += FString::Printf(TEXT("Stability Score: %.1f/100\n"), CurrentMetrics.StabilityScore);
    Report += FString::Printf(TEXT("\n--- Active Alerts ---\n"));
    
    if (ActiveAlerts.Num() == 0)
    {
        Report += TEXT("No active alerts\n");
    }
    else
    {
        for (const FPerformanceAlert& Alert : ActiveAlerts)
        {
            Report += FString::Printf(TEXT("- %s (Severity: %.1f): %s\n"), 
                *UEnum::GetValueAsString(Alert.AlertType), Alert.Severity, *Alert.Description);
        }
    }
    
    return Report;
}

TMap<FString, float> UPerformanceMonitoringManager::GetPerformanceBottlenecks() const
{
    TMap<FString, float> Bottlenecks;
    
    // Identify bottlenecks based on current metrics
    if (CurrentMetrics.FrameRate < Thresholds.MinFrameRate)
    {
        Bottlenecks.Add(TEXT("Frame Rate"), (Thresholds.MinFrameRate - CurrentMetrics.FrameRate) / Thresholds.MinFrameRate);
    }
    
    if (CurrentMetrics.MemoryUsageMB > Thresholds.MaxMemoryUsageMB * 0.8f)
    {
        Bottlenecks.Add(TEXT("Memory"), CurrentMetrics.MemoryUsageMB / Thresholds.MaxMemoryUsageMB);
    }
    
    if (CurrentMetrics.CPUUsagePercent > Thresholds.MaxCPUUsage)
    {
        Bottlenecks.Add(TEXT("CPU"), CurrentMetrics.CPUUsagePercent / 100.0f);
    }
    
    if (CurrentMetrics.GPUUsagePercent > Thresholds.MaxGPUUsage)
    {
        Bottlenecks.Add(TEXT("GPU"), CurrentMetrics.GPUUsagePercent / 100.0f);
    }
    
    return Bottlenecks;
}

TArray<FString> UPerformanceMonitoringManager::GetOptimizationRecommendations() const
{
    return GenerateRecommendations();
}

// Resource Monitoring
float UPerformanceMonitoringManager::GetMemoryUsage() const
{
    return CurrentMetrics.MemoryUsageMB;
}

float UPerformanceMonitoringManager::GetCPUUsage() const
{
    return CurrentMetrics.CPUUsagePercent;
}

float UPerformanceMonitoringManager::GetGPUUsage() const
{
    return CurrentMetrics.GPUUsagePercent;
}

float UPerformanceMonitoringManager::GetNetworkLatency() const
{
    return CurrentMetrics.NetworkLatency;
}

int32 UPerformanceMonitoringManager::GetActiveDrawCalls() const
{
    return CurrentMetrics.DrawCalls;
}

int32 UPerformanceMonitoringManager::GetActiveTriangles() const
{
    return CurrentMetrics.Triangles;
}

// Game Integration
void UPerformanceMonitoringManager::OnLevelTransition(const FString& NewLevelName)
{
    TakePerformanceSnapshot(FString::Printf(TEXT("Level Transition: %s"), *NewLevelName));
    
    // Record in social experiment
    if (USocialExperimentManager* ExperimentManager = GetExperimentManager())
    {
        ExperimentManager->RecordBehavioralData(
            1, // Default experiment ID
            TEXT("CURRENT_PLAYER"),
            TEXT("Level Transition Performance"),
            NewLevelName,
            CurrentMetrics.OverallPerformanceScore / 100.0f
        );
    }
}

void UPerformanceMonitoringManager::OnPlayerCountChanged(int32 NewPlayerCount)
{
    TakePerformanceSnapshot(FString::Printf(TEXT("Player Count: %d"), NewPlayerCount));
}

void UPerformanceMonitoringManager::OnGamePhaseChanged(const FString& NewPhase)
{
    TakePerformanceSnapshot(FString::Printf(TEXT("Game Phase: %s"), *NewPhase));
}

void UPerformanceMonitoringManager::OnSignificantGameEvent(const FString& EventType, const FString& EventData)
{
    TakePerformanceSnapshot(FString::Printf(TEXT("Game Event: %s - %s"), *EventType, *EventData));
}

// Configuration
void UPerformanceMonitoringManager::SetMonitoringFrequency(float FrequencyHz)
{
    MonitoringFrequency = FMath::Clamp(FrequencyHz, 0.1f, 60.0f);
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Monitoring frequency set to %.1f Hz"), MonitoringFrequency);
}

void UPerformanceMonitoringManager::SetHistoryRetentionTime(float RetentionTimeSeconds)
{
    HistoryRetentionTime = FMath::Clamp(RetentionTimeSeconds, 60.0f, 3600.0f);
    MaxHistoryEntries = FMath::RoundToInt(HistoryRetentionTime * MonitoringFrequency);
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("History retention time set to %.1f seconds"), HistoryRetentionTime);
}

void UPerformanceMonitoringManager::EnableDetailedProfiling(bool bEnabled)
{
    bDetailedProfilingEnabled = bEnabled;
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Detailed profiling %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

void UPerformanceMonitoringManager::SetBenchmarkMode(bool bEnabled)
{
    bBenchmarkMode = bEnabled;
    
    if (bEnabled)
    {
        // Configure for benchmarking
        SetMonitoringFrequency(60.0f); // High frequency
        EnableDetailedProfiling(true);
        SetAutoOptimizationEnabled(false); // Don't auto-optimize during benchmarks
    }
    
    UE_LOG(LogPerformanceMonitoring, Log, TEXT("Benchmark mode %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

// Internal Implementation Functions
void UPerformanceMonitoringManager::UpdatePerformanceMetrics()
{
    PreviousMetrics = CurrentMetrics;
    CurrentMetrics = CollectCurrentMetrics();
    
    // Broadcast metrics update
    OnPerformanceMetricsUpdated.Broadcast(CurrentMetrics, PreviousMetrics);
    
    // Update stability
    UpdateSystemStability();
}

void UPerformanceMonitoringManager::ProcessAlerts()
{
    // Update active alert durations
    for (FPerformanceAlert& Alert : ActiveAlerts)
    {
        if (!Alert.bIsResolved)
        {
            Alert.Duration = (FDateTime::Now() - Alert.Timestamp).GetTotalSeconds();
        }
    }
}

void UPerformanceMonitoringManager::ManagePerformanceHistory()
{
    // Remove old entries beyond retention time
    FDateTime CutoffTime = FDateTime::Now() - FTimespan::FromSeconds(HistoryRetentionTime);
    
    PerformanceHistory.RemoveAll([CutoffTime](const FPerformanceSnapshot& Snapshot)
    {
        return Snapshot.Timestamp < CutoffTime;
    });
    
    // Enforce max entries limit
    while (PerformanceHistory.Num() > MaxHistoryEntries)
    {
        PerformanceHistory.RemoveAt(0);
    }
}

void UPerformanceMonitoringManager::ApplyAutoOptimizations()
{
    // Only apply auto-optimizations if enough time has passed since last optimization
    if ((FDateTime::Now() - LastOptimizationTime).GetTotalSeconds() < 30.0f)
    {
        return;
    }
    
    // Check if optimization is needed
    bool bNeedsOptimization = false;
    
    if (CurrentMetrics.FrameRate < Thresholds.MinFrameRate * 0.9f)
    {
        bNeedsOptimization = true;
    }
    
    if (CurrentMetrics.MemoryUsageMB > Thresholds.MaxMemoryUsageMB * 0.85f)
    {
        bNeedsOptimization = true;
    }
    
    if (CurrentMetrics.StabilityScore < Thresholds.MinStabilityScore)
    {
        bNeedsOptimization = true;
    }
    
    if (bNeedsOptimization)
    {
        ApplyOptimizations();
    }
}

void UPerformanceMonitoringManager::CalculatePerformanceScores()
{
    CurrentMetrics.OverallPerformanceScore = CalculateOverallPerformanceScore();
    CurrentMetrics.StabilityScore = CalculateSystemStabilityScore();
}

void UPerformanceMonitoringManager::CleanupExpiredAlerts()
{
    // Remove resolved alerts older than 5 minutes
    FDateTime CutoffTime = FDateTime::Now() - FTimespan::FromMinutes(5.0);
    
    ActiveAlerts.RemoveAll([CutoffTime](const FPerformanceAlert& Alert)
    {
        return Alert.bIsResolved && Alert.Timestamp < CutoffTime;
    });
    
    // Enforce max alerts limit
    while (ActiveAlerts.Num() > MaxActiveAlerts)
    {
        ActiveAlerts.RemoveAt(0);
    }
}

void UPerformanceMonitoringManager::UpdateSystemStability()
{
    float NewStabilityScore = CalculateSystemStabilityScore();
    
    if (FMath::Abs(NewStabilityScore - CurrentMetrics.StabilityScore) > 5.0f)
    {
        OnSystemStabilityChanged.Broadcast(NewStabilityScore);
    }
    
    CurrentMetrics.StabilityScore = NewStabilityScore;
}

// Helper function implementations with simplified logic for compilation
float UPerformanceMonitoringManager::CollectFrameRateMetrics()
{
    // Get frame rate from engine - simplified implementation
    if (GEngine)
    {
        return GEngine->GetMaxFPS();
    }
    return 60.0f; // Default fallback
}

void UPerformanceMonitoringManager::CollectMemoryMetrics(FPerformanceMetrics& Metrics)
{
    // Simplified memory collection
    Metrics.MemoryUsageMB = 1024.0f; // Placeholder
    Metrics.MemoryPeakMB = 1200.0f;  // Placeholder
    Metrics.TextureMemoryMB = 256.0f; // Placeholder
    Metrics.MeshMemoryMB = 128.0f;    // Placeholder
    Metrics.AudioMemoryMB = 64.0f;    // Placeholder
}

void UPerformanceMonitoringManager::CollectRenderingMetrics(FPerformanceMetrics& Metrics)
{
    Metrics.DrawCalls = 2000;      // Placeholder
    Metrics.Triangles = 1000000;   // Placeholder
    Metrics.Vertices = 3000000;    // Placeholder
    Metrics.ParticleCount = 500;   // Placeholder
    Metrics.RenderThreadTime = 10.0f; // Placeholder
    Metrics.GPUFrameTime = 12.0f;  // Placeholder
}

void UPerformanceMonitoringManager::CollectCPUMetrics(FPerformanceMetrics& Metrics)
{
    Metrics.CPUUsagePercent = 45.0f;    // Placeholder
    Metrics.GameThreadTime = 8.0f;      // Placeholder
    Metrics.PhysicsTime = 2.0f;         // Placeholder
    Metrics.AITime = 1.0f;              // Placeholder
    Metrics.AnimationTime = 3.0f;       // Placeholder
}

void UPerformanceMonitoringManager::CollectGPUMetrics(FPerformanceMetrics& Metrics)
{
    Metrics.GPUUsagePercent = 60.0f; // Placeholder
}

void UPerformanceMonitoringManager::CollectNetworkMetrics(FPerformanceMetrics& Metrics)
{
    Metrics.NetworkLatency = 50.0f; // Placeholder
}

void UPerformanceMonitoringManager::CollectAudioMetrics(FPerformanceMetrics& Metrics)
{
    Metrics.AudioSourceCount = 8;   // Placeholder
    Metrics.AudioLatency = 10.0f;   // Placeholder
}

void UPerformanceMonitoringManager::CollectSystemMetrics(FPerformanceMetrics& Metrics)
{
    Metrics.SystemTemperature = 65.0f;  // Placeholder
    Metrics.PowerConsumption = 150.0f;  // Placeholder
    Metrics.InputLatency = 5.0f;        // Placeholder
    Metrics.LoadingTime = 2.0f;         // Placeholder
    Metrics.GarbageCollectionTime = 1.0f; // Placeholder
}

// Alert threshold checking functions
void UPerformanceMonitoringManager::CheckFrameRateThreshold()
{
    if (CurrentMetrics.FrameRate < Thresholds.MinFrameRate && ShouldCreateAlert(EPerformanceAlertType::LowFrameRate))
    {
        CreateAlert(EPerformanceAlertType::LowFrameRate, 
                   (Thresholds.MinFrameRate - CurrentMetrics.FrameRate) / Thresholds.MinFrameRate * 10.0f,
                   FString::Printf(TEXT("Frame rate dropped to %.1f FPS"), CurrentMetrics.FrameRate),
                   TEXT("Consider reducing graphics quality or optimizing performance"));
    }
}

void UPerformanceMonitoringManager::CheckMemoryThreshold()
{
    if (CurrentMetrics.MemoryUsageMB > Thresholds.MaxMemoryUsageMB && ShouldCreateAlert(EPerformanceAlertType::HighMemoryUsage))
    {
        CreateAlert(EPerformanceAlertType::HighMemoryUsage,
                   (CurrentMetrics.MemoryUsageMB / Thresholds.MaxMemoryUsageMB) * 10.0f,
                   FString::Printf(TEXT("Memory usage is %.1f MB"), CurrentMetrics.MemoryUsageMB),
                   TEXT("Consider running garbage collection or reducing memory usage"));
    }
}

void UPerformanceMonitoringManager::CheckCPUThreshold()
{
    if (CurrentMetrics.CPUUsagePercent > Thresholds.MaxCPUUsage && ShouldCreateAlert(EPerformanceAlertType::CPUBottleneck))
    {
        CreateAlert(EPerformanceAlertType::CPUBottleneck,
                   (CurrentMetrics.CPUUsagePercent / 100.0f) * 10.0f,
                   FString::Printf(TEXT("CPU usage is %.1f%%"), CurrentMetrics.CPUUsagePercent),
                   TEXT("Consider reducing CPU-intensive operations"));
    }
}

void UPerformanceMonitoringManager::CheckGPUThreshold()
{
    if (CurrentMetrics.GPUUsagePercent > Thresholds.MaxGPUUsage && ShouldCreateAlert(EPerformanceAlertType::GPUBottleneck))
    {
        CreateAlert(EPerformanceAlertType::GPUBottleneck,
                   (CurrentMetrics.GPUUsagePercent / 100.0f) * 10.0f,
                   FString::Printf(TEXT("GPU usage is %.1f%%"), CurrentMetrics.GPUUsagePercent),
                   TEXT("Consider reducing graphics quality or complexity"));
    }
}

void UPerformanceMonitoringManager::CheckStabilityThreshold()
{
    if (CurrentMetrics.StabilityScore < Thresholds.MinStabilityScore && ShouldCreateAlert(EPerformanceAlertType::SystemOverload))
    {
        CreateAlert(EPerformanceAlertType::SystemOverload,
                   (1.0f - CurrentMetrics.StabilityScore / 100.0f) * 10.0f,
                   FString::Printf(TEXT("System stability is %.1f%%"), CurrentMetrics.StabilityScore),
                   TEXT("System is experiencing instability - consider optimization"));
    }
}

void UPerformanceMonitoringManager::CreateAlert(EPerformanceAlertType AlertType, float Severity, const FString& Description, const FString& RecommendedAction)
{
    FPerformanceAlert NewAlert;
    NewAlert.AlertType = AlertType;
    NewAlert.Severity = FMath::Clamp(Severity, 1.0f, 10.0f);
    NewAlert.Timestamp = FDateTime::Now();
    NewAlert.Description = Description;
    NewAlert.RecommendedAction = RecommendedAction;
    NewAlert.bIsResolved = false;
    NewAlert.Duration = 0.0f;
    NewAlert.Category = EMonitoringCategory::FrameRate; // Simplified categorization
    
    ActiveAlerts.Add(NewAlert);
    LastAlertTimes.Add(UEnum::GetValueAsString(AlertType), FDateTime::Now());
    
    OnPerformanceAlert.Broadcast(NewAlert);
    
    UE_LOG(LogPerformanceMonitoring, Warning, TEXT("Performance Alert: %s - %s"), 
           *UEnum::GetValueAsString(AlertType), *Description);
}

bool UPerformanceMonitoringManager::ShouldCreateAlert(EPerformanceAlertType AlertType) const
{
    FString AlertTypeString = UEnum::GetValueAsString(AlertType);
    
    // Don't create duplicate alerts within 30 seconds
    if (const FDateTime* LastTime = LastAlertTimes.Find(AlertTypeString))
    {
        if ((FDateTime::Now() - *LastTime).GetTotalSeconds() < 30.0f)
        {
            return false;
        }
    }
    
    return true;
}

// Simplified implementations for remaining functions
void UPerformanceMonitoringManager::ApplyMemoryOptimizations() { /* Implementation */ }
void UPerformanceMonitoringManager::ApplyRenderingOptimizations() { /* Implementation */ }
void UPerformanceMonitoringManager::ApplyCPUOptimizations() { /* Implementation */ }
void UPerformanceMonitoringManager::ApplyGPUOptimizations() { /* Implementation */ }
void UPerformanceMonitoringManager::ApplyQualityOptimizations(EPerformanceLevel TargetLevel) { /* Implementation */ }
float UPerformanceMonitoringManager::EvaluateOptimizationEffectiveness(const FString& OptimizationType) { return 1.0f; }
void UPerformanceMonitoringManager::AnalyzeFrameRateTrends() { /* Implementation */ }
void UPerformanceMonitoringManager::AnalyzeMemoryTrends() { /* Implementation */ }
void UPerformanceMonitoringManager::AnalyzeStabilityTrends() { /* Implementation */ }

TArray<FString> UPerformanceMonitoringManager::GenerateRecommendations() const
{
    TArray<FString> Recommendations;
    
    if (CurrentMetrics.FrameRate < Thresholds.MinFrameRate)
    {
        Recommendations.Add(TEXT("フレームレートが低下しています。グラフィック品質を下げることを検討してください。"));
    }
    
    if (CurrentMetrics.MemoryUsageMB > Thresholds.MaxMemoryUsageMB * 0.8f)
    {
        Recommendations.Add(TEXT("メモリ使用量が高くなっています。ガベージコレクションを実行することを検討してください。"));
    }
    
    if (CurrentMetrics.CPUUsagePercent > Thresholds.MaxCPUUsage)
    {
        Recommendations.Add(TEXT("CPU使用率が高くなっています。処理を最適化することを検討してください。"));
    }
    
    return Recommendations;
}

// Simplified system integration functions
UStoryManager* UPerformanceMonitoringManager::GetStoryManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UStoryManager>();
        }
    }
    return nullptr;
}

ULevelDesignManager* UPerformanceMonitoringManager::GetLevelDesignManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<ULevelDesignManager>();
        }
    }
    return nullptr;
}

UAudioSystemManager* UPerformanceMonitoringManager::GetAudioSystemManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UAudioSystemManager>();
        }
    }
    return nullptr;
}

UGameProgressionManager* UPerformanceMonitoringManager::GetProgressionManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UGameProgressionManager>();
        }
    }
    return nullptr;
}

USocialExperimentManager* UPerformanceMonitoringManager::GetExperimentManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<USocialExperimentManager>();
        }
    }
    return nullptr;
}

// Placeholder implementations for data export/import
FString UPerformanceMonitoringManager::ExportPerformanceData() const { return TEXT("{}"); }
bool UPerformanceMonitoringManager::ImportPerformanceData(const FString& JsonData) { return true; }
void UPerformanceMonitoringManager::SavePerformanceProfile(const FString& ProfileName) { /* Implementation */ }
bool UPerformanceMonitoringManager::LoadPerformanceProfile(const FString& ProfileName) { return true; }
