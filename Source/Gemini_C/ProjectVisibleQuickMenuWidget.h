// Project Visible - Quick Menu Widget
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectVisibleBaseWidget.h"
#include "ProjectVisibleQuickMenuWidget.generated.h"

/**
 * Quick menu widget for in-game access - maintains game controls (GameAndUI mode)
 */
UCLASS(BlueprintType, Blueprintable)
class GEMINI_C_API UProjectVisibleQuickMenuWidget : public UProjectVisibleBaseWidget
{
	GENERATED_BODY()

public:
	UProjectVisibleQuickMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
	// UUserWidget interface
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	// UProjectVisibleBaseWidget interface
	virtual void InitializeWidget() override;

public:
	// Quick Menu Actions
	UFUNCTION(BlueprintCallable, Category = "Quick Menu")
	void OpenFullSettings();

	UFUNCTION(BlueprintCallable, Category = "Quick Menu")
	void OpenVirtueProfile();

	UFUNCTION(BlueprintCallable, Category = "Quick Menu")
	void OpenMemoryViewer();

	UFUNCTION(BlueprintCallable, Category = "Quick Menu")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Quick Menu")
	void LoadGame();

	UFUNCTION(BlueprintCallable, Category = "Quick Menu")
	void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Quick Menu")
	void CloseQuickMenu();

	// System Controls
	UFUNCTION(BlueprintCallable, Category = "Quick Menu")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category = "Quick Menu")
	void ToggleMap();

	UFUNCTION(BlueprintCallable, Category = "Quick Menu")
	void ToggleJournal();

	// Quick Actions - BlueprintImplementableEvent for UMG
	UFUNCTION(BlueprintImplementableEvent, Category = "Quick Menu")
	void UpdateQuickStats();

	UFUNCTION(BlueprintImplementableEvent, Category = "Quick Menu")
	void UpdateCurrentObjective();

	UFUNCTION(BlueprintImplementableEvent, Category = "Quick Menu")
	void UpdateGameProgress();

protected:
	// Menu state
	UPROPERTY(BlueprintReadOnly, Category = "Quick Menu")
	bool bIsMinimized = false;

	UPROPERTY(BlueprintReadOnly, Category = "Quick Menu")
	float MenuOpacity = 0.9f;

	// Quick access data
	UPROPERTY(BlueprintReadOnly, Category = "Quick Menu")
	FString CurrentObjective;

	UPROPERTY(BlueprintReadOnly, Category = "Quick Menu")
	float GameProgressPercentage = 0.0f;

private:
	// Menu button structure
	struct FMenuButton
	{
		FString Text;
		FVector2D Position;
		FVector2D Size;
		FLinearColor Color;
		int32 ButtonID;

		FMenuButton(const FString& InText, const FVector2D& InPos, const FVector2D& InSize, const FLinearColor& InColor, int32 InID)
			: Text(InText), Position(InPos), Size(InSize), Color(InColor), ButtonID(InID) {}
	};

	// Helper functions
	void RefreshQuickMenuData();
	void UpdateMenuVisuals();
	void DrawMenuButtons(FSlateWindowElementList& OutDrawElements, const FGeometry& AllottedGeometry, int32 LayerId, const FVector2D& PanelPos, const FVector2D& PanelSize) const;
	void InitializeMenuButtons();
	void HandleButtonClick(int32 ButtonID);
	FMenuButton* GetButtonAtPosition(const FVector2D& LocalPosition, const FVector2D& PanelPos, const FVector2D& PanelSize);

	// Menu buttons data
	TArray<FMenuButton> MenuButtons;
};
