#include "MemoryFragmentManager.h"
#include "Gemini_C.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"

void UMemoryFragmentManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogProjectVisible, Log, TEXT("MemoryFragmentManager: Initializing memory fragment system"));
    
    // Initialize default memory fragments and collections
    LoadDefaultMemoryFragments();
    LoadDefaultMemoryCollections();
    
    bIsInitialized = true;
    
    UE_LOG(LogProjectVisible, Log, TEXT("MemoryFragmentManager: Memory fragment system initialized successfully"));
}

void UMemoryFragmentManager::Deinitialize()
{
    UE_LOG(LogProjectVisible, Log, TEXT("MemoryFragmentManager: Deinitializing memory fragment system"));
    
    bIsInitialized = false;
    MemoryFragments.Empty();
    MemoryCollections.Empty();
    
    Super::Deinitialize();
}

void UMemoryFragmentManager::InitializeMemorySystem()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("MemoryFragmentManager: Initializing memory system"));
    
    // Reset current state
    MemoryFragments.Empty();
    MemoryCollections.Empty();
    
    // Load default data
    LoadDefaultMemoryFragments();
    LoadDefaultMemoryCollections();
    
    // Recalculate statistics
    RecalculateStatistics();
    
    UE_LOG(LogProjectVisible, Warning, TEXT("MemoryFragmentManager: Memory system initialized with %d fragments and %d collections"), 
        MemoryFragments.Num(), MemoryCollections.Num());
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, 
            FString::Printf(TEXT("Memory System Initialized: %d fragments, %d collections"), 
                MemoryFragments.Num(), MemoryCollections.Num()));
    }
}

FMemoryFragment UMemoryFragmentManager::GetMemoryFragment(const FString& FragmentID) const
{
    if (const FMemoryFragment* FoundFragment = MemoryFragments.Find(FragmentID))
    {
        return *FoundFragment;
    }
    
    UE_LOG(LogProjectVisible, Warning, TEXT("MemoryFragmentManager: Memory fragment %s not found"), *FragmentID);
    return FMemoryFragment();
}

bool UMemoryFragmentManager::HasMemoryFragment(const FString& FragmentID) const
{
    return MemoryFragments.Contains(FragmentID);
}

TArray<FMemoryFragment> UMemoryFragmentManager::GetAllMemoryFragments() const
{
    TArray<FMemoryFragment> AllFragments;
    for (const auto& FragmentPair : MemoryFragments)
    {
        AllFragments.Add(FragmentPair.Value);
    }
    return AllFragments;
}

TArray<FMemoryFragment> UMemoryFragmentManager::GetUnlockedMemoryFragments() const
{
    TArray<FMemoryFragment> UnlockedFragments;
    for (const auto& FragmentPair : MemoryFragments)
    {
        if (FragmentPair.Value.bIsUnlocked)
        {
            UnlockedFragments.Add(FragmentPair.Value);
        }
    }
    return UnlockedFragments;
}

TArray<FMemoryFragment> UMemoryFragmentManager::GetMemoryFragmentsByType(EMemoryFragmentType FragmentType) const
{
    TArray<FMemoryFragment> TypedFragments;
    for (const auto& FragmentPair : MemoryFragments)
    {
        if (FragmentPair.Value.FragmentType == FragmentType)
        {
            TypedFragments.Add(FragmentPair.Value);
        }
    }
    return TypedFragments;
}

bool UMemoryFragmentManager::UnlockMemoryFragment(const FString& FragmentID)
{
    if (FMemoryFragment* Fragment = MemoryFragments.Find(FragmentID))
    {
        if (!Fragment->bIsUnlocked)
        {
            Fragment->bIsUnlocked = true;
            Fragment->UnlockedTimestamp = FDateTime::Now();
            
            UE_LOG(LogProjectVisible, Warning, TEXT("MemoryFragmentManager: Unlocked memory fragment: %s"), *FragmentID);
            
            // Broadcast unlock event
            OnMemoryFragmentUnlocked.Broadcast(FragmentID);
            
            // Update collection completion
            for (auto& CollectionPair : MemoryCollections)
            {
                if (CollectionPair.Value.FragmentIDs.Contains(FragmentID))
                {
                    UpdateCollectionCompletion(CollectionPair.Key);
                }
            }
            
            // Recalculate statistics
            RecalculateStatistics();
            
            if (GEngine)
            {
                FString Title = Fragment->Title.ToString();
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor(255, 215, 0, 255),
                    FString::Printf(TEXT("Memory Unlocked: %s"), *Title));
            }
            
            return true;
        }
    }
    
    return false;
}

bool UMemoryFragmentManager::ViewMemoryFragment(const FString& FragmentID)
{
    if (FMemoryFragment* Fragment = MemoryFragments.Find(FragmentID))
    {
        if (Fragment->bIsUnlocked && !Fragment->bHasBeenViewed)
        {
            Fragment->bHasBeenViewed = true;
            
            UE_LOG(LogProjectVisible, Log, TEXT("MemoryFragmentManager: Viewed memory fragment: %s"), *FragmentID);
            
            // Broadcast view event
            OnMemoryFragmentViewed.Broadcast(FragmentID);
            
            // Recalculate statistics
            RecalculateStatistics();
            
            return true;
        }
    }
    
    return false;
}

bool UMemoryFragmentManager::IsMemoryFragmentUnlocked(const FString& FragmentID) const
{
    if (const FMemoryFragment* Fragment = MemoryFragments.Find(FragmentID))
    {
        return Fragment->bIsUnlocked;
    }
    return false;
}

bool UMemoryFragmentManager::IsMemoryFragmentViewed(const FString& FragmentID) const
{
    if (const FMemoryFragment* Fragment = MemoryFragments.Find(FragmentID))
    {
        return Fragment->bHasBeenViewed;
    }
    return false;
}

FMemoryStatistics UMemoryFragmentManager::GetMemoryStatistics() const
{
    return CachedStatistics;
}

int32 UMemoryFragmentManager::GetTotalMemoryFragmentCount() const
{
    return MemoryFragments.Num();
}

int32 UMemoryFragmentManager::GetUnlockedMemoryFragmentCount() const
{
    int32 Count = 0;
    for (const auto& FragmentPair : MemoryFragments)
    {
        if (FragmentPair.Value.bIsUnlocked)
        {
            Count++;
        }
    }
    return Count;
}

int32 UMemoryFragmentManager::GetViewedMemoryFragmentCount() const
{
    int32 Count = 0;
    for (const auto& FragmentPair : MemoryFragments)
    {
        if (FragmentPair.Value.bHasBeenViewed)
        {
            Count++;
        }
    }
    return Count;
}

float UMemoryFragmentManager::GetOverallMemoryCompletionPercentage() const
{
    if (MemoryFragments.Num() == 0)
    {
        return 0.0f;
    }
    
    return (float)GetUnlockedMemoryFragmentCount() / (float)MemoryFragments.Num() * 100.0f;
}

FText UMemoryFragmentManager::GetMemoryFragmentTypeDisplayName(EMemoryFragmentType FragmentType) const
{
    switch (FragmentType)
    {
        case EMemoryFragmentType::Episodic:    return FText::FromString(TEXT("エピソード記憶"));
        case EMemoryFragmentType::Semantic:    return FText::FromString(TEXT("意味記憶"));
        case EMemoryFragmentType::Procedural:  return FText::FromString(TEXT("手続き記憶"));
        case EMemoryFragmentType::Emotional:   return FText::FromString(TEXT("感情記憶"));
        case EMemoryFragmentType::Sensory:     return FText::FromString(TEXT("感覚記憶"));
        case EMemoryFragmentType::Conceptual:  return FText::FromString(TEXT("概念記憶"));
        default:                               return FText::FromString(TEXT("不明"));
    }
}

FLinearColor UMemoryFragmentManager::GetMemoryFragmentTypeColor(EMemoryFragmentType FragmentType) const
{
    switch (FragmentType)
    {
        case EMemoryFragmentType::Episodic:    return FLinearColor(0.8f, 0.6f, 0.2f, 1.0f); // Orange
        case EMemoryFragmentType::Semantic:    return FLinearColor(0.2f, 0.6f, 0.8f, 1.0f); // Blue
        case EMemoryFragmentType::Procedural:  return FLinearColor(0.6f, 0.8f, 0.2f, 1.0f); // Green
        case EMemoryFragmentType::Emotional:   return FLinearColor(0.8f, 0.2f, 0.6f, 1.0f); // Pink
        case EMemoryFragmentType::Sensory:     return FLinearColor(0.6f, 0.2f, 0.8f, 1.0f); // Purple
        case EMemoryFragmentType::Conceptual:  return FLinearColor(0.8f, 0.8f, 0.2f, 1.0f); // Yellow
        default:                               return FLinearColor(0.5f, 0.5f, 0.5f, 1.0f); // Gray
    }
}

void UMemoryFragmentManager::AddMemoryFragment(const FMemoryFragment& NewFragment)
{
    if (!NewFragment.FragmentID.IsEmpty())
    {
        MemoryFragments.Add(NewFragment.FragmentID, NewFragment);
        RecalculateStatistics();
        
        UE_LOG(LogProjectVisible, Log, TEXT("MemoryFragmentManager: Added memory fragment: %s"), *NewFragment.FragmentID);
    }
}

void UMemoryFragmentManager::ResetMemorySystem()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("MemoryFragmentManager: Resetting memory system"));
    
    MemoryFragments.Empty();
    MemoryCollections.Empty();
    
    LoadDefaultMemoryFragments();
    LoadDefaultMemoryCollections();
    RecalculateStatistics();
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("Memory System Reset"));
    }
}

void UMemoryFragmentManager::LoadDefaultMemoryFragments()
{
    const TArray<FDefaultMemoryFragment>& DefaultFragments = GetDefaultMemoryFragments();
    
    for (const FDefaultMemoryFragment& DefaultFragment : DefaultFragments)
    {
        FMemoryFragment Fragment;
        Fragment.FragmentID = DefaultFragment.ID;
        Fragment.Title = FText::FromString(DefaultFragment.Title);
        Fragment.Description = FText::FromString(DefaultFragment.Description);
        Fragment.Excerpt = FText::FromString(DefaultFragment.Excerpt);
        Fragment.FragmentType = DefaultFragment.Type;
        Fragment.EmotionalIntensity = DefaultFragment.Intensity;
        Fragment.Clarity = DefaultFragment.Clarity;
        Fragment.TimePeriod = DefaultFragment.TimePeriod;
        Fragment.ImportanceScore = DefaultFragment.ImportanceScore;
        Fragment.Keywords = DefaultFragment.Keywords;
        Fragment.LocationContext = DefaultFragment.LocationContext;
        Fragment.AssociatedCharacter = DefaultFragment.AssociatedCharacter;
        Fragment.bIsUnlocked = false; // Start locked
        Fragment.bHasBeenViewed = false;
        
        MemoryFragments.Add(Fragment.FragmentID, Fragment);
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("MemoryFragmentManager: Loaded %d default memory fragments"), DefaultFragments.Num());
}

void UMemoryFragmentManager::LoadDefaultMemoryCollections()
{
    const TArray<FDefaultMemoryCollection>& DefaultCollections = GetDefaultMemoryCollections();
    
    for (const FDefaultMemoryCollection& DefaultCollection : DefaultCollections)
    {
        FMemoryCollection Collection;
        Collection.CollectionID = DefaultCollection.ID;
        Collection.CollectionName = FText::FromString(DefaultCollection.Name);
        Collection.Description = FText::FromString(DefaultCollection.Description);
        Collection.FragmentIDs = DefaultCollection.FragmentIDs;
        Collection.ThemeColor = DefaultCollection.ThemeColor;
        Collection.bIsUnlocked = true; // Collections start unlocked
        Collection.CompletionPercentage = 0.0f;
        
        MemoryCollections.Add(Collection.CollectionID, Collection);
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("MemoryFragmentManager: Loaded %d default memory collections"), DefaultCollections.Num());
}

void UMemoryFragmentManager::UpdateCollectionCompletion(const FString& CollectionID)
{
    if (FMemoryCollection* Collection = MemoryCollections.Find(CollectionID))
    {
        int32 UnlockedCount = 0;
        for (const FString& FragmentID : Collection->FragmentIDs)
        {
            if (IsMemoryFragmentUnlocked(FragmentID))
            {
                UnlockedCount++;
            }
        }
        
        if (Collection->FragmentIDs.Num() > 0)
        {
            Collection->CompletionPercentage = (float)UnlockedCount / (float)Collection->FragmentIDs.Num() * 100.0f;
            
            // Check if collection is completed
            if (Collection->CompletionPercentage >= 100.0f)
            {
                OnMemoryCollectionCompleted.Broadcast(CollectionID);
                
                UE_LOG(LogProjectVisible, Warning, TEXT("MemoryFragmentManager: Collection completed: %s"), *CollectionID);
                
                if (GEngine)
                {
                    FString CollectionName = Collection->CollectionName.ToString();
                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple,
                        FString::Printf(TEXT("Memory Collection Completed: %s"), *CollectionName));
                }
            }
        }
    }
}

void UMemoryFragmentManager::RecalculateStatistics()
{
    CachedStatistics.TotalFragments = MemoryFragments.Num();
    CachedStatistics.UnlockedFragments = GetUnlockedMemoryFragmentCount();
    CachedStatistics.ViewedFragments = GetViewedMemoryFragmentCount();
    CachedStatistics.TotalCollections = MemoryCollections.Num();
    
    // Count completed collections
    CachedStatistics.CompletedCollections = 0;
    for (const auto& CollectionPair : MemoryCollections)
    {
        if (CollectionPair.Value.CompletionPercentage >= 100.0f)
        {
            CachedStatistics.CompletedCollections++;
        }
    }
    
    CachedStatistics.OverallCompletionPercentage = GetOverallMemoryCompletionPercentage();
}

const TArray<UMemoryFragmentManager::FDefaultMemoryFragment>& UMemoryFragmentManager::GetDefaultMemoryFragments()
{
    static TArray<FDefaultMemoryFragment> DefaultFragments = {
        {
            TEXT("childhood_001"),
            TEXT("母親との記憶"),
            TEXT("幼い頃、母親と一緒に公園で過ごした暖かい午後の記憶。"),
            TEXT("お母さんの優しい手..."),
            EMemoryFragmentType::Episodic,
            EMemoryEmotionalIntensity::Mild,
            EMemoryClarity::Hazy,
            EMemoryTimePeriod::Childhood,
            75.0f,
            {TEXT("母親"), TEXT("公園"), TEXT("幸せ")},
            TEXT("近所の公園"),
            TEXT("母親")
        },
        {
            TEXT("school_001"),
            TEXT("初登校の日"),
            TEXT("小学校の初日、不安と期待が入り混じった複雑な感情。"),
            TEXT("新しい制服を着て..."),
            EMemoryFragmentType::Episodic,
            EMemoryEmotionalIntensity::Moderate,
            EMemoryClarity::Partial,
            EMemoryTimePeriod::Childhood,
            60.0f,
            {TEXT("学校"), TEXT("不安"), TEXT("期待")},
            TEXT("小学校"),
            TEXT("先生")
        },
        {
            TEXT("concept_001"),
            TEXT("正義という概念"),
            TEXT("正義とは何か、善悪の判断について考えた記憶。"),
            TEXT("正しいこととは..."),
            EMemoryFragmentType::Conceptual,
            EMemoryEmotionalIntensity::Mild,
            EMemoryClarity::Clear,
            EMemoryTimePeriod::Adolescence,
            80.0f,
            {TEXT("正義"), TEXT("道徳"), TEXT("哲学")},
            TEXT("図書館"),
            TEXT("哲学者")
        },
        {
            TEXT("skill_001"),
            TEXT("自転車に乗れた日"),
            TEXT("初めて自転車に乗れるようになった達成感の記憶。"),
            TEXT("ペダルを踏んで..."),
            EMemoryFragmentType::Procedural,
            EMemoryEmotionalIntensity::Strong,
            EMemoryClarity::Vivid,
            EMemoryTimePeriod::Childhood,
            70.0f,
            {TEXT("自転車"), TEXT("達成感"), TEXT("練習")},
            TEXT("公園の広場"),
            TEXT("父親")
        }
    };
    
    return DefaultFragments;
}

const TArray<UMemoryFragmentManager::FDefaultMemoryCollection>& UMemoryFragmentManager::GetDefaultMemoryCollections()
{
    static TArray<FDefaultMemoryCollection> DefaultCollections = {
        {
            TEXT("childhood_memories"),
            TEXT("幼少期の記憶"),
            TEXT("子供時代の大切な思い出たち"),
            {TEXT("childhood_001"), TEXT("school_001"), TEXT("skill_001")},
            FLinearColor(0.8f, 0.6f, 0.4f, 1.0f)
        },
        {
            TEXT("philosophical_thoughts"),
            TEXT("哲学的思考"),
            TEXT("人生や存在について考えた記憶"),
            {TEXT("concept_001")},
            FLinearColor(0.6f, 0.4f, 0.8f, 1.0f)
        }
    };
    
    return DefaultCollections;
}

// Placeholder implementations for remaining functions
TArray<FMemoryFragment> UMemoryFragmentManager::SearchMemoryFragments(const FMemorySearchFilter& SearchFilter) const { return TArray<FMemoryFragment>(); }
FMemoryCollection UMemoryFragmentManager::GetMemoryCollection(const FString& CollectionID) const { return FMemoryCollection(); }
TArray<FMemoryCollection> UMemoryFragmentManager::GetAllMemoryCollections() const { return TArray<FMemoryCollection>(); }
TArray<FMemoryCollection> UMemoryFragmentManager::GetUnlockedMemoryCollections() const { return TArray<FMemoryCollection>(); }
float UMemoryFragmentManager::GetCollectionCompletionPercentage(const FString& CollectionID) const { return 0.0f; }
bool UMemoryFragmentManager::IsCollectionCompleted(const FString& CollectionID) const { return false; }
FText UMemoryFragmentManager::GetEmotionalIntensityDisplayName(EMemoryEmotionalIntensity Intensity) const { return FText(); }
FText UMemoryFragmentManager::GetClarityDisplayName(EMemoryClarity Clarity) const { return FText(); }
FText UMemoryFragmentManager::GetTimePeriodDisplayName(EMemoryTimePeriod TimePeriod) const { return FText(); }
void UMemoryFragmentManager::AddMemoryCollection(const FMemoryCollection& NewCollection) {}
bool UMemoryFragmentManager::MatchesSearchFilter(const FMemoryFragment& Fragment, const FMemorySearchFilter& Filter) const { return true; }
