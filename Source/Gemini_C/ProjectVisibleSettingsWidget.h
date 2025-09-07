#pragma once

#include "CoreMinimal.h"
#include "ProjectVisibleBaseWidget.h"
#include "ProjectVisibleUITypes.h"
#include "Engine/Engine.h"
#include "ProjectVisibleSettingsWidget.generated.h"

/**
 * Settings screen widget for Project Visible
 * Provides game settings configuration interface
 */
UCLASS(BlueprintType, Blueprintable)
class GEMINI_C_API UProjectVisibleSettingsWidget : public UProjectVisibleBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	// Initialize widget content
	virtual void InitializeWidget() override;

	// Update settings display
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void RefreshSettingsData();

protected:
	// Settings categories
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TArray<FString> SettingsCategories;

	// Current volume settings
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	float MasterVolume;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	float MusicVolume;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	float SFXVolume;

	// Graphics settings
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	int32 QualityLevel;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	bool bFullscreen;

	// Audio Manager reference
	UPROPERTY()
	class UProjectVisibleAudioManager* AudioManager = nullptr;

private:
	struct FSettingsButton
	{
		FString Text;
		FVector2D Position;
		FVector2D Size;
		FLinearColor Color;
		int32 ButtonID;

		FSettingsButton() {}
		FSettingsButton(const FString& InText, const FVector2D& InPos, const FVector2D& InSize, const FLinearColor& InColor, int32 InID)
			: Text(InText), Position(InPos), Size(InSize), Color(InColor), ButtonID(InID) {}
	};

	void DrawSettingsInterface(FSlateWindowElementList& OutDrawElements, const FGeometry& AllottedGeometry, int32 LayerId, const FVector2D& PanelPos, const FVector2D& PanelSize) const;
	void InitializeSettingsButtons();
	void HandleSettingsButtonClick(int32 ButtonID);
	FSettingsButton* GetSettingsButtonAtPosition(const FVector2D& LocalPosition, const FVector2D& PanelPos, const FVector2D& PanelSize);

	TArray<FSettingsButton> SettingsButtons;
	int32 SelectedCategoryIndex;

	// Settings functions
	void ToggleFullscreen();
	void AdjustMasterVolume(float Delta);
	void AdjustMusicVolume(float Delta);
	void AdjustSFXVolume(float Delta);
	void ChangeQualityLevel(int32 NewLevel);
	void ResetToDefaults();
	void ApplySettings();
	void CloseSettings();
};
