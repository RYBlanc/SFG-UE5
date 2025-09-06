// Project Visible - Character Manager Implementation (Simplified)
// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterManager.h"
#include "Gemini_C.h"
#include "StoryManager.h"
#include "VirtueManager.h"
#include "MemoryManager.h"
#include "SocialExperimentManager.h"
#include "LevelDesignManager.h"
#include "ProjectVisibleUIManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void UCharacterManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize settings
    bEnablePsychologicalAnalysis = true;
    bEnableClassBias = true;
    bEnableEmotionalMemory = true;
    DefaultTrustLevel = 50.0f;
    TrustDecayRate = 1.0f;
    MaxDialogueTurns = 15;
    InformationRevealThreshold = 70.0f;
    NextSessionID = 1;
    
    InitializeCharacterSystem();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Character Manager initialized"));
}

void UCharacterManager::Deinitialize()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Character Manager shutting down"));
    SaveCharacterData();
    Super::Deinitialize();
}

void UCharacterManager::InitializeCharacterSystem()
{
    InitializeDefaultCharacters();
    InitializeClassInteractionMatrix();
    SetupCharacterAI();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Character system initialized with %d characters"), RegisteredCharacters.Num());
}

FString UCharacterManager::RegisterCharacter(const FCharacterDefinition& Character)
{
    FString CharacterID = Character.CharacterID;
    
    if (CharacterID.IsEmpty() || GetCharacterByID(CharacterID))
    {
        CharacterID = FString::Printf(TEXT("CHAR_%d"), RegisteredCharacters.Num() + 1);
    }
    
    FCharacterDefinition NewCharacter = Character;
    NewCharacter.CharacterID = CharacterID;
    
    RegisteredCharacters.Add(NewCharacter);
    
    LastInteractionTimes.Add(CharacterID, FDateTime::Now());
    InteractionCounts.Add(CharacterID, 0);
    
    OnCharacterRegistered.Broadcast(NewCharacter);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Registered character: %s (%s)"), 
           *NewCharacter.CharacterName, *CharacterID);
    
    return CharacterID;
}

void UCharacterManager::UnregisterCharacter(const FString& CharacterID)
{
    RegisteredCharacters.RemoveAll([CharacterID](const FCharacterDefinition& Character) {
        return Character.CharacterID == CharacterID;
    });
    
    LastInteractionTimes.Remove(CharacterID);
    InteractionCounts.Remove(CharacterID);
    CharacterRelationships.Remove(CharacterID);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Unregistered character: %s"), *CharacterID);
}

bool UCharacterManager::GetCharacter(const FString& CharacterID, FCharacterDefinition& OutCharacter) const
{
    const FCharacterDefinition* Character = GetCharacterByID(CharacterID);
    if (Character)
    {
        OutCharacter = *Character;
        return true;
    }
    return false;
}

TArray<FCharacterDefinition> UCharacterManager::GetAllCharacters() const
{
    return RegisteredCharacters;
}

TArray<FCharacterDefinition> UCharacterManager::GetCharactersByLocation(EStoryLocation Location) const
{
    TArray<FCharacterDefinition> LocationCharacters;
    for (const FCharacterDefinition& Character : RegisteredCharacters)
    {
        if (Character.CurrentLocation == Location)
        {
            LocationCharacters.Add(Character);
        }
    }
    return LocationCharacters;
}

TArray<FCharacterDefinition> UCharacterManager::GetCharactersByClass(ESocialClass SocialClass) const
{
    TArray<FCharacterDefinition> ClassCharacters;
    for (const FCharacterDefinition& Character : RegisteredCharacters)
    {
        if (Character.SocialClass == SocialClass)
        {
            ClassCharacters.Add(Character);
        }
    }
    return ClassCharacters;
}

TArray<FCharacterDefinition> UCharacterManager::GetCharactersByType(ECharacterType CharacterType) const
{
    TArray<FCharacterDefinition> TypeCharacters;
    for (const FCharacterDefinition& Character : RegisteredCharacters)
    {
        if (Character.CharacterType == CharacterType)
        {
            TypeCharacters.Add(Character);
        }
    }
    return TypeCharacters;
}

void UCharacterManager::MoveCharacterToLocation(const FString& CharacterID, EStoryLocation NewLocation)
{
    if (FCharacterDefinition* Character = GetCharacterByID(CharacterID))
    {
        if (Character->bCanTravel)
        {
            Character->CurrentLocation = NewLocation;
            UE_LOG(LogProjectVisible, Log, TEXT("Moved %s to new location"), *Character->CharacterName);
        }
    }
}

FString UCharacterManager::StartDialogueSession(const FString& CharacterID, const FString& PlayerID)
{
    FCharacterDefinition* Character = GetCharacterByID(CharacterID);
    if (!Character)
    {
        return TEXT("");
    }
    
    if (IsDialogueActive(CharacterID))
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Character %s is already in dialogue"), *CharacterID);
        return TEXT("");
    }
    
    FDialogueSession NewSession;
    NewSession.SessionID = FString::Printf(TEXT("SESSION_%d"), NextSessionID++);
    NewSession.CharacterID = CharacterID;
    NewSession.PlayerID = PlayerID;
    NewSession.StartTime = FDateTime::Now();
    NewSession.bIsActive = true;
    NewSession.MaxTurns = MaxDialogueTurns;
    
    ActiveDialogueSessions.Add(NewSession);
    
    InteractionCounts[CharacterID]++;
    LastInteractionTimes[CharacterID] = FDateTime::Now();
    Character->InteractionCount++;
    
    OnDialogueStarted.Broadcast(CharacterID, NewSession);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Started dialogue session %s with %s"), 
           *NewSession.SessionID, *Character->CharacterName);
    
    return NewSession.SessionID;
}

FCharacterInteractionResult UCharacterManager::ProcessDialogueApproach(const FString& SessionID, EDialogueApproach Approach, 
                                                                      const FString& DialogueText)
{
    FCharacterInteractionResult Result;
    
    FDialogueSession* Session = GetDialogueSessionByID(SessionID);
    if (!Session || !Session->bIsActive)
    {
        return Result;
    }
    
    FCharacterDefinition* Character = GetCharacterByID(Session->CharacterID);
    if (!Character)
    {
        return Result;
    }
    
    Result.Approach = Approach;
    Result.bWasSuccessful = (FMath::RandRange(0.0f, 1.0f) > 0.5f);
    Result.Effectiveness = FMath::RandRange(0.3f, 0.9f);
    Result.TrustChange = Result.bWasSuccessful ? 5.0f : -3.0f;
    Result.InformationValue = Result.bWasSuccessful ? 25.0f : 5.0f;
    Result.ResponseText = FString::Printf(TEXT("%s responded to your %s approach"), 
                                         *Character->CharacterName, *UEnum::GetValueAsString(Approach));
    
    // Update character state
    UpdateCharacterTrust(Character->CharacterID, Result.TrustChange);
    
    Session->CurrentTurn++;
    Session->ExchangedDialogues.Add(DialogueText);
    Session->ApproachesUsed.Add(Approach);
    Session->InformationGained += Result.bWasSuccessful ? 1 : 0;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Processed dialogue approach %s with %s - Success: %s"), 
           *UEnum::GetValueAsString(Approach), *Character->CharacterName, 
           Result.bWasSuccessful ? TEXT("Yes") : TEXT("No"));
    
    return Result;
}

void UCharacterManager::EndDialogueSession(const FString& SessionID)
{
    FDialogueSession* Session = GetDialogueSessionByID(SessionID);
    if (Session)
    {
        Session->bIsActive = false;
        Session->EndTime = FDateTime::Now();
        
        FCharacterDefinition* Character = GetCharacterByID(Session->CharacterID);
        if (Character)
        {
            FCharacterInteractionResult FinalResult;
            FinalResult.InformationValue = Session->InformationGained * 10.0f;
            
            OnDialogueEnded.Broadcast(Session->CharacterID, FinalResult);
            
            UE_LOG(LogProjectVisible, Log, TEXT("Ended dialogue session %s with %s - Info: %d"), 
                   *SessionID, *Character->CharacterName, Session->InformationGained);
        }
        
        ActiveDialogueSessions.RemoveAll([SessionID](const FDialogueSession& DialogueSession) {
            return DialogueSession.SessionID == SessionID;
        });
    }
}

bool UCharacterManager::IsDialogueActive(const FString& CharacterID) const
{
    for (const FDialogueSession& Session : ActiveDialogueSessions)
    {
        if (Session.CharacterID == CharacterID && Session.bIsActive)
        {
            return true;
        }
    }
    return false;
}

bool UCharacterManager::GetActiveDialogueSession(const FString& CharacterID, FDialogueSession& OutSession) const
{
    for (const FDialogueSession& Session : ActiveDialogueSessions)
    {
        if (Session.CharacterID == CharacterID && Session.bIsActive)
        {
            OutSession = Session;
            return true;
        }
    }
    return false;
}

TArray<EDialogueApproach> UCharacterManager::GetRecommendedApproaches(const FString& CharacterID) const
{
    TArray<EDialogueApproach> RecommendedApproaches;
    RecommendedApproaches.Add(EDialogueApproach::Question);
    RecommendedApproaches.Add(EDialogueApproach::Empathy);
    RecommendedApproaches.Add(EDialogueApproach::Logic);
    return RecommendedApproaches;
}

void UCharacterManager::UpdateCharacterTrust(const FString& CharacterID, float TrustChange)
{
    FCharacterDefinition* Character = GetCharacterByID(CharacterID);
    if (Character)
    {
        float OldTrust = Character->TrustLevel;
        Character->TrustLevel = FMath::Clamp(Character->TrustLevel + TrustChange, 0.0f, 100.0f);
        
        OnTrustChanged.Broadcast(CharacterID, Character->TrustLevel);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Updated %s trust: %.1f -> %.1f"), 
               *Character->CharacterName, OldTrust, Character->TrustLevel);
    }
}

void UCharacterManager::UpdateCharacterSuspicion(const FString& CharacterID, float SuspicionChange)
{
    FCharacterDefinition* Character = GetCharacterByID(CharacterID);
    if (Character)
    {
        Character->SuspicionLevel = FMath::Clamp(Character->SuspicionLevel + SuspicionChange, 0.0f, 100.0f);
        UE_LOG(LogProjectVisible, Log, TEXT("Updated %s suspicion"), *Character->CharacterName);
    }
}

void UCharacterManager::UpdateCharacterEmotion(const FString& CharacterID, ENPCEmotionalState NewState)
{
    FCharacterDefinition* Character = GetCharacterByID(CharacterID);
    if (Character)
    {
        ENPCEmotionalState OldState = Character->EmotionalState;
        Character->EmotionalState = NewState;
        
        OnCharacterStateChanged.Broadcast(CharacterID, OldState, NewState);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Updated %s emotion"), *Character->CharacterName);
    }
}

void UCharacterManager::UpdateCharacterRelationship(const FString& CharacterID, ENPCRelationship NewRelationship)
{
    FCharacterDefinition* Character = GetCharacterByID(CharacterID);
    if (Character)
    {
        Character->Relationship = NewRelationship;
        OnRelationshipChanged.Broadcast(CharacterID, NewRelationship);
        UE_LOG(LogProjectVisible, Log, TEXT("Updated %s relationship"), *Character->CharacterName);
    }
}

// Helper Functions
void UCharacterManager::InitializeDefaultCharacters()
{
    // Nicomachos
    FCharacterDefinition Nicomachos;
    Nicomachos.CharacterID = TEXT("NICOMACHOS");
    Nicomachos.CharacterName = TEXT("ニコマコス");
    Nicomachos.DisplayName = TEXT("謎の研究者");
    Nicomachos.Description = TEXT("幸福論の実験を行う神秘的な存在");
    Nicomachos.CharacterType = ECharacterType::Nicomachos;
    Nicomachos.SocialClass = ESocialClass::Operator;
    Nicomachos.CurrentLocation = EStoryLocation::Tokyo;
    Nicomachos.Relationship = ENPCRelationship::Authority;
    Nicomachos.EmotionalState = ENPCEmotionalState::Neutral;
    Nicomachos.TrustLevel = 30.0f;
    Nicomachos.KnowledgeLevel = 95.0f;
    Nicomachos.bHasSecrets = true;
    Nicomachos.ImportanceRating = 5;
    RegisterCharacter(Nicomachos);
    
    // Android A1
    FCharacterDefinition AndroidA1;
    AndroidA1.CharacterID = TEXT("ANDROID_A1");
    AndroidA1.CharacterName = TEXT("市民A1");
    AndroidA1.DisplayName = TEXT("同級アンドロイド");
    AndroidA1.Description = TEXT("同じ識閲者階級のアンドロイド");
    AndroidA1.CharacterType = ECharacterType::Android;
    AndroidA1.SocialClass = ESocialClass::Viewer;
    AndroidA1.CurrentLocation = EStoryLocation::Tokyo;
    AndroidA1.Relationship = ENPCRelationship::Acquaintance;
    AndroidA1.EmotionalState = ENPCEmotionalState::Neutral;
    AndroidA1.TrustLevel = 60.0f;
    AndroidA1.KnowledgeLevel = 55.0f;
    AndroidA1.bCanTravel = true;
    RegisterCharacter(AndroidA1);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Initialized %d default characters"), RegisteredCharacters.Num());
}

void UCharacterManager::InitializeClassInteractionMatrix()
{
    InvisibleToViewerModifier = -0.3f;
    ViewerToOperatorModifier = -0.2f;
    OperatorToInvisibleModifier = -0.4f;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Initialized class interaction matrix"));
}

void UCharacterManager::SetupCharacterAI()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Character AI systems configured"));
}

FCharacterDefinition* UCharacterManager::GetCharacterByID(const FString& CharacterID)
{
    for (FCharacterDefinition& Character : RegisteredCharacters)
    {
        if (Character.CharacterID == CharacterID)
        {
            return &Character;
        }
    }
    return nullptr;
}

const FCharacterDefinition* UCharacterManager::GetCharacterByID(const FString& CharacterID) const
{
    for (const FCharacterDefinition& Character : RegisteredCharacters)
    {
        if (Character.CharacterID == CharacterID)
        {
            return &Character;
        }
    }
    return nullptr;
}

FDialogueSession* UCharacterManager::GetDialogueSessionByID(const FString& SessionID)
{
    for (FDialogueSession& Session : ActiveDialogueSessions)
    {
        if (Session.SessionID == SessionID)
        {
            return &Session;
        }
    }
    return nullptr;
}

const FDialogueSession* UCharacterManager::GetDialogueSessionByID(const FString& SessionID) const
{
    for (const FDialogueSession& Session : ActiveDialogueSessions)
    {
        if (Session.SessionID == SessionID)
        {
            return &Session;
        }
    }
    return nullptr;
}

float UCharacterManager::GetClassInteractionModifier(ESocialClass PlayerClass, ESocialClass NPCClass) const
{
    if (PlayerClass == ESocialClass::Invisible && NPCClass == ESocialClass::Viewer)
    {
        return InvisibleToViewerModifier;
    }
    else if (PlayerClass == ESocialClass::Viewer && NPCClass == ESocialClass::Operator)
    {
        return ViewerToOperatorModifier;
    }
    else if (PlayerClass == ESocialClass::Operator && NPCClass == ESocialClass::Invisible)
    {
        return OperatorToInvisibleModifier;
    }
    else if (PlayerClass == NPCClass)
    {
        return 0.2f;
    }
    
    return 0.0f;
}

void UCharacterManager::SaveCharacterData()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Saving character data"));
}

void UCharacterManager::LoadCharacterData()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Loading character data"));
}

// System Integration
UStoryManager* UCharacterManager::GetStoryManager()
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

UVirtueManager* UCharacterManager::GetVirtueManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UVirtueManager>();
        }
    }
    return nullptr;
}

UMemoryManager* UCharacterManager::GetMemoryManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UMemoryManager>();
        }
    }
    return nullptr;
}

USocialExperimentManager* UCharacterManager::GetSocialExperimentManager()
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

ULevelDesignManager* UCharacterManager::GetLevelDesignManager()
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

UProjectVisibleUIManager* UCharacterManager::GetUIManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UProjectVisibleUIManager>();
        }
    }
    return nullptr;
}
