#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MemoryFragmentTypes.h"
#include "Engine/DataTable.h"
#include "MemoryFragmentManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryFragmentUnlocked, const FString&, FragmentID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryFragmentViewed, const FString&, FragmentID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryCollectionCompleted, const FString&, CollectionID);

/**
 * Memory Fragment Manager - Game Instance Subsystem
 * Manages the memory fragment system for Project Visible
 */
UCLASS(BlueprintType)
class GEMINI_C_API UMemoryFragmentManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // === Memory Fragment Management ===

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    void InitializeMemorySystem();

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    FMemoryFragment GetMemoryFragment(const FString& FragmentID) const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    bool HasMemoryFragment(const FString& FragmentID) const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    TArray<FMemoryFragment> GetAllMemoryFragments() const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    TArray<FMemoryFragment> GetUnlockedMemoryFragments() const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    TArray<FMemoryFragment> GetMemoryFragmentsByType(EMemoryFragmentType FragmentType) const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    TArray<FMemoryFragment> SearchMemoryFragments(const FMemorySearchFilter& SearchFilter) const;

    // === Memory Fragment Actions ===

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    bool UnlockMemoryFragment(const FString& FragmentID);

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    bool ViewMemoryFragment(const FString& FragmentID);

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    bool IsMemoryFragmentUnlocked(const FString& FragmentID) const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    bool IsMemoryFragmentViewed(const FString& FragmentID) const;

    // === Memory Collections ===

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    FMemoryCollection GetMemoryCollection(const FString& CollectionID) const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    TArray<FMemoryCollection> GetAllMemoryCollections() const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    TArray<FMemoryCollection> GetUnlockedMemoryCollections() const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    float GetCollectionCompletionPercentage(const FString& CollectionID) const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    bool IsCollectionCompleted(const FString& CollectionID) const;

    // === Statistics and Analytics ===

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    FMemoryStatistics GetMemoryStatistics() const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    int32 GetTotalMemoryFragmentCount() const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    int32 GetUnlockedMemoryFragmentCount() const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    int32 GetViewedMemoryFragmentCount() const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    float GetOverallMemoryCompletionPercentage() const;

    // === Utility Functions ===

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    FText GetMemoryFragmentTypeDisplayName(EMemoryFragmentType FragmentType) const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    FLinearColor GetMemoryFragmentTypeColor(EMemoryFragmentType FragmentType) const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    FText GetEmotionalIntensityDisplayName(EMemoryEmotionalIntensity Intensity) const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    FText GetClarityDisplayName(EMemoryClarity Clarity) const;

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    FText GetTimePeriodDisplayName(EMemoryTimePeriod TimePeriod) const;

    // === Events ===

    UPROPERTY(BlueprintAssignable, Category = "Memory Fragment System")
    FOnMemoryFragmentUnlocked OnMemoryFragmentUnlocked;

    UPROPERTY(BlueprintAssignable, Category = "Memory Fragment System")
    FOnMemoryFragmentViewed OnMemoryFragmentViewed;

    UPROPERTY(BlueprintAssignable, Category = "Memory Fragment System")
    FOnMemoryCollectionCompleted OnMemoryCollectionCompleted;

    // === Data Management ===

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    void AddMemoryFragment(const FMemoryFragment& NewFragment);

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    void AddMemoryCollection(const FMemoryCollection& NewCollection);

    UFUNCTION(BlueprintCallable, Category = "Memory Fragment System")
    void ResetMemorySystem();

protected:
    // Current memory fragments
    UPROPERTY(BlueprintReadOnly, Category = "Memory Fragment System")
    TMap<FString, FMemoryFragment> MemoryFragments;

    // Current memory collections
    UPROPERTY(BlueprintReadOnly, Category = "Memory Fragment System")
    TMap<FString, FMemoryCollection> MemoryCollections;

    // Configuration data table for memory fragments
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    UDataTable* MemoryFragmentDataTable = nullptr;

    // Configuration data table for memory collections
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    UDataTable* MemoryCollectionDataTable = nullptr;

private:
    // === Internal Helper Functions ===
    void LoadDefaultMemoryFragments();
    void LoadDefaultMemoryCollections();
    void UpdateCollectionCompletion(const FString& CollectionID);
    void RecalculateStatistics();
    bool MatchesSearchFilter(const FMemoryFragment& Fragment, const FMemorySearchFilter& Filter) const;

    // === Default Data ===
    struct FDefaultMemoryFragment
    {
        FString ID;
        FString Title;
        FString Description;
        FString Excerpt;
        EMemoryFragmentType Type;
        EMemoryEmotionalIntensity Intensity;
        EMemoryClarity Clarity;
        EMemoryTimePeriod TimePeriod;
        float ImportanceScore;
        TArray<FString> Keywords;
        FString LocationContext;
        FString AssociatedCharacter;
    };

    struct FDefaultMemoryCollection
    {
        FString ID;
        FString Name;
        FString Description;
        TArray<FString> FragmentIDs;
        FLinearColor ThemeColor;
    };

    static const TArray<FDefaultMemoryFragment>& GetDefaultMemoryFragments();
    static const TArray<FDefaultMemoryCollection>& GetDefaultMemoryCollections();

    // Internal state
    bool bIsInitialized = false;
    FMemoryStatistics CachedStatistics;
};
