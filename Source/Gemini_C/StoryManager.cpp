// Project Visible - Story Manager Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "StoryManager.h"
#include "Gemini_C.h"
#include "VirtueManager.h"
#include "MemoryManager.h"
#include "SocialExperimentManager.h"
#include "ProjectVisibleUIManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void UStoryManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize settings
    bAutoSaveEnabled = true;
    AutoSaveIntervalMinutes = 5.0f;
    bAnalyticsEnabled = true;
    bSkipIntroduction = false;
    DialogueSpeedMultiplier = 1.0f;
    
    // Initialize story state
    CurrentProgress.CurrentPhase = EStoryPhase::Introduction;
    CurrentProgress.CurrentLocation = EStoryLocation::Tokyo;
    CurrentProgress.CurrentChapterID = 0;
    CurrentProgress.SaveGameVersion = TEXT("1.0");
    
    InitializeStorySystem();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Story Manager initialized"));
}

void UStoryManager::Deinitialize()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Story Manager shutting down"));
    
    // Auto-save on shutdown
    if (bAutoSaveEnabled)
    {
        SaveStoryProgress();
    }
    
    Super::Deinitialize();
}

// Story Progression
void UStoryManager::InitializeStorySystem()
{
    // Initialize default content
    InitializeDefaultChapters();
    InitializeDefaultCharacters();
    
    // Set up initial state
    CurrentProgress.LastSaveTime = FDateTime::Now();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Story system initialized - Starting Phase: %s, Location: %s"), 
           *UEnum::GetValueAsString(CurrentProgress.CurrentPhase),
           *UEnum::GetValueAsString(CurrentProgress.CurrentLocation));
}

void UStoryManager::StartChapter(int32 ChapterID)
{
    if (FStoryChapter* Chapter = GetChapterByID(ChapterID))
    {
        if (!Chapter->IsCompleted)
        {
            CurrentProgress.CurrentChapterID = ChapterID;
            CurrentProgress.CurrentPhase = Chapter->ChapterPhase;
            CurrentProgress.CurrentLocation = Chapter->PrimaryLocation;
            
            OnChapterStarted.Broadcast(*Chapter);
            OnPhaseChanged.Broadcast(Chapter->ChapterPhase);
            OnLocationChanged.Broadcast(Chapter->PrimaryLocation);
            
            // Record analytics
            if (bAnalyticsEnabled)
            {
                RecordStoryAnalytics(TEXT("ChapterStarted"), FString::Printf(TEXT("Chapter_%d"), ChapterID));
            }
            
            UE_LOG(LogProjectVisible, Log, TEXT("Started Chapter %d: %s"), ChapterID, *Chapter->ChapterTitle);
            
            // Update UI
            if (UProjectVisibleUIManager* UIManager = GetUIManager())
            {
                UIManager->UpdateInvestigationDisplayData();
            }
        }
        else
        {
            UE_LOG(LogProjectVisible, Warning, TEXT("Cannot start completed chapter: %d"), ChapterID);
        }
    }
    else
    {
        UE_LOG(LogProjectVisible, Error, TEXT("Chapter not found: %d"), ChapterID);
    }
}

void UStoryManager::CompleteChapter(int32 ChapterID)
{
    if (FStoryChapter* Chapter = GetChapterByID(ChapterID))
    {
        Chapter->IsCompleted = true;
        Chapter->CompletionPercentage = 100.0f;
        
        CurrentProgress.TotalCasesCompleted += Chapter->CompletedCases;
        CurrentProgress.CompletedChapters.Add(*Chapter);
        
        OnChapterCompleted.Broadcast(*Chapter);
        
        // Record analytics
        if (bAnalyticsEnabled)
        {
            RecordStoryAnalytics(TEXT("ChapterCompleted"), 
                               FString::Printf(TEXT("Chapter_%d_Time_%.1f"), ChapterID, Chapter->PlayTimeMinutes));
        }
        
        UE_LOG(LogProjectVisible, Log, TEXT("Completed Chapter %d: %s (%.1f minutes)"), 
               ChapterID, *Chapter->ChapterTitle, Chapter->PlayTimeMinutes);
        
        // Auto-advance to next chapter
        if (ChapterID < Chapters.Num() - 1)
        {
            StartChapter(ChapterID + 1);
        }
        else
        {
            // Story completed
            CurrentProgress.CurrentPhase = EStoryPhase::Completed;
            UE_LOG(LogProjectVisible, Log, TEXT("Story completed!"));
        }
        
        UpdateProgressCalculations();
    }
}

void UStoryManager::AdvanceToNextPhase()
{
    EStoryPhase NewPhase = EStoryPhase::Introduction;
    
    switch (CurrentProgress.CurrentPhase)
    {
        case EStoryPhase::Introduction:
            NewPhase = EStoryPhase::Chapter1;
            break;
        case EStoryPhase::Chapter1:
            NewPhase = EStoryPhase::Chapter2;
            break;
        case EStoryPhase::Chapter2:
            NewPhase = EStoryPhase::Chapter3;
            break;
        case EStoryPhase::Chapter3:
            NewPhase = EStoryPhase::Chapter4;
            break;
        case EStoryPhase::Chapter4:
            NewPhase = EStoryPhase::Chapter5;
            break;
        case EStoryPhase::Chapter5:
            NewPhase = EStoryPhase::Epilogue;
            break;
        case EStoryPhase::Epilogue:
            NewPhase = EStoryPhase::Completed;
            break;
        default:
            NewPhase = CurrentProgress.CurrentPhase;
            break;
    }
    
    if (NewPhase != CurrentProgress.CurrentPhase)
    {
        CurrentProgress.CurrentPhase = NewPhase;
        OnPhaseChanged.Broadcast(NewPhase);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Advanced to phase: %s"), *UEnum::GetValueAsString(NewPhase));
    }
}

void UStoryManager::SetCurrentLocation(EStoryLocation NewLocation)
{
    if (NewLocation != CurrentProgress.CurrentLocation)
    {
        EStoryLocation OldLocation = CurrentProgress.CurrentLocation;
        CurrentProgress.CurrentLocation = NewLocation;
        
        OnLocationChanged.Broadcast(NewLocation);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Location changed: %s -> %s"), 
               *UEnum::GetValueAsString(OldLocation), *UEnum::GetValueAsString(NewLocation));
        
        // Update UI
        if (UProjectVisibleUIManager* UIManager = GetUIManager())
        {
            UIManager->UpdateInvestigationDisplayData();
        }
    }
}

// Character Management
void UStoryManager::RegisterCharacter(const FString& CharacterID, const FString& Name, 
                                     ECharacterType Type, EStoryLocation Location)
{
    FStoryCharacter NewCharacter;
    NewCharacter.CharacterID = CharacterID;
    NewCharacter.CharacterName = Name;
    NewCharacter.DisplayName = Name;
    NewCharacter.CharacterType = Type;
    NewCharacter.CurrentLocation = Location;
    
    // Set defaults based on character type
    switch (Type)
    {
        case ECharacterType::Nicomachos:
            NewCharacter.TrustLevel = 30.0f;
            NewCharacter.ImportanceRating = 5;
            NewCharacter.KnowledgeLevel = 100;
            break;
        case ECharacterType::Android:
            NewCharacter.TrustLevel = 60.0f;
            NewCharacter.ImportanceRating = 2;
            NewCharacter.KnowledgeLevel = 50;
            break;
        case ECharacterType::AI:
            NewCharacter.TrustLevel = 80.0f;
            NewCharacter.ImportanceRating = 3;
            NewCharacter.KnowledgeLevel = 90;
            break;
        default:
            NewCharacter.TrustLevel = 50.0f;
            NewCharacter.ImportanceRating = 1;
            NewCharacter.KnowledgeLevel = 25;
            break;
    }
    
    Characters.Add(NewCharacter);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Registered character: %s (%s) at %s"), 
           *Name, *UEnum::GetValueAsString(Type), *UEnum::GetValueAsString(Location));
}

bool UStoryManager::GetCharacter(const FString& CharacterID, FStoryCharacter& OutCharacter) const
{
    const FStoryCharacter* Character = GetCharacterByID(CharacterID);
    if (Character)
    {
        OutCharacter = *Character;
        return true;
    }
    return false;
}

TArray<FStoryCharacter> UStoryManager::GetCharactersAtLocation(EStoryLocation Location) const
{
    TArray<FStoryCharacter> LocationCharacters;
    for (const FStoryCharacter& Character : Characters)
    {
        if (Character.CurrentLocation == Location)
        {
            LocationCharacters.Add(Character);
        }
    }
    return LocationCharacters;
}

void UStoryManager::UpdateCharacterTrust(const FString& CharacterID, float TrustChange)
{
    if (FStoryCharacter* Character = GetCharacterByID(CharacterID))
    {
        float OldTrust = Character->TrustLevel;
        Character->TrustLevel = FMath::Clamp(Character->TrustLevel + TrustChange, 0.0f, 100.0f);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Updated %s trust: %.1f -> %.1f (%.1f change)"), 
               *Character->CharacterName, OldTrust, Character->TrustLevel, TrustChange);
        
        // Record in virtue system
        if (UVirtueManager* VirtueManager = GetVirtueManager())
        {
            if (TrustChange > 0)
            {
                VirtueManager->RecordJusticeAction(TEXT("Building Trust"), true, FMath::Abs(TrustChange) / 20.0f);
            }
            else
            {
                VirtueManager->RecordJusticeAction(TEXT("Losing Trust"), false, FMath::Abs(TrustChange) / 20.0f);
            }
        }
    }
}

// Dialogue System
void UStoryManager::StartDialogue(const FString& CharacterID, const FString& DialogueID)
{
    const FStoryCharacter* Character = GetCharacterByID(CharacterID);
    const FStoryDialogue* Dialogue = GetDialogueByID(DialogueID);
    
    if (Character && Dialogue)
    {
        CurrentDialogueID = DialogueID;
        OnDialogueStarted.Broadcast(*Character, *Dialogue);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Started dialogue: %s with %s"), 
               *DialogueID, *Character->CharacterName);
        
        // Update UI to dialogue mode
        if (UProjectVisibleUIManager* UIManager = GetUIManager())
        {
            UIManager->PushScreen(EProjectVisibleScreenType::Investigation);
        }
    }
}

void UStoryManager::MakeDialogueChoice(const FString& DialogueID, const FString& ChoiceID, 
                                      EDialogueChoiceType ChoiceType)
{
    if (FStoryDialogue* Dialogue = GetDialogueByID(DialogueID))
    {
        CurrentProgress.TotalChoicesMade++;
        CurrentProgress.PlayerChoices.Add(ChoiceID, Dialogue->DialogueText);
        
        OnChoiceMade.Broadcast(ChoiceID, ChoiceType, Dialogue->DialogueText);
        
        // Process consequences
        ProcessDialogueConsequences(*Dialogue, ChoiceType);
        
        // Record analytics
        if (bAnalyticsEnabled)
        {
            RecordChoiceAnalytics(ChoiceID, ChoiceType);
        }
        
        UE_LOG(LogProjectVisible, Log, TEXT("Made dialogue choice: %s (%s)"), 
               *ChoiceID, *UEnum::GetValueAsString(ChoiceType));
    }
}

// Investigation System
void UStoryManager::StartInvestigation(const FString& CaseID, const FString& CaseName, 
                                      EStoryLocation Location)
{
    FStoryInvestigationCase NewCase;
    NewCase.CaseID = GetTypeHash(CaseID);
    NewCase.CaseName = CaseName;
    NewCase.Location = Location;
    NewCase.IsCompleted = false;
    NewCase.RequiredEvidenceCount = 3; // Default
    
    Cases.Add(NewCase);
    CurrentCaseID = CaseID;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Started investigation: %s at %s"), 
           *CaseName, *UEnum::GetValueAsString(Location));
}

void UStoryManager::AddEvidenceToCase(const FString& CaseID, const FString& EvidenceID)
{
    if (FStoryInvestigationCase* Case = GetCaseByID(CaseID))
    {
        Case->CollectedClues.Add(EvidenceID);
        Case->CollectedEvidenceCount++;
        UpdateCaseProgress(CaseID);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Added evidence %s to case %s"), *EvidenceID, *CaseID);
        
        // Create memory of evidence
        if (UMemoryManager* MemoryManager = GetMemoryManager())
        {
            MemoryManager->CreateMemory(
                FString::Printf(TEXT("Evidence: %s"), *EvidenceID),
                FString::Printf(TEXT("Found evidence in case: %s"), *Case->CaseName),
                EMemoryType::Episodic,
                EMemoryImportance::Medium,
                60.0f
            );
        }
    }
}

TArray<FStoryInvestigationCase> UStoryManager::GetActiveCases() const
{
    TArray<FStoryInvestigationCase> ActiveCases;
    for (const FStoryInvestigationCase& Case : Cases)
    {
        if (!Case.IsCompleted)
        {
            ActiveCases.Add(Case);
        }
    }
    return ActiveCases;
}

// Helper Functions
void UStoryManager::InitializeDefaultChapters()
{
    // Chapter 1: Tokyo Encounter
    FStoryChapter Chapter1;
    Chapter1.ChapterID = 1;
    Chapter1.ChapterTitle = TEXT("東京での出会い");
    Chapter1.ChapterDescription = TEXT("2089年の東京で謎めいた存在ニコマコスと遭遇する");
    Chapter1.ChapterPhase = EStoryPhase::Chapter1;
    Chapter1.PrimaryLocation = EStoryLocation::Tokyo;
    Chapter1.ExpectedDurationMinutes = 45.0f;
    Chapter1.RequiredCases = 2;
    Chapters.Add(Chapter1);
    
    // Chapter 2: Karuizawa Memories
    FStoryChapter Chapter2;
    Chapter2.ChapterID = 2;
    Chapter2.ChapterTitle = TEXT("軽井沢の記憶");
    Chapter2.ChapterDescription = TEXT("別荘地で過去の記憶と実験の痕跡を発見する");
    Chapter2.ChapterPhase = EStoryPhase::Chapter2;
    Chapter2.PrimaryLocation = EStoryLocation::Karuizawa;
    Chapter2.ExpectedDurationMinutes = 60.0f;
    Chapter2.RequiredCases = 3;
    Chapters.Add(Chapter2);
    
    // Chapter 3: Kagawa Experiments
    FStoryChapter Chapter3;
    Chapter3.ChapterID = 3;
    Chapter3.ChapterTitle = TEXT("香川の実験");
    Chapter3.ChapterDescription = TEXT("実験施設で幸福論実験の真相に迫る");
    Chapter3.ChapterPhase = EStoryPhase::Chapter3;
    Chapter3.PrimaryLocation = EStoryLocation::Kagawa;
    Chapter3.ExpectedDurationMinutes = 75.0f;
    Chapter3.RequiredCases = 4;
    Chapters.Add(Chapter3);
    
    // Chapter 4: Yamagata Truth
    FStoryChapter Chapter4;
    Chapter4.ChapterID = 4;
    Chapter4.ChapterTitle = TEXT("山形の真実");
    Chapter4.ChapterDescription = TEXT("ニコマコスの隠れ家で究極の真実を発見する");
    Chapter4.ChapterPhase = EStoryPhase::Chapter4;
    Chapter4.PrimaryLocation = EStoryLocation::Yamagata;
    Chapter4.ExpectedDurationMinutes = 90.0f;
    Chapter4.RequiredCases = 5;
    Chapters.Add(Chapter4);
    
    // Chapter 5: Boundary Collapse
    FStoryChapter Chapter5;
    Chapter5.ChapterID = 5;
    Chapter5.ChapterTitle = TEXT("境界の崩壊");
    Chapter5.ChapterDescription = TEXT("現実と夢の境界が崩壊し、真の選択の時が来る");
    Chapter5.ChapterPhase = EStoryPhase::Chapter5;
    Chapter5.PrimaryLocation = EStoryLocation::BoundaryZone;
    Chapter5.ExpectedDurationMinutes = 120.0f;
    Chapter5.RequiredCases = 1;
    Chapters.Add(Chapter5);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Initialized %d default chapters"), Chapters.Num());
}

void UStoryManager::InitializeDefaultCharacters()
{
    // Register key characters
    RegisterCharacter(TEXT("Nicomachos"), TEXT("ニコマコス"), ECharacterType::Nicomachos, EStoryLocation::Tokyo);
    RegisterCharacter(TEXT("Android_A1"), TEXT("市民A1"), ECharacterType::Android, EStoryLocation::Tokyo);
    RegisterCharacter(TEXT("AI_Guide"), TEXT("案内AI"), ECharacterType::AI, EStoryLocation::Tokyo);
    RegisterCharacter(TEXT("Memory_Voice"), TEXT("記憶の声"), ECharacterType::Memory, EStoryLocation::DreamLandscape);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Initialized %d default characters"), Characters.Num());
}

FStoryCharacter* UStoryManager::GetCharacterByID(const FString& CharacterID)
{
    for (FStoryCharacter& Character : Characters)
    {
        if (Character.CharacterID == CharacterID)
        {
            return &Character;
        }
    }
    return nullptr;
}

const FStoryCharacter* UStoryManager::GetCharacterByID(const FString& CharacterID) const
{
    for (const FStoryCharacter& Character : Characters)
    {
        if (Character.CharacterID == CharacterID)
        {
            return &Character;
        }
    }
    return nullptr;
}

FStoryChapter* UStoryManager::GetChapterByID(int32 ChapterID)
{
    for (FStoryChapter& Chapter : Chapters)
    {
        if (Chapter.ChapterID == ChapterID)
        {
            return &Chapter;
        }
    }
    return nullptr;
}

const FStoryChapter* UStoryManager::GetChapterByID(int32 ChapterID) const
{
    for (const FStoryChapter& Chapter : Chapters)
    {
        if (Chapter.ChapterID == ChapterID)
        {
            return &Chapter;
        }
    }
    return nullptr;
}

void UStoryManager::UpdateProgressCalculations()
{
    float TotalProgress = 0.0f;
    int32 CompletedChapters = 0;
    
    for (const FStoryChapter& Chapter : Chapters)
    {
        TotalProgress += Chapter.CompletionPercentage;
        if (Chapter.IsCompleted)
        {
            CompletedChapters++;
        }
    }
    
    if (Chapters.Num() > 0)
    {
        CurrentProgress.OverallCompletionPercentage = TotalProgress / Chapters.Num();
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Updated progress: %.1f%% (%d/%d chapters completed)"), 
           CurrentProgress.OverallCompletionPercentage, CompletedChapters, Chapters.Num());
}

void UStoryManager::ProcessDialogueConsequences(const FStoryDialogue& Dialogue, EDialogueChoiceType ChoiceType)
{
    // Update character trust based on choice type
    if (!Dialogue.SpeakerID.IsEmpty())
    {
        UpdateCharacterTrust(Dialogue.SpeakerID, Dialogue.TrustImpact);
    }
    
    // Record virtue-based choice
    if (UVirtueManager* VirtueManager = GetVirtueManager())
    {
        switch (ChoiceType)
        {
            case EDialogueChoiceType::Question:
            case EDialogueChoiceType::Logic:
                VirtueManager->RecordWisdomAction(TEXT("Dialogue Choice"), true, 1.0f);
                break;
            case EDialogueChoiceType::Persuasion:
            case EDialogueChoiceType::Intimidation:
                VirtueManager->RecordCourageAction(TEXT("Dialogue Choice"), true, 1.0f);
                break;
            case EDialogueChoiceType::Accusation:
                VirtueManager->RecordJusticeAction(TEXT("Dialogue Choice"), true, 1.0f);
                break;
            case EDialogueChoiceType::Empathy:
            case EDialogueChoiceType::Silence:
                VirtueManager->RecordTemperanceAction(TEXT("Dialogue Choice"), true, 1.0f);
                break;
            default:
                break;
        }
    }
    
    // Trigger social experiment if applicable
    if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
    {
        ExperimentManager->RecordMoralChoice(
            1, // Default experiment ID
            TEXT("CURRENT_PLAYER"),
            TEXT("Dialogue Choice"),
            UEnum::GetValueAsString(ChoiceType),
            Dialogue.EmotionalWeight
        );
    }
}

// System Integration
UVirtueManager* UStoryManager::GetVirtueManager()
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

UMemoryManager* UStoryManager::GetMemoryManager()
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

USocialExperimentManager* UStoryManager::GetSocialExperimentManager()
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

UProjectVisibleUIManager* UStoryManager::GetUIManager()
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

// Placeholder implementations for remaining functions
FStoryChapter UStoryManager::GetCurrentChapter() const
{
    const FStoryChapter* Chapter = GetChapterByID(CurrentProgress.CurrentChapterID);
    return Chapter ? *Chapter : FStoryChapter();
}

TArray<FStoryChapter> UStoryManager::GetAvailableChapters() const
{
    return Chapters;
}

void UStoryManager::UpdateChapterProgress(int32 ChapterID, float ProgressPercentage)
{
    if (FStoryChapter* Chapter = GetChapterByID(ChapterID))
    {
        Chapter->CompletionPercentage = FMath::Clamp(ProgressPercentage, 0.0f, 100.0f);
        UpdateProgressCalculations();
    }
}

bool UStoryManager::CreateChapter(int32 ChapterID, const FString& Title, const FString& Description, 
                                 EStoryPhase Phase, EStoryLocation Location)
{
    if (GetChapterByID(ChapterID))
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Chapter %d already exists"), ChapterID);
        return false;
    }
    
    FStoryChapter NewChapter;
    NewChapter.ChapterID = ChapterID;
    NewChapter.ChapterTitle = Title;
    NewChapter.ChapterDescription = Description;
    NewChapter.ChapterPhase = Phase;
    NewChapter.PrimaryLocation = Location;
    
    Chapters.Add(NewChapter);
    UE_LOG(LogProjectVisible, Log, TEXT("Created chapter %d: %s"), ChapterID, *Title);
    return true;
}

void UStoryManager::RecordStoryAnalytics(const FString& EventName, const FString& EventData)
{
    if (bAnalyticsEnabled)
    {
        UE_LOG(LogProjectVisible, Log, TEXT("Story Analytics: %s - %s"), *EventName, *EventData);
        // TODO: Integrate with analytics system
    }
}

void UStoryManager::RecordChoiceAnalytics(const FString& ChoiceID, EDialogueChoiceType ChoiceType)
{
    RecordStoryAnalytics(TEXT("DialogueChoice"), 
                        FString::Printf(TEXT("%s_%s"), *ChoiceID, *UEnum::GetValueAsString(ChoiceType)));
}

// Save/Load placeholder implementations
void UStoryManager::SaveStoryProgress()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Saving story progress"));
    CurrentProgress.LastSaveTime = FDateTime::Now();
}

void UStoryManager::LoadStoryProgress()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Loading story progress"));
}

// Additional placeholder implementations
FStoryInvestigationCase* UStoryManager::GetCaseByID(const FString& CaseID)
{
    uint32 CaseHash = GetTypeHash(CaseID);
    for (FStoryInvestigationCase& Case : Cases)
    {
        if (Case.CaseID == CaseHash)
        {
            return &Case;
        }
    }
    return nullptr;
}

const FStoryInvestigationCase* UStoryManager::GetCaseByID(const FString& CaseID) const
{
    uint32 CaseHash = GetTypeHash(CaseID);
    for (const FStoryInvestigationCase& Case : Cases)
    {
        if (Case.CaseID == CaseHash)
        {
            return &Case;
        }
    }
    return nullptr;
}

void UStoryManager::UpdateCaseProgress(const FString& CaseID)
{
    if (FStoryInvestigationCase* Case = GetCaseByID(CaseID))
    {
        float Progress = 0.0f;
        if (Case->RequiredEvidenceCount > 0)
        {
            Progress = (float)Case->CollectedEvidenceCount / (float)Case->RequiredEvidenceCount * 100.0f;
        }
        Case->ProgressPercentage = FMath::Clamp(Progress, 0.0f, 100.0f);
        
        if (Case->ProgressPercentage >= 100.0f && !Case->IsCompleted)
        {
            Case->IsCompleted = true;
            OnCaseCompleted.Broadcast(*Case);
            UE_LOG(LogProjectVisible, Log, TEXT("Case completed: %s"), *Case->CaseName);
        }
    }
}

FStoryDialogue* UStoryManager::GetDialogueByID(const FString& DialogueID)
{
    for (FStoryDialogue& Dialogue : Dialogues)
    {
        if (Dialogue.DialogueID == DialogueID)
        {
            return &Dialogue;
        }
    }
    return nullptr;
}

const FStoryDialogue* UStoryManager::GetDialogueByID(const FString& DialogueID) const
{
    for (const FStoryDialogue& Dialogue : Dialogues)
    {
        if (Dialogue.DialogueID == DialogueID)
        {
            return &Dialogue;
        }
    }
    return nullptr;
}

// Missing function implementations
bool UStoryManager::GetChapter(int32 ChapterID, FStoryChapter& OutChapter) const
{
    const FStoryChapter* Chapter = GetChapterByID(ChapterID);
    if (Chapter)
    {
        OutChapter = *Chapter;
        return true;
    }
    return false;
}

void UStoryManager::UpdateCharacterEmotion(const FString& CharacterID, float EmotionalState)
{
    if (FStoryCharacter* Character = GetCharacterByID(CharacterID))
    {
        Character->EmotionalState = FMath::Clamp(EmotionalState, 0.0f, 100.0f);
        UE_LOG(LogProjectVisible, Log, TEXT("Updated %s emotion: %.1f"), *Character->CharacterName, EmotionalState);
    }
}

void UStoryManager::RevealCharacterSecret(const FString& CharacterID, const FString& SecretID)
{
    if (FStoryCharacter* Character = GetCharacterByID(CharacterID))
    {
        Character->Secrets.Add(SecretID);
        UE_LOG(LogProjectVisible, Log, TEXT("Revealed secret %s for %s"), *SecretID, *Character->CharacterName);
    }
}

void UStoryManager::EndDialogue(const FString& DialogueID)
{
    CurrentDialogueID = TEXT("");
    CurrentProgress.TotalDialoguesCompleted++;
    UE_LOG(LogProjectVisible, Log, TEXT("Ended dialogue: %s"), *DialogueID);
}

TArray<FStoryDialogue> UStoryManager::GetAvailableDialogues(const FString& CharacterID) const
{
    TArray<FStoryDialogue> AvailableDialogues;
    for (const FStoryDialogue& Dialogue : Dialogues)
    {
        if (Dialogue.SpeakerID == CharacterID)
        {
            AvailableDialogues.Add(Dialogue);
        }
    }
    return AvailableDialogues;
}

void UStoryManager::RegisterDialogue(const FString& DialogueID, const FString& SpeakerID, const FString& Text, EDialogueChoiceType ChoiceType)
{
    FStoryDialogue NewDialogue;
    NewDialogue.DialogueID = DialogueID;
    NewDialogue.SpeakerID = SpeakerID;
    NewDialogue.DialogueText = Text;
    NewDialogue.ChoiceType = ChoiceType;
    Dialogues.Add(NewDialogue);
    UE_LOG(LogProjectVisible, Log, TEXT("Registered dialogue: %s"), *DialogueID);
}

void UStoryManager::CompleteInvestigation(const FString& CaseID, const FString& Conclusion)
{
    if (FStoryInvestigationCase* Case = GetCaseByID(CaseID))
    {
        Case->IsCompleted = true;
        Case->ConclusionText = Conclusion;
        Case->ProgressPercentage = 100.0f;
        OnCaseCompleted.Broadcast(*Case);
        UE_LOG(LogProjectVisible, Log, TEXT("Completed investigation: %s"), *Case->CaseName);
    }
}

void UStoryManager::AddClueToCase(const FString& CaseID, const FString& ClueID)
{
    if (FStoryInvestigationCase* Case = GetCaseByID(CaseID))
    {
        Case->CollectedClues.Add(ClueID);
        UpdateCaseProgress(CaseID);
        UE_LOG(LogProjectVisible, Log, TEXT("Added clue %s to case %s"), *ClueID, *CaseID);
    }
}

bool UStoryManager::GetCase(const FString& CaseID, FStoryInvestigationCase& OutCase) const
{
    const FStoryInvestigationCase* Case = GetCaseByID(CaseID);
    if (Case)
    {
        OutCase = *Case;
        return true;
    }
    return false;
}

void UStoryManager::RecordPlayerChoice(const FString& ChoiceID, const FString& ChoiceText, const FString& Context)
{
    CurrentProgress.PlayerChoices.Add(ChoiceID, ChoiceText);
    CurrentProgress.TotalChoicesMade++;
    UE_LOG(LogProjectVisible, Log, TEXT("Recorded choice: %s - %s"), *ChoiceID, *ChoiceText);
}

TArray<FString> UStoryManager::GetPlayerChoiceHistory() const
{
    TArray<FString> ChoiceHistory;
    for (const auto& Choice : CurrentProgress.PlayerChoices)
    {
        ChoiceHistory.Add(FString::Printf(TEXT("%s: %s"), *Choice.Key, *Choice.Value));
    }
    return ChoiceHistory;
}

void UStoryManager::TriggerChoiceConsequence(const FString& ChoiceID, const FString& ConsequenceText)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Choice consequence: %s - %s"), *ChoiceID, *ConsequenceText);
}

void UStoryManager::RevealTruth(const FString& TruthID, const FString& TruthDescription)
{
    CurrentProgress.DiscoveredTruths.Add(TruthID);
    OnTruthRevealed.Broadcast(TruthID, TruthDescription);
    UpdateProgressCalculations();
    UE_LOG(LogProjectVisible, Log, TEXT("Truth revealed: %s - %s"), *TruthID, *TruthDescription);
}

TArray<FString> UStoryManager::GetDiscoveredTruths() const
{
    return CurrentProgress.DiscoveredTruths;
}

float UStoryManager::GetTruthDiscoveryPercentage() const
{
    return CurrentProgress.TruthDiscoveryPercentage;
}

void UStoryManager::UnlockSecret(const FString& SecretID, const FString& SecretDescription)
{
    CurrentProgress.UnlockedSecrets.Add(SecretID);
    UE_LOG(LogProjectVisible, Log, TEXT("Secret unlocked: %s - %s"), *SecretID, *SecretDescription);
}

void UStoryManager::TriggerStoryEvent(EStoryEventType EventType, const FString& EventData)
{
    OnStoryEventTriggered.Broadcast(EventType);
    UE_LOG(LogProjectVisible, Log, TEXT("Story event triggered: %s - %s"), *UEnum::GetValueAsString(EventType), *EventData);
}

void UStoryManager::ScheduleDelayedEvent(EStoryEventType EventType, const FString& EventData, float DelaySeconds)
{
    FDateTime ScheduledTime = FDateTime::Now() + FTimespan::FromSeconds(DelaySeconds);
    ScheduledEvents.Add(FString::Printf(TEXT("%s_%s"), *UEnum::GetValueAsString(EventType), *EventData), ScheduledTime);
    UE_LOG(LogProjectVisible, Log, TEXT("Scheduled event: %s in %.1f seconds"), *UEnum::GetValueAsString(EventType), DelaySeconds);
}

void UStoryManager::CreateSaveData(FStoryProgress& OutSaveData)
{
    OutSaveData = CurrentProgress;
    OutSaveData.LastSaveTime = FDateTime::Now();
    UE_LOG(LogProjectVisible, Log, TEXT("Created save data"));
}

void UStoryManager::LoadFromSaveData(const FStoryProgress& SaveData)
{
    CurrentProgress = SaveData;
    UpdateProgressCalculations();
    UE_LOG(LogProjectVisible, Log, TEXT("Loaded from save data"));
}

void UStoryManager::AnalyzePlayerBehavior()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Analyzing player behavior - Choices: %d, Dialogues: %d"), 
           CurrentProgress.TotalChoicesMade, CurrentProgress.TotalDialoguesCompleted);
}
