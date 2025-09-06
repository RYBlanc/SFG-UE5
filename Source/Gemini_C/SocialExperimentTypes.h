// Project Visible - Social Experiment System Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "VirtueMemoryTypes.h"
#include "SocialExperimentTypes.generated.h"

/**
 * Types of social experiments
 */
UENUM(BlueprintType)
enum class ESocialExperimentType : uint8
{
    MoralDilemma        UMETA(DisplayName = "Moral Dilemma"),           // 道徳的ジレンマ
    TrustTest           UMETA(DisplayName = "Trust Test"),              // 信頼テスト
    AltruismMeasure     UMETA(DisplayName = "Altruism Measure"),       // 利他主義測定
    FairnessAssessment  UMETA(DisplayName = "Fairness Assessment"),    // 公平性評価
    RiskTaking          UMETA(DisplayName = "Risk Taking"),            // リスク志向
    SocialConformity    UMETA(DisplayName = "Social Conformity"),      // 社会的同調
    AuthorityObedience  UMETA(DisplayName = "Authority Obedience"),    // 権威服従
    GroupDynamics       UMETA(DisplayName = "Group Dynamics"),         // 集団力学
    DecisionMaking      UMETA(DisplayName = "Decision Making"),        // 意思決定
    EmpatheticResponse  UMETA(DisplayName = "Empathetic Response"),    // 共感反応
    ResourceSharing     UMETA(DisplayName = "Resource Sharing"),       // 資源共有
    CompetitiveCooperation UMETA(DisplayName = "Competitive Cooperation") // 競争・協力
};

/**
 * Experiment status
 */
UENUM(BlueprintType)
enum class EExperimentStatus : uint8
{
    Planned             UMETA(DisplayName = "Planned"),                // 計画中
    Active              UMETA(DisplayName = "Active"),                 // 実行中
    Completed           UMETA(DisplayName = "Completed"),              // 完了
    Paused              UMETA(DisplayName = "Paused"),                 // 一時停止
    Cancelled           UMETA(DisplayName = "Cancelled"),              // キャンセル
    Failed              UMETA(DisplayName = "Failed")                  // 失敗
};

/**
 * Data collection methods
 */
UENUM(BlueprintType)
enum class EDataCollectionMethod : uint8
{
    BehaviorObservation UMETA(DisplayName = "Behavior Observation"),   // 行動観察
    ChoiceAnalysis      UMETA(DisplayName = "Choice Analysis"),        // 選択分析
    ResponseTime        UMETA(DisplayName = "Response Time"),          // 反応時間
    PhysiologicalData   UMETA(DisplayName = "Physiological Data"),     // 生理的データ
    SelfReport          UMETA(DisplayName = "Self Report"),            // 自己報告
    ImplicitMeasure     UMETA(DisplayName = "Implicit Measure"),       // 潜在測定
    SocialInteraction   UMETA(DisplayName = "Social Interaction"),     // 社会的相互作用
    GameMetrics         UMETA(DisplayName = "Game Metrics")            // ゲーム指標
};

/**
 * Privacy levels for data collection
 */
UENUM(BlueprintType)
enum class EPrivacyLevel : uint8
{
    Anonymous           UMETA(DisplayName = "Anonymous"),              // 匿名
    Pseudonymous        UMETA(DisplayName = "Pseudonymous"),           // 仮名
    Confidential        UMETA(DisplayName = "Confidential"),           // 機密
    Sensitive           UMETA(DisplayName = "Sensitive"),              // センシティブ
    Public              UMETA(DisplayName = "Public")                  // 公開
};

/**
 * Experiment ethics approval status
 */
UENUM(BlueprintType)
enum class EEthicsApprovalStatus : uint8
{
    Pending             UMETA(DisplayName = "Pending Review"),         // 審査中
    Approved            UMETA(DisplayName = "Approved"),               // 承認済み
    ConditionalApproval UMETA(DisplayName = "Conditional Approval"),  // 条件付き承認
    Rejected            UMETA(DisplayName = "Rejected"),               // 却下
    UnderReview         UMETA(DisplayName = "Under Review"),           // 再審査中
    Exempt              UMETA(DisplayName = "Exempt")                  // 免除
};

/**
 * Individual experiment definition
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FSocialExperiment
{
    GENERATED_BODY()

    FSocialExperiment()
    {
        ExperimentID = 0;
        ExperimentType = ESocialExperimentType::MoralDilemma;
        Status = EExperimentStatus::Planned;
        PrivacyLevel = EPrivacyLevel::Anonymous;
        EthicsStatus = EEthicsApprovalStatus::Pending;
        Priority = 1;
        MinParticipants = 1;
        MaxParticipants = 1;
        EstimatedDurationMinutes = 5.0f;
        bRequiresConsent = true;
        bIsHidden = true;
        bCollectsPersonalData = false;
        StartTime = FDateTime::Now();
        CompletionTime = FDateTime::MinValue();
        SuccessRate = 0.0f;
        ParticipantCount = 0;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    int32 ExperimentID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    FString ExperimentName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    ESocialExperimentType ExperimentType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    EExperimentStatus Status;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    EPrivacyLevel PrivacyLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    EEthicsApprovalStatus EthicsStatus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    int32 Priority;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    int32 MinParticipants;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    int32 MaxParticipants;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    float EstimatedDurationMinutes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    bool bRequiresConsent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    bool bIsHidden;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    bool bCollectsPersonalData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    FDateTime StartTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    FDateTime CompletionTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    float SuccessRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    int32 ParticipantCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    TArray<EDataCollectionMethod> DataCollectionMethods;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    TArray<FString> Hypotheses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    TArray<FString> Variables;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    TMap<FString, FString> ExperimentParameters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment")
    TArray<FString> EthicsConsiderations;
};

/**
 * Participant data (anonymized)
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FParticipantData
{
    GENERATED_BODY()

    FParticipantData()
    {
        ParticipantID = TEXT("");
        SessionID = TEXT("");
        Age = 0;
        Gender = TEXT("");
        ConsentGiven = false;
        StartTime = FDateTime::Now();
        EndTime = FDateTime::MinValue();
        CompletedExperiments = 0;
        AverageResponseTime = 0.0f;
        EngagementLevel = 50.0f;
        StressLevel = 50.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    FString ParticipantID; // Anonymized/Hashed ID

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    FString SessionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    int32 Age;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    FString Gender;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    FString Country;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    FString Language;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    bool ConsentGiven;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    FDateTime StartTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    FDateTime EndTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    int32 CompletedExperiments;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    float AverageResponseTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    float EngagementLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    float StressLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    TMap<FString, FString> Demographics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    TMap<FString, float> PsychometricScores;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participant")
    TArray<FString> PreferencesData;
};

/**
 * Experiment result data point
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FExperimentDataPoint
{
    GENERATED_BODY()

    FExperimentDataPoint()
    {
        DataPointID = 0;
        ExperimentID = 0;
        Timestamp = FDateTime::Now();
        ResponseTime = 0.0f;
        ConfidenceLevel = 50.0f;
        EmotionalState = 50.0f;
        StressLevel = 50.0f;
        bIsValidResponse = true;
        bIsOutlier = false;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    int32 DataPointID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    int32 ExperimentID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    FString ParticipantID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    FString DataType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    FString Response;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    float ResponseTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    float ConfidenceLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    float EmotionalState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    float StressLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    bool bIsValidResponse;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    bool bIsOutlier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    TMap<FString, FString> ContextualData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    TMap<FString, float> NumericData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Point")
    TArray<FString> TaggedBehaviors;
};

/**
 * Aggregated experiment results
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FExperimentResults
{
    GENERATED_BODY()

    FExperimentResults()
    {
        ExperimentID = 0;
        TotalParticipants = 0;
        ValidResponses = 0;
        AverageResponseTime = 0.0f;
        StandardDeviation = 0.0f;
        ConfidenceInterval = 95.0f;
        PValue = 1.0f;
        EffectSize = 0.0f;
        StatisticalPower = 0.0f;
        CompletionDate = FDateTime::MinValue();
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    int32 ExperimentID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    FString ExperimentName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    int32 TotalParticipants;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    int32 ValidResponses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    float AverageResponseTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    float StandardDeviation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    float ConfidenceInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    float PValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    float EffectSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    float StatisticalPower;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    FDateTime CompletionDate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    TMap<FString, float> DescriptiveStatistics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    TMap<FString, float> CorrelationAnalysis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    TArray<FString> KeyFindings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    TArray<FString> LimitationsAndBiases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Results")
    TMap<FString, FString> QualitativeInsights;
};

/**
 * Data privacy and ethics configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FDataPrivacyConfig
{
    GENERATED_BODY()

    FDataPrivacyConfig()
    {
        DataRetentionDays = 365;
        bUseEncryption = true;
        bAnonymizeData = true;
        bRequireConsent = true;
        bAllowOptOut = true;
        bShareAggregatedData = false;
        bExportRawData = false;
        bAuditAccess = true;
        ConsentVersion = TEXT("1.0");
        PrivacyPolicyVersion = TEXT("1.0");
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    int32 DataRetentionDays;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    bool bUseEncryption;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    bool bAnonymizeData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    bool bRequireConsent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    bool bAllowOptOut;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    bool bShareAggregatedData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    bool bExportRawData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    bool bAuditAccess;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    FString ConsentVersion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    FString PrivacyPolicyVersion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    TArray<FString> DataProcessingPurposes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    TArray<FString> LegalBases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Privacy")
    TMap<FString, FString> DataCategories;
};

/**
 * Real-time analytics configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FAnalyticsConfig
{
    GENERATED_BODY()

    FAnalyticsConfig()
    {
        bRealTimeAnalysis = true;
        bGenerateReports = true;
        bUseMLAnalysis = false;
        bDetectAnomalies = true;
        AnalysisFrequencySeconds = 60.0f;
        ReportGenerationFrequency = 3600.0f;
        MinDataPointsForAnalysis = 10;
        ConfidenceThreshold = 0.95f;
        bUsePredictiveAnalytics = false;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    bool bRealTimeAnalysis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    bool bGenerateReports;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    bool bUseMLAnalysis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    bool bDetectAnomalies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    float AnalysisFrequencySeconds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    float ReportGenerationFrequency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    int32 MinDataPointsForAnalysis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    float ConfidenceThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    bool bUsePredictiveAnalytics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    TArray<FString> AnalysisModels;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    TMap<FString, float> ModelParameters;
};
