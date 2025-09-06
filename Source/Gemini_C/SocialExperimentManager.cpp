// Project Visible - Social Experiment Manager Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "SocialExperimentManager.h"
#include "Gemini_C.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Misc/Guid.h"
#include "Misc/DateTime.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

void USocialExperimentManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize settings
    bCollectAnonymousData = true;
    bRequireExplicitConsent = true;
    bUseRealTimeAnalysis = true;
    DataRetentionDays = 365.0f;
    MaxActiveExperiments = 10;
    MaxParticipantsPerExperiment = 1000;
    
    // Initialize ID generators
    NextExperimentID = 1;
    NextDataPointID = 1;
    
    // Initialize timing
    LastAnalysisUpdate = FDateTime::Now();
    LastDataCleanup = FDateTime::Now();
    LastPrivacyCheck = FDateTime::Now();
    
    // Initialize default configurations
    FDataPrivacyConfig DefaultPrivacy;
    FAnalyticsConfig DefaultAnalytics;
    InitializeExperimentSystem(DefaultPrivacy, DefaultAnalytics);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Social Experiment Manager initialized"));
}

void USocialExperimentManager::Deinitialize()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Social Experiment Manager shutting down"));
    
    // Complete any active experiments
    for (FSocialExperiment& Experiment : Experiments)
    {
        if (Experiment.Status == EExperimentStatus::Active)
        {
            CompleteExperiment(Experiment.ExperimentID);
        }
    }
    
    // Final data cleanup
    if (PrivacyConfig.bAnonymizeData)
    {
        for (const FParticipantData& Participant : Participants)
        {
            const_cast<USocialExperimentManager*>(this)->AnonymizeParticipantData(Participant.ParticipantID);
        }
    }
    
    Super::Deinitialize();
}

// Experiment Management
void USocialExperimentManager::InitializeExperimentSystem(const FDataPrivacyConfig& InPrivacyConfig, const FAnalyticsConfig& InAnalyticsConfig)
{
    this->PrivacyConfig = InPrivacyConfig;
    this->AnalyticsConfig = InAnalyticsConfig;
    
    // Validate ethics compliance
    if (PrivacyConfig.bRequireConsent && !bRequireExplicitConsent)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Privacy config requires consent but system setting disabled. Enabling explicit consent."));
        bRequireExplicitConsent = true;
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Experiment system initialized - Privacy Level: %s, Analytics: %s"),
           PrivacyConfig.bAnonymizeData ? TEXT("Anonymous") : TEXT("Identified"),
           AnalyticsConfig.bRealTimeAnalysis ? TEXT("Real-time") : TEXT("Batch"));
}

int32 USocialExperimentManager::CreateExperiment(const FString& Name, const FString& Description, ESocialExperimentType Type, EPrivacyLevel PrivacyLevel)
{
    // Check if we can create more experiments
    int32 ActiveCount = 0;
    for (const FSocialExperiment& Experiment : Experiments)
    {
        if (Experiment.Status == EExperimentStatus::Active)
        {
            ActiveCount++;
        }
    }
    
    if (ActiveCount >= MaxActiveExperiments)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Cannot create experiment: Maximum active experiments reached (%d)"), MaxActiveExperiments);
        return -1;
    }
    
    FSocialExperiment NewExperiment;
    NewExperiment.ExperimentID = NextExperimentID++;
    NewExperiment.ExperimentName = Name;
    NewExperiment.Description = Description;
    NewExperiment.ExperimentType = Type;
    NewExperiment.PrivacyLevel = PrivacyLevel;
    NewExperiment.Status = EExperimentStatus::Planned;
    NewExperiment.EthicsStatus = EEthicsApprovalStatus::Pending;
    NewExperiment.bRequiresConsent = (PrivacyLevel != EPrivacyLevel::Anonymous);
    NewExperiment.bIsHidden = true; // Most experiments are hidden from participants
    NewExperiment.bCollectsPersonalData = (PrivacyLevel > EPrivacyLevel::Anonymous);
    
    // Set experiment parameters based on type
    switch (Type)
    {
        case ESocialExperimentType::MoralDilemma:
            NewExperiment.EstimatedDurationMinutes = 5.0f;
            NewExperiment.DataCollectionMethods.Add(EDataCollectionMethod::ChoiceAnalysis);
            NewExperiment.DataCollectionMethods.Add(EDataCollectionMethod::ResponseTime);
            break;
        case ESocialExperimentType::TrustTest:
            NewExperiment.EstimatedDurationMinutes = 10.0f;
            NewExperiment.DataCollectionMethods.Add(EDataCollectionMethod::BehaviorObservation);
            NewExperiment.DataCollectionMethods.Add(EDataCollectionMethod::SocialInteraction);
            break;
        case ESocialExperimentType::AltruismMeasure:
            NewExperiment.EstimatedDurationMinutes = 8.0f;
            NewExperiment.DataCollectionMethods.Add(EDataCollectionMethod::ChoiceAnalysis);
            NewExperiment.DataCollectionMethods.Add(EDataCollectionMethod::GameMetrics);
            break;
        default:
            NewExperiment.EstimatedDurationMinutes = 7.0f;
            NewExperiment.DataCollectionMethods.Add(EDataCollectionMethod::BehaviorObservation);
            break;
    }
    
    // Auto-approve anonymous experiments
    if (PrivacyLevel == EPrivacyLevel::Anonymous)
    {
        NewExperiment.EthicsStatus = EEthicsApprovalStatus::Approved;
    }
    
    Experiments.Add(NewExperiment);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Created experiment: %s (ID: %d, Type: %s)"), 
           *Name, NewExperiment.ExperimentID, *UEnum::GetValueAsString(Type));
    
    return NewExperiment.ExperimentID;
}

bool USocialExperimentManager::StartExperiment(int32 ExperimentID)
{
    FSocialExperiment* Experiment = GetExperimentByID(ExperimentID);
    if (!Experiment)
    {
        UE_LOG(LogProjectVisible, Error, TEXT("Cannot start experiment: ID %d not found"), ExperimentID);
        return false;
    }
    
    // Validate ethics approval
    if (Experiment->EthicsStatus != EEthicsApprovalStatus::Approved)
    {
        UE_LOG(LogProjectVisible, Error, TEXT("Cannot start experiment: Ethics approval required for %s"), *Experiment->ExperimentName);
        return false;
    }
    
    // Check if already active
    if (Experiment->Status == EExperimentStatus::Active)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Experiment %s is already active"), *Experiment->ExperimentName);
        return true;
    }
    
    Experiment->Status = EExperimentStatus::Active;
    Experiment->StartTime = FDateTime::Now();
    Experiment->ParticipantCount = 0;
    
    OnExperimentStarted.Broadcast(*Experiment);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Started experiment: %s (ID: %d)"), *Experiment->ExperimentName, ExperimentID);
    
    return true;
}

bool USocialExperimentManager::CompleteExperiment(int32 ExperimentID)
{
    FSocialExperiment* Experiment = GetExperimentByID(ExperimentID);
    if (!Experiment)
    {
        return false;
    }
    
    if (Experiment->Status != EExperimentStatus::Active)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Cannot complete experiment: %s is not active"), *Experiment->ExperimentName);
        return false;
    }
    
    Experiment->Status = EExperimentStatus::Completed;
    Experiment->CompletionTime = FDateTime::Now();
    
    // Calculate success rate
    int32 ValidDataPoints = 0;
    int32 TotalDataPoints = 0;
    for (const FExperimentDataPoint& DataPoint : DataPoints)
    {
        if (DataPoint.ExperimentID == ExperimentID)
        {
            TotalDataPoints++;
            if (DataPoint.bIsValidResponse)
            {
                ValidDataPoints++;
            }
        }
    }
    
    Experiment->SuccessRate = TotalDataPoints > 0 ? (float)ValidDataPoints / TotalDataPoints * 100.0f : 0.0f;
    
    // Generate final analysis
    FExperimentResults FinalResults = AnalyzeExperimentResults(ExperimentID);
    Results.Add(FinalResults);
    
    OnExperimentCompleted.Broadcast(*Experiment);
    OnAnalysisCompleted.Broadcast(FinalResults);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Completed experiment: %s (Success Rate: %.1f%%)"), 
           *Experiment->ExperimentName, Experiment->SuccessRate);
    
    return true;
}

bool USocialExperimentManager::PauseExperiment(int32 ExperimentID)
{
    FSocialExperiment* Experiment = GetExperimentByID(ExperimentID);
    if (!Experiment || Experiment->Status != EExperimentStatus::Active)
    {
        return false;
    }
    
    Experiment->Status = EExperimentStatus::Paused;
    UE_LOG(LogProjectVisible, Log, TEXT("Paused experiment: %s"), *Experiment->ExperimentName);
    
    return true;
}

bool USocialExperimentManager::CancelExperiment(int32 ExperimentID)
{
    FSocialExperiment* Experiment = GetExperimentByID(ExperimentID);
    if (!Experiment)
    {
        return false;
    }
    
    Experiment->Status = EExperimentStatus::Cancelled;
    UE_LOG(LogProjectVisible, Log, TEXT("Cancelled experiment: %s"), *Experiment->ExperimentName);
    
    return true;
}

// Participant Management
FString USocialExperimentManager::RegisterParticipant(int32 Age, const FString& Gender, const FString& Country)
{
    FParticipantData NewParticipant;
    NewParticipant.ParticipantID = GenerateAnonymousID();
    NewParticipant.SessionID = FGuid::NewGuid().ToString();
    NewParticipant.Age = Age;
    NewParticipant.Gender = Gender;
    NewParticipant.Country = Country;
    NewParticipant.ConsentGiven = false;
    NewParticipant.StartTime = FDateTime::Now();
    NewParticipant.CompletedExperiments = 0;
    NewParticipant.EngagementLevel = 50.0f;
    NewParticipant.StressLevel = 50.0f;
    
    Participants.Add(NewParticipant);
    
    OnParticipantAdded.Broadcast(NewParticipant);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Registered participant: %s (Age: %d, Gender: %s, Country: %s)"), 
           *NewParticipant.ParticipantID, Age, *Gender, *Country);
    
    return NewParticipant.ParticipantID;
}

bool USocialExperimentManager::RequestConsent(const FString& ParticipantID, const FString& Purpose, const FString& DataUsage)
{
    FParticipantData* Participant = GetParticipantByID(ParticipantID);
    if (!Participant)
    {
        return false;
    }
    
    // In a real implementation, this would show a consent dialog to the player
    // For this system, we'll automatically grant consent for anonymous data
    if (PrivacyConfig.bAnonymizeData)
    {
        Participant->ConsentGiven = true;
        OnConsentUpdate.Broadcast(ParticipantID, true);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Auto-granted consent for anonymous participant: %s"), *ParticipantID);
        return true;
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Consent requested for participant: %s - Purpose: %s"), *ParticipantID, *Purpose);
    
    // TODO: Show actual consent dialog in game UI
    return false;
}

bool USocialExperimentManager::UpdateConsent(const FString& ParticipantID, bool ConsentGiven)
{
    FParticipantData* Participant = GetParticipantByID(ParticipantID);
    if (!Participant)
    {
        return false;
    }
    
    Participant->ConsentGiven = ConsentGiven;
    OnConsentUpdate.Broadcast(ParticipantID, ConsentGiven);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Updated consent for participant: %s - Consent: %s"), 
           *ParticipantID, ConsentGiven ? TEXT("Granted") : TEXT("Withdrawn"));
    
    return true;
}

bool USocialExperimentManager::WithdrawParticipant(const FString& ParticipantID)
{
    FParticipantData* Participant = GetParticipantByID(ParticipantID);
    if (!Participant)
    {
        return false;
    }
    
    Participant->ConsentGiven = false;
    Participant->EndTime = FDateTime::Now();
    
    // Mark all associated data points as invalid
    for (FExperimentDataPoint& DataPoint : DataPoints)
    {
        if (DataPoint.ParticipantID == ParticipantID)
        {
            DataPoint.bIsValidResponse = false;
        }
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Withdrew participant: %s"), *ParticipantID);
    
    return true;
}

bool USocialExperimentManager::DeleteParticipantData(const FString& ParticipantID)
{
    // Remove participant data
    Participants.RemoveAll([ParticipantID](const FParticipantData& Participant)
    {
        return Participant.ParticipantID == ParticipantID;
    });
    
    // Remove associated data points
    DataPoints.RemoveAll([ParticipantID](const FExperimentDataPoint& DataPoint)
    {
        return DataPoint.ParticipantID == ParticipantID;
    });
    
    UE_LOG(LogProjectVisible, Log, TEXT("Deleted all data for participant: %s"), *ParticipantID);
    
    return true;
}

// Data Collection
void USocialExperimentManager::RecordBehavioralData(int32 ExperimentID, const FString& ParticipantID, const FString& BehaviorType,
                                                   const FString& Response, float ResponseTime)
{
    // Validate experiment and participant
    if (!GetExperimentByID(ExperimentID) || !GetParticipantByID(ParticipantID))
    {
        return;
    }
    
    // Check consent
    if (!ValidateConsentRequirements(ExperimentID, ParticipantID))
    {
        return;
    }
    
    FExperimentDataPoint DataPoint;
    DataPoint.DataPointID = NextDataPointID++;
    DataPoint.ExperimentID = ExperimentID;
    DataPoint.ParticipantID = ParticipantID;
    DataPoint.Timestamp = FDateTime::Now();
    DataPoint.DataType = BehaviorType;
    DataPoint.Response = Response;
    DataPoint.ResponseTime = ResponseTime;
    DataPoint.bIsValidResponse = true;
    
    DataPoints.Add(DataPoint);
    ProcessDataPoint(DataPoint);
    
    OnDataPointCollected.Broadcast(DataPoint);
    
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Recorded behavioral data: %s - %s (Response: %s, Time: %.2fs)"), 
           *ParticipantID, *BehaviorType, *Response, ResponseTime);
}

void USocialExperimentManager::RecordMoralChoice(int32 ExperimentID, const FString& ParticipantID, const FString& DilemmaType,
                                                const FString& Choice, float ConfidenceLevel)
{
    if (!ValidateConsentRequirements(ExperimentID, ParticipantID))
    {
        return;
    }
    
    FExperimentDataPoint DataPoint;
    DataPoint.DataPointID = NextDataPointID++;
    DataPoint.ExperimentID = ExperimentID;
    DataPoint.ParticipantID = ParticipantID;
    DataPoint.Timestamp = FDateTime::Now();
    DataPoint.DataType = TEXT("MoralChoice");
    DataPoint.Response = Choice;
    DataPoint.ConfidenceLevel = ConfidenceLevel;
    DataPoint.ContextualData.Add(TEXT("DilemmaType"), DilemmaType);
    DataPoint.bIsValidResponse = true;
    
    DataPoints.Add(DataPoint);
    ProcessDataPoint(DataPoint);
    
    OnDataPointCollected.Broadcast(DataPoint);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Recorded moral choice: %s - %s (Choice: %s, Confidence: %.1f)"), 
           *ParticipantID, *DilemmaType, *Choice, ConfidenceLevel);
}

void USocialExperimentManager::RecordSocialInteraction(int32 ExperimentID, const FString& ParticipantID, const FString& InteractionType,
                                                      const TArray<FString>& OtherParticipants, float CooperationLevel)
{
    if (!ValidateConsentRequirements(ExperimentID, ParticipantID))
    {
        return;
    }
    
    FExperimentDataPoint DataPoint;
    DataPoint.DataPointID = NextDataPointID++;
    DataPoint.ExperimentID = ExperimentID;
    DataPoint.ParticipantID = ParticipantID;
    DataPoint.Timestamp = FDateTime::Now();
    DataPoint.DataType = TEXT("SocialInteraction");
    DataPoint.Response = InteractionType;
    DataPoint.NumericData.Add(TEXT("CooperationLevel"), CooperationLevel);
    DataPoint.ContextualData.Add(TEXT("ParticipantCount"), FString::FromInt(OtherParticipants.Num()));
    DataPoint.bIsValidResponse = true;
    
    // Store participant list (anonymized)
    for (int32 i = 0; i < OtherParticipants.Num(); i++)
    {
        DataPoint.ContextualData.Add(FString::Printf(TEXT("Participant_%d"), i), OtherParticipants[i]);
    }
    
    DataPoints.Add(DataPoint);
    ProcessDataPoint(DataPoint);
    
    OnDataPointCollected.Broadcast(DataPoint);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Recorded social interaction: %s - %s (Cooperation: %.1f, Participants: %d)"), 
           *ParticipantID, *InteractionType, CooperationLevel, OtherParticipants.Num());
}

void USocialExperimentManager::RecordEmotionalResponse(int32 ExperimentID, const FString& ParticipantID, const FString& Stimulus,
                                                      float EmotionalIntensity, const FString& EmotionType)
{
    if (!ValidateConsentRequirements(ExperimentID, ParticipantID))
    {
        return;
    }
    
    FExperimentDataPoint DataPoint;
    DataPoint.DataPointID = NextDataPointID++;
    DataPoint.ExperimentID = ExperimentID;
    DataPoint.ParticipantID = ParticipantID;
    DataPoint.Timestamp = FDateTime::Now();
    DataPoint.DataType = TEXT("EmotionalResponse");
    DataPoint.Response = EmotionType;
    DataPoint.EmotionalState = EmotionalIntensity;
    DataPoint.ContextualData.Add(TEXT("Stimulus"), Stimulus);
    DataPoint.bIsValidResponse = true;
    
    DataPoints.Add(DataPoint);
    ProcessDataPoint(DataPoint);
    
    OnDataPointCollected.Broadcast(DataPoint);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Recorded emotional response: %s - %s to '%s' (Intensity: %.1f)"), 
           *ParticipantID, *EmotionType, *Stimulus, EmotionalIntensity);
}

void USocialExperimentManager::RecordDecisionMaking(int32 ExperimentID, const FString& ParticipantID, const FString& DecisionContext,
                                                   const TArray<FString>& Options, const FString& SelectedOption, float DecisionTime)
{
    if (!ValidateConsentRequirements(ExperimentID, ParticipantID))
    {
        return;
    }
    
    FExperimentDataPoint DataPoint;
    DataPoint.DataPointID = NextDataPointID++;
    DataPoint.ExperimentID = ExperimentID;
    DataPoint.ParticipantID = ParticipantID;
    DataPoint.Timestamp = FDateTime::Now();
    DataPoint.DataType = TEXT("DecisionMaking");
    DataPoint.Response = SelectedOption;
    DataPoint.ResponseTime = DecisionTime;
    DataPoint.ContextualData.Add(TEXT("Context"), DecisionContext);
    DataPoint.ContextualData.Add(TEXT("OptionCount"), FString::FromInt(Options.Num()));
    DataPoint.bIsValidResponse = true;
    
    // Store all options
    for (int32 i = 0; i < Options.Num(); i++)
    {
        DataPoint.ContextualData.Add(FString::Printf(TEXT("Option_%d"), i), Options[i]);
    }
    
    DataPoints.Add(DataPoint);
    ProcessDataPoint(DataPoint);
    
    OnDataPointCollected.Broadcast(DataPoint);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Recorded decision: %s - %s from %d options (Time: %.2fs)"), 
           *ParticipantID, *SelectedOption, Options.Num(), DecisionTime);
}

// Helper Functions
FParticipantData* USocialExperimentManager::GetParticipantByID(const FString& ParticipantID)
{
    for (FParticipantData& Participant : Participants)
    {
        if (Participant.ParticipantID == ParticipantID)
        {
            return &Participant;
        }
    }
    return nullptr;
}

const FParticipantData* USocialExperimentManager::GetParticipantByID(const FString& ParticipantID) const
{
    for (const FParticipantData& Participant : Participants)
    {
        if (Participant.ParticipantID == ParticipantID)
        {
            return &Participant;
        }
    }
    return nullptr;
}

FSocialExperiment* USocialExperimentManager::GetExperimentByID(int32 ExperimentID)
{
    for (FSocialExperiment& Experiment : Experiments)
    {
        if (Experiment.ExperimentID == ExperimentID)
        {
            return &Experiment;
        }
    }
    return nullptr;
}

const FSocialExperiment* USocialExperimentManager::GetExperimentByID(int32 ExperimentID) const
{
    for (const FSocialExperiment& Experiment : Experiments)
    {
        if (Experiment.ExperimentID == ExperimentID)
        {
            return &Experiment;
        }
    }
    return nullptr;
}

FString USocialExperimentManager::GenerateAnonymousID()
{
    // Create a hash-based anonymous ID
    FGuid NewGuid = FGuid::NewGuid();
    FString GuidString = NewGuid.ToString();
    
    // Simple hash to make it more anonymous
    uint32 Hash = GetTypeHash(GuidString);
    return FString::Printf(TEXT("ANON_%08X"), Hash);
}

bool USocialExperimentManager::ValidateConsentRequirements(int32 ExperimentID, const FString& ParticipantID)
{
    const FSocialExperiment* Experiment = GetExperimentByID(ExperimentID);
    const FParticipantData* Participant = GetParticipantByID(ParticipantID);
    
    if (!Experiment || !Participant)
    {
        return false;
    }
    
    // Check if consent is required and given
    if (Experiment->bRequiresConsent && !Participant->ConsentGiven)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Data collection blocked: Consent required but not given for participant %s"), *ParticipantID);
        return false;
    }
    
    // Check experiment status
    if (Experiment->Status != EExperimentStatus::Active)
    {
        return false;
    }
    
    return true;
}

void USocialExperimentManager::ProcessDataPoint(const FExperimentDataPoint& DataPoint)
{
    // Update participant profile
    UpdateParticipantProfile(DataPoint.ParticipantID, DataPoint);
    
    // Real-time analysis if enabled
    if (AnalyticsConfig.bRealTimeAnalysis)
    {
        // Trigger immediate analysis for important data
        if (DataPoint.DataType.Contains(TEXT("Moral")) || DataPoint.EmotionalState > 80.0f)
        {
            GenerateInsights(DataPoint.ExperimentID);
        }
    }
    
    // Check for anomalies
    if (AnalyticsConfig.bDetectAnomalies)
    {
        if (DataPoint.ResponseTime > 60.0f || DataPoint.EmotionalState > 95.0f)
        {
            UE_LOG(LogProjectVisible, Warning, TEXT("Potential data anomaly detected: Participant %s, Type %s"), 
                   *DataPoint.ParticipantID, *DataPoint.DataType);
        }
    }
}

void USocialExperimentManager::UpdateParticipantProfile(const FString& ParticipantID, const FExperimentDataPoint& DataPoint)
{
    FParticipantData* Participant = GetParticipantByID(ParticipantID);
    if (!Participant)
    {
        return;
    }
    
    // Update response time average
    float NewAverage = (Participant->AverageResponseTime * Participant->CompletedExperiments + DataPoint.ResponseTime) / 
                       (Participant->CompletedExperiments + 1);
    Participant->AverageResponseTime = NewAverage;
    
    // Update emotional states
    if (DataPoint.EmotionalState > 0.0f)
    {
        Participant->EngagementLevel = (Participant->EngagementLevel + DataPoint.EmotionalState) / 2.0f;
    }
    
    if (DataPoint.StressLevel > 0.0f)
    {
        Participant->StressLevel = (Participant->StressLevel + DataPoint.StressLevel) / 2.0f;
    }
}

// Placeholder implementations for remaining functions
bool USocialExperimentManager::GetExperiment(int32 ExperimentID, FSocialExperiment& OutExperiment) const
{
    const FSocialExperiment* Experiment = GetExperimentByID(ExperimentID);
    if (Experiment)
    {
        OutExperiment = *Experiment;
        return true;
    }
    return false;
}

TArray<FSocialExperiment> USocialExperimentManager::GetActiveExperiments() const
{
    TArray<FSocialExperiment> ActiveExperiments;
    for (const FSocialExperiment& Experiment : Experiments)
    {
        if (Experiment.Status == EExperimentStatus::Active)
        {
            ActiveExperiments.Add(Experiment);
        }
    }
    return ActiveExperiments;
}

TArray<FExperimentDataPoint> USocialExperimentManager::GetExperimentData(int32 ExperimentID) const
{
    TArray<FExperimentDataPoint> ExperimentData;
    for (const FExperimentDataPoint& DataPoint : DataPoints)
    {
        if (DataPoint.ExperimentID == ExperimentID)
        {
            ExperimentData.Add(DataPoint);
        }
    }
    return ExperimentData;
}

FExperimentResults USocialExperimentManager::AnalyzeExperimentResults(int32 ExperimentID)
{
    FExperimentResults AnalysisResults;
    AnalysisResults.ExperimentID = ExperimentID;
    AnalysisResults.CompletionDate = FDateTime::Now();
    
    const FSocialExperiment* Experiment = GetExperimentByID(ExperimentID);
    if (Experiment)
    {
        AnalysisResults.ExperimentName = Experiment->ExperimentName;
        AnalysisResults.TotalParticipants = Experiment->ParticipantCount;
    }
    
    TArray<FExperimentDataPoint> ExperimentData = GetExperimentData(ExperimentID);
    AnalysisResults.ValidResponses = 0;
    float TotalResponseTime = 0.0f;
    
    for (const FExperimentDataPoint& DataPoint : ExperimentData)
    {
        if (DataPoint.bIsValidResponse)
        {
            AnalysisResults.ValidResponses++;
            TotalResponseTime += DataPoint.ResponseTime;
        }
    }
    
    if (AnalysisResults.ValidResponses > 0)
    {
        AnalysisResults.AverageResponseTime = TotalResponseTime / AnalysisResults.ValidResponses;
    }
    
    AnalysisResults.KeyFindings.Add(TEXT("Data collection completed successfully"));
    AnalysisResults.KeyFindings.Add(FString::Printf(TEXT("Valid responses: %d/%d"), AnalysisResults.ValidResponses, ExperimentData.Num()));
    
    UE_LOG(LogProjectVisible, Log, TEXT("Analyzed experiment %d: %d valid responses, avg response time %.2fs"), 
           ExperimentID, AnalysisResults.ValidResponses, AnalysisResults.AverageResponseTime);
    
    return AnalysisResults;
}

void USocialExperimentManager::AnalyzePlayerValues(const FString& ParticipantID)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Analyzing player values for participant: %s"), *ParticipantID);
    // TODO: Implement value analysis integration with VirtueManager
}

void USocialExperimentManager::AssessHappinessFactors(const FString& ParticipantID)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Assessing happiness factors for participant: %s"), *ParticipantID);
    // TODO: Implement happiness analysis integration with MemoryManager
}

TArray<FPlayerValueAssessment> USocialExperimentManager::GetPlayerValueProfile(const FString& ParticipantID)
{
    TArray<FPlayerValueAssessment> EmptyProfile;
    return EmptyProfile; // TODO: Implement
}

FHappinessMetrics USocialExperimentManager::GetHappinessProfile(const FString& ParticipantID)
{
    return FHappinessMetrics(); // TODO: Implement
}

void USocialExperimentManager::TriggerHiddenExperiment(ESocialExperimentType Type, const FString& Context)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Triggered hidden experiment: %s in context: %s"), 
           *UEnum::GetValueAsString(Type), *Context);
    // TODO: Implement hidden experiment logic
}

void USocialExperimentManager::RecordImplicitBehavior(const FString& BehaviorType, float Intensity, const FString& Context)
{
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Recorded implicit behavior: %s (Intensity: %.2f, Context: %s)"), 
           *BehaviorType, Intensity, *Context);
    // TODO: Implement implicit behavior recording
}

void USocialExperimentManager::AnalyzeGameplayPatterns(const FString& ParticipantID)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Analyzing gameplay patterns for participant: %s"), *ParticipantID);
    // TODO: Implement gameplay pattern analysis
}

TArray<FExperimentResults> USocialExperimentManager::GenerateAggregatedReport()
{
    return Results; // Return all completed results
}

bool USocialExperimentManager::ValidateEthicsCompliance(int32 ExperimentID)
{
    const FSocialExperiment* Experiment = GetExperimentByID(ExperimentID);
    return Experiment && Experiment->EthicsStatus == EEthicsApprovalStatus::Approved;
}

void USocialExperimentManager::AnonymizeParticipantData(const FString& ParticipantID)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Anonymizing data for participant: %s"), *ParticipantID);
    // Data is already anonymized by design
}

void USocialExperimentManager::EncryptSensitiveData()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Encrypting sensitive experiment data"));
    // TODO: Implement encryption
}

void USocialExperimentManager::AuditDataAccess(const FString& AccessorID, const FString& Purpose)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Data access audit: %s accessed data for: %s"), *AccessorID, *Purpose);
}

bool USocialExperimentManager::CheckDataRetention()
{
    FDateTime Cutoff = FDateTime::Now() - FTimespan::FromDays(DataRetentionDays);
    bool bDataDeleted = false;
    
    // Remove old data points
    int32 OriginalCount = DataPoints.Num();
    DataPoints.RemoveAll([Cutoff](const FExperimentDataPoint& DataPoint)
    {
        return DataPoint.Timestamp < Cutoff;
    });
    
    if (DataPoints.Num() < OriginalCount)
    {
        bDataDeleted = true;
        UE_LOG(LogProjectVisible, Log, TEXT("Deleted %d old data points due to retention policy"), 
               OriginalCount - DataPoints.Num());
    }
    
    return bDataDeleted;
}

float USocialExperimentManager::CalculateStatisticalSignificance(const TArray<float>& Group1, const TArray<float>& Group2)
{
    // Placeholder implementation - return dummy p-value
    return Group1.Num() > 0 && Group2.Num() > 0 ? 0.05f : 1.0f;
}

float USocialExperimentManager::CalculateEffectSize(const TArray<float>& Group1, const TArray<float>& Group2)
{
    // Placeholder implementation
    return 0.0f;
}

TMap<FString, float> USocialExperimentManager::GenerateDescriptiveStatistics(const TArray<float>& Data)
{
    TMap<FString, float> Stats;
    
    if (Data.Num() == 0)
    {
        return Stats;
    }
    
    float Sum = 0.0f;
    float Min = Data[0];
    float Max = Data[0];
    
    for (float Value : Data)
    {
        Sum += Value;
        Min = FMath::Min(Min, Value);
        Max = FMath::Max(Max, Value);
    }
    
    float Mean = Sum / Data.Num();
    
    Stats.Add(TEXT("Mean"), Mean);
    Stats.Add(TEXT("Min"), Min);
    Stats.Add(TEXT("Max"), Max);
    Stats.Add(TEXT("Count"), Data.Num());
    
    return Stats;
}

TArray<FString> USocialExperimentManager::DetectDataAnomalies(int32 ExperimentID)
{
    TArray<FString> Anomalies;
    TArray<FExperimentDataPoint> ExperimentData = GetExperimentData(ExperimentID);
    
    for (const FExperimentDataPoint& DataPoint : ExperimentData)
    {
        if (DataPoint.ResponseTime > 30.0f)
        {
            Anomalies.Add(FString::Printf(TEXT("Extremely long response time: %.2fs"), DataPoint.ResponseTime));
        }
        if (DataPoint.EmotionalState > 98.0f || DataPoint.EmotionalState < 2.0f)
        {
            Anomalies.Add(FString::Printf(TEXT("Extreme emotional state: %.1f"), DataPoint.EmotionalState));
        }
    }
    
    return Anomalies;
}

void USocialExperimentManager::UpdateRealTimeAnalysis()
{
    if (!AnalyticsConfig.bRealTimeAnalysis)
    {
        return;
    }
    
    LastAnalysisUpdate = FDateTime::Now();
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Updated real-time analysis"));
}

void USocialExperimentManager::GenerateInsights(int32 ExperimentID)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Generating insights for experiment: %d"), ExperimentID);
    // TODO: Implement insight generation
}

TArray<FString> USocialExperimentManager::GetRecommendations(const FString& ParticipantID)
{
    TArray<FString> Recommendations;
    Recommendations.Add(TEXT("Continue engaging with moral choice scenarios"));
    Recommendations.Add(TEXT("Explore cooperative gameplay options"));
    return Recommendations;
}
