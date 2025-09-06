// Project Visible - Performance Monitoring Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PerformanceMonitoringTypes.generated.h"

/**
 * Performance alert severity levels
 */
UENUM(BlueprintType)
enum class EPerformanceAlertType : uint8
{
    LowFrameRate        UMETA(DisplayName = "Low Frame Rate"),         // フレームレート低下
    HighMemoryUsage     UMETA(DisplayName = "High Memory Usage"),      // メモリ使用量過多
    GPUBottleneck       UMETA(DisplayName = "GPU Bottleneck"),         // GPU ボトルネック
    CPUBottleneck       UMETA(DisplayName = "CPU Bottleneck"),         // CPU ボトルネック
    SystemOverload      UMETA(DisplayName = "System Overload"),        // システム過負荷
    MemoryLeak          UMETA(DisplayName = "Memory Leak"),            // メモリリーク
    ThreadStarvation    UMETA(DisplayName = "Thread Starvation"),      // スレッド飢餓
    AssetLoadBottleneck UMETA(DisplayName = "Asset Load Bottleneck"),  // アセット読み込みボトルネック
    NetworkLatency      UMETA(DisplayName = "Network Latency"),        // ネットワーク遅延
    AudioStutter        UMETA(DisplayName = "Audio Stutter")           // オーディオ途切れ
};

/**
 * Performance optimization levels
 */
UENUM(BlueprintType)
enum class EPerformanceLevel : uint8
{
    Lowest              UMETA(DisplayName = "Lowest Quality"),         // 最低品質
    Low                 UMETA(DisplayName = "Low Quality"),            // 低品質
    Medium              UMETA(DisplayName = "Medium Quality"),         // 中品質
    High                UMETA(DisplayName = "High Quality"),           // 高品質
    Highest             UMETA(DisplayName = "Highest Quality"),        // 最高品質
    Custom              UMETA(DisplayName = "Custom Settings")         // カスタム設定
};

/**
 * System resource types for monitoring
 */
UENUM(BlueprintType)
enum class EResourceType : uint8
{
    Memory              UMETA(DisplayName = "Memory"),                 // メモリ
    CPU                 UMETA(DisplayName = "CPU"),                   // CPU
    GPU                 UMETA(DisplayName = "GPU"),                   // GPU
    Network             UMETA(DisplayName = "Network"),               // ネットワーク
    Storage             UMETA(DisplayName = "Storage"),               // ストレージ
    Audio               UMETA(DisplayName = "Audio"),                 // オーディオ
    Rendering           UMETA(DisplayName = "Rendering"),             // レンダリング
    Physics             UMETA(DisplayName = "Physics"),               // 物理演算
    AI                  UMETA(DisplayName = "AI"),                    // AI処理
    Animation           UMETA(DisplayName = "Animation")              // アニメーション
};

/**
 * Performance monitoring categories
 */
UENUM(BlueprintType)
enum class EMonitoringCategory : uint8
{
    FrameRate           UMETA(DisplayName = "Frame Rate"),            // フレームレート
    MemoryUsage         UMETA(DisplayName = "Memory Usage"),         // メモリ使用量
    CPUUsage            UMETA(DisplayName = "CPU Usage"),            // CPU使用率
    GPUUsage            UMETA(DisplayName = "GPU Usage"),            // GPU使用率
    NetworkPerformance  UMETA(DisplayName = "Network Performance"),  // ネットワーク性能
    LoadTimes           UMETA(DisplayName = "Load Times"),           // 読み込み時間
    RenderingStats      UMETA(DisplayName = "Rendering Stats"),      // レンダリング統計
    AudioPerformance    UMETA(DisplayName = "Audio Performance"),    // オーディオ性能
    InputLatency        UMETA(DisplayName = "Input Latency"),        // 入力遅延
    SystemStability     UMETA(DisplayName = "System Stability")      // システム安定性
};

/**
 * Comprehensive performance metrics structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FPerformanceMetrics
{
    GENERATED_BODY()

    FPerformanceMetrics()
    {
        Timestamp = FDateTime::Now();
        FrameRate = 0.0f;
        AverageFrameTime = 0.0f;
        MinFrameTime = 0.0f;
        MaxFrameTime = 0.0f;
        FrameTimeVariance = 0.0f;
        MemoryUsageMB = 0.0f;
        MemoryPeakMB = 0.0f;
        GPUUsagePercent = 0.0f;
        CPUUsagePercent = 0.0f;
        DrawCalls = 0;
        Triangles = 0;
        Vertices = 0;
        RenderThreadTime = 0.0f;
        GameThreadTime = 0.0f;
        GPUFrameTime = 0.0f;
        InputLatency = 0.0f;
        NetworkLatency = 0.0f;
        AudioLatency = 0.0f;
        LoadingTime = 0.0f;
        GarbageCollectionTime = 0.0f;
        PhysicsTime = 0.0f;
        AITime = 0.0f;
        AnimationTime = 0.0f;
        ParticleCount = 0;
        AudioSourceCount = 0;
        TextureMemoryMB = 0.0f;
        MeshMemoryMB = 0.0f;
        AudioMemoryMB = 0.0f;
        SystemTemperature = 0.0f;
        PowerConsumption = 0.0f;
        QualityLevel = EPerformanceLevel::Medium;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Metrics")
    FDateTime Timestamp;

    // Frame Performance
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame Performance")
    float FrameRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame Performance")
    float AverageFrameTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame Performance")
    float MinFrameTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame Performance")
    float MaxFrameTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Frame Performance")
    float FrameTimeVariance;

    // Memory Statistics
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    float MemoryUsageMB;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    float MemoryPeakMB;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    float TextureMemoryMB;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    float MeshMemoryMB;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    float AudioMemoryMB;

    // Processing Performance
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing")
    float GPUUsagePercent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing")
    float CPUUsagePercent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing")
    float RenderThreadTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing")
    float GameThreadTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing")
    float GPUFrameTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing")
    float PhysicsTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing")
    float AITime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing")
    float AnimationTime;

    // Rendering Statistics
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    int32 DrawCalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    int32 Triangles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    int32 Vertices;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    int32 ParticleCount;

    // Latency Measurements
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Latency")
    float InputLatency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Latency")
    float NetworkLatency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Latency")
    float AudioLatency;

    // Loading Performance
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading")
    float LoadingTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading")
    float GarbageCollectionTime;

    // Audio Performance
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    int32 AudioSourceCount;

    // System Information
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System")
    float SystemTemperature;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System")
    float PowerConsumption;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System")
    EPerformanceLevel QualityLevel;

    // Custom Metrics
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    TMap<FString, float> CustomMetrics;

    // Performance Scores
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scores")
    float OverallPerformanceScore; // 0-100

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scores")
    float StabilityScore; // 0-100
};

/**
 * Performance alert information
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FPerformanceAlert
{
    GENERATED_BODY()

    FPerformanceAlert()
    {
        AlertType = EPerformanceAlertType::LowFrameRate;
        Severity = 1.0f;
        Timestamp = FDateTime::Now();
        Description = TEXT("");
        RecommendedAction = TEXT("");
        bIsResolved = false;
        Duration = 0.0f;
        AffectedResource = EResourceType::Memory;
        Category = EMonitoringCategory::FrameRate;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
    EPerformanceAlertType AlertType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
    float Severity; // 0-10

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
    FString RecommendedAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
    bool bIsResolved;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
    float Duration; // Alert duration in seconds

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
    EResourceType AffectedResource;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
    EMonitoringCategory Category;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alert")
    TMap<FString, FString> AdditionalData;
};

/**
 * Performance thresholds for monitoring
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FPerformanceThresholds
{
    GENERATED_BODY()

    FPerformanceThresholds()
    {
        MinFrameRate = 30.0f;
        MaxFrameTime = 33.33f;
        MaxMemoryUsageMB = 8192.0f;
        MaxCPUUsage = 80.0f;
        MaxGPUUsage = 90.0f;
        MaxInputLatency = 50.0f;
        MaxLoadTime = 10.0f;
        MaxGCTime = 5.0f;
        MinStabilityScore = 70.0f;
        MaxFrameTimeVariance = 10.0f;
        MaxDrawCalls = 5000;
        MaxTriangles = 2000000;
        MaxAudioSources = 64;
        MaxParticles = 10000;
        MaxSystemTemperature = 85.0f;
        MaxPowerConsumption = 200.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MinFrameRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MaxFrameTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MaxMemoryUsageMB;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MaxCPUUsage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MaxGPUUsage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MaxInputLatency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MaxLoadTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MaxGCTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MinStabilityScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MaxFrameTimeVariance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    int32 MaxDrawCalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    int32 MaxTriangles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    int32 MaxAudioSources;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    int32 MaxParticles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MaxSystemTemperature;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    float MaxPowerConsumption;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thresholds")
    TMap<FString, float> CustomThresholds;
};

/**
 * Performance optimization configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FOptimizationSettings
{
    GENERATED_BODY()

    FOptimizationSettings()
    {
        bAutoOptimizationEnabled = true;
        bDynamicQualityAdjustment = true;
        bMemoryPooling = true;
        bCullingOptimization = true;
        bLODAutoAdjustment = true;
        bTextureStreaming = true;
        bAsyncLoading = true;
        bGarbageCollectionOptimization = true;
        bPhysicsOptimization = true;
        bAudioOptimization = true;
        bNetworkOptimization = true;
        bRenderingOptimization = true;
        OptimizationAggressiveness = 0.5f;
        QualityAdjustmentSensitivity = 0.3f;
        MemoryManagementLevel = 0.7f;
        RenderingOptimizationLevel = 0.6f;
        PhysicsOptimizationLevel = 0.4f;
        AudioOptimizationLevel = 0.5f;
        NetworkOptimizationLevel = 0.3f;
        LoadingOptimizationLevel = 0.8f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bAutoOptimizationEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bDynamicQualityAdjustment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bMemoryPooling;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bCullingOptimization;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bLODAutoAdjustment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bTextureStreaming;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bAsyncLoading;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bGarbageCollectionOptimization;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bPhysicsOptimization;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bAudioOptimization;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bNetworkOptimization;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    bool bRenderingOptimization;

    // Optimization intensity levels (0.0-1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Levels", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float OptimizationAggressiveness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Levels", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float QualityAdjustmentSensitivity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Levels", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MemoryManagementLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Levels", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float RenderingOptimizationLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Levels", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float PhysicsOptimizationLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Levels", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AudioOptimizationLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Levels", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float NetworkOptimizationLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Levels", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float LoadingOptimizationLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization Settings")
    TMap<FString, float> CustomOptimizationSettings;
};

/**
 * Performance snapshot for trend analysis
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FPerformanceSnapshot
{
    GENERATED_BODY()

    FPerformanceSnapshot()
    {
        Timestamp = FDateTime::Now();
        SessionID = TEXT("");
        PlayerID = TEXT("");
        GamePhase = TEXT("");
        Location = TEXT("");
        PlayerCount = 1;
        ActiveSystemCount = 0;
        bIsSignificantEvent = false;
        EventType = TEXT("");
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snapshot")
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snapshot")
    FPerformanceMetrics Metrics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snapshot")
    TArray<FPerformanceAlert> ActiveAlerts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    FString SessionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    FString PlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    FString GamePhase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    FString Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    int32 PlayerCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    int32 ActiveSystemCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    bool bIsSignificantEvent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    FString EventType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    TMap<FString, FString> ContextualData;
};
