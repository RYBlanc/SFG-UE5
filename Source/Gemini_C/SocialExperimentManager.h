// Project Visible - Social Experiment Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "SocialExperimentTypes.h"
#include "VirtueMemoryTypes.h"
#include "SocialExperimentManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExperimentStarted, const FSocialExperiment&, Experiment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExperimentCompleted, const FSocialExperiment&, Experiment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParticipantAdded, const FParticipantData&, Participant);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDataPointCollected, const FExperimentDataPoint&, DataPoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnalysisCompleted, const FExperimentResults&, Results);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConsentUpdate, const FString&, ParticipantID, bool, ConsentGiven);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPrivacyViolationDetected, const FString&, ViolationDetails);

/**
 * Central manager for social experiments and behavioral data collection
 * Manages ethical data collection, analysis, and privacy protection
 */
UCLASS()
class GEMINI_C_API USocialExperimentManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Experiment Management
    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void InitializeExperimentSystem(const FDataPrivacyConfig& InPrivacyConfig, const FAnalyticsConfig& InAnalyticsConfig);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    int32 CreateExperiment(const FString& Name, const FString& Description, ESocialExperimentType Type,
                          EPrivacyLevel PrivacyLevel = EPrivacyLevel::Anonymous);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    bool StartExperiment(int32 ExperimentID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    bool CompleteExperiment(int32 ExperimentID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    bool PauseExperiment(int32 ExperimentID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    bool CancelExperiment(int32 ExperimentID);

    // Participant Management
    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    FString RegisterParticipant(int32 Age = 0, const FString& Gender = TEXT(""), const FString& Country = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    bool RequestConsent(const FString& ParticipantID, const FString& Purpose, const FString& DataUsage);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    bool UpdateConsent(const FString& ParticipantID, bool ConsentGiven);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    bool WithdrawParticipant(const FString& ParticipantID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    bool DeleteParticipantData(const FString& ParticipantID);

    // Data Collection
    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void RecordBehavioralData(int32 ExperimentID, const FString& ParticipantID, const FString& BehaviorType,
                             const FString& Response, float ResponseTime = 0.0f);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void RecordMoralChoice(int32 ExperimentID, const FString& ParticipantID, const FString& DilemmaType,
                          const FString& Choice, float ConfidenceLevel = 50.0f);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void RecordSocialInteraction(int32 ExperimentID, const FString& ParticipantID, const FString& InteractionType,
                               const TArray<FString>& OtherParticipants, float CooperationLevel = 50.0f);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void RecordEmotionalResponse(int32 ExperimentID, const FString& ParticipantID, const FString& Stimulus,
                               float EmotionalIntensity, const FString& EmotionType);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void RecordDecisionMaking(int32 ExperimentID, const FString& ParticipantID, const FString& DecisionContext,
                            const TArray<FString>& Options, const FString& SelectedOption, float DecisionTime);

    // Value and Happiness Analysis
    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void AnalyzePlayerValues(const FString& ParticipantID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void AssessHappinessFactors(const FString& ParticipantID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    TArray<FPlayerValueAssessment> GetPlayerValueProfile(const FString& ParticipantID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    FHappinessMetrics GetHappinessProfile(const FString& ParticipantID);

    // Hidden Experiments (Implicit Measurement)
    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void TriggerHiddenExperiment(ESocialExperimentType Type, const FString& Context);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void RecordImplicitBehavior(const FString& BehaviorType, float Intensity, const FString& Context);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void AnalyzeGameplayPatterns(const FString& ParticipantID);

    // Data Retrieval and Analysis
    UFUNCTION(BlueprintCallable, Category = "Social Experiments", BlueprintPure)
    bool GetExperiment(int32 ExperimentID, FSocialExperiment& OutExperiment) const;

    UFUNCTION(BlueprintCallable, Category = "Social Experiments", BlueprintPure)
    TArray<FSocialExperiment> GetActiveExperiments() const;

    UFUNCTION(BlueprintCallable, Category = "Social Experiments", BlueprintPure)
    TArray<FExperimentDataPoint> GetExperimentData(int32 ExperimentID) const;

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    FExperimentResults AnalyzeExperimentResults(int32 ExperimentID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    TArray<FExperimentResults> GenerateAggregatedReport();

    // Privacy and Ethics
    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    bool ValidateEthicsCompliance(int32 ExperimentID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void AnonymizeParticipantData(const FString& ParticipantID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void EncryptSensitiveData();

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void AuditDataAccess(const FString& AccessorID, const FString& Purpose);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    bool CheckDataRetention();

    // Statistical Analysis
    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    float CalculateStatisticalSignificance(const TArray<float>& Group1, const TArray<float>& Group2);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    float CalculateEffectSize(const TArray<float>& Group1, const TArray<float>& Group2);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    TMap<FString, float> GenerateDescriptiveStatistics(const TArray<float>& Data);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    TArray<FString> DetectDataAnomalies(int32 ExperimentID);

    // Real-time Analysis
    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void UpdateRealTimeAnalysis();

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    void GenerateInsights(int32 ExperimentID);

    UFUNCTION(BlueprintCallable, Category = "Social Experiments")
    TArray<FString> GetRecommendations(const FString& ParticipantID);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Social Experiment Events")
    FOnExperimentStarted OnExperimentStarted;

    UPROPERTY(BlueprintAssignable, Category = "Social Experiment Events")
    FOnExperimentCompleted OnExperimentCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Social Experiment Events")
    FOnParticipantAdded OnParticipantAdded;

    UPROPERTY(BlueprintAssignable, Category = "Social Experiment Events")
    FOnDataPointCollected OnDataPointCollected;

    UPROPERTY(BlueprintAssignable, Category = "Social Experiment Events")
    FOnAnalysisCompleted OnAnalysisCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Social Experiment Events")
    FOnConsentUpdate OnConsentUpdate;

    UPROPERTY(BlueprintAssignable, Category = "Social Experiment Events")
    FOnPrivacyViolationDetected OnPrivacyViolationDetected;

protected:
    // Experiment Storage
    UPROPERTY(BlueprintReadOnly, Category = "Experiment Data")
    TArray<FSocialExperiment> Experiments;

    UPROPERTY(BlueprintReadOnly, Category = "Experiment Data")
    TArray<FParticipantData> Participants;

    UPROPERTY(BlueprintReadOnly, Category = "Experiment Data")
    TArray<FExperimentDataPoint> DataPoints;

    UPROPERTY(BlueprintReadOnly, Category = "Experiment Data")
    TArray<FExperimentResults> Results;

    // Configuration
    UPROPERTY(BlueprintReadOnly, Category = "Experiment Settings")
    FDataPrivacyConfig PrivacyConfig;

    UPROPERTY(BlueprintReadOnly, Category = "Experiment Settings")
    FAnalyticsConfig AnalyticsConfig;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment Settings")
    bool bCollectAnonymousData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment Settings")
    bool bRequireExplicitConsent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment Settings")
    bool bUseRealTimeAnalysis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment Settings")
    float DataRetentionDays;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment Settings")
    int32 MaxActiveExperiments;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experiment Settings")
    int32 MaxParticipantsPerExperiment;

    // ID Generators
    UPROPERTY()
    int32 NextExperimentID;

    UPROPERTY()
    int32 NextDataPointID;

    // Timing
    UPROPERTY()
    FDateTime LastAnalysisUpdate;

    UPROPERTY()
    FDateTime LastDataCleanup;

    UPROPERTY()
    FDateTime LastPrivacyCheck;

private:
    // Helper Functions
    FParticipantData* GetParticipantByID(const FString& ParticipantID);
    const FParticipantData* GetParticipantByID(const FString& ParticipantID) const;
    FSocialExperiment* GetExperimentByID(int32 ExperimentID);
    const FSocialExperiment* GetExperimentByID(int32 ExperimentID) const;
    
    FString GenerateAnonymousID();
    bool ValidateConsentRequirements(int32 ExperimentID, const FString& ParticipantID);
    void ProcessDataPoint(const FExperimentDataPoint& DataPoint);
    
    // Analysis Functions
    void PerformStatisticalAnalysis(int32 ExperimentID);
    void UpdateParticipantProfile(const FString& ParticipantID, const FExperimentDataPoint& DataPoint);
    void DetectBehavioralPatterns(const FString& ParticipantID);
    
    // Privacy Functions
    void ApplyDataMinimization();
    void ScheduleDataDeletion(const FString& ParticipantID);
    bool CheckEthicsViolation(const FExperimentDataPoint& DataPoint);
    void LogDataAccess(const FString& DataType, const FString& Purpose);
    
    // Real-time Processing
    void ProcessRealTimeUpdates(float DeltaTime);
    void UpdateAnalyticsModels();
    void GenerateAutomaticInsights();
    
    // Hidden Experiment Management
    TMap<FString, TArray<FExperimentDataPoint>> ImplicitDataCache;
    void ProcessImplicitBehavior();
    void CorrelateWithExplicitData();
};
