#include "ProjectVisibleSettingsWidget.h"
#include "ProjectVisibleUIManager.h"
#include "ProjectVisibleAudioManager.h"
#include "Gemini_C.h"
#include "Rendering/DrawElements.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameUserSettings.h"

void UProjectVisibleSettingsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	// Initialize default values
	MasterVolume = 0.8f;
	MusicVolume = 0.7f;
	SFXVolume = 0.9f;
	QualityLevel = 2; // Medium
	bFullscreen = false;
	SelectedCategoryIndex = 0;
}

void UProjectVisibleSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Set as semi-transparent overlay
	SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));
	SetRenderOpacity(0.9f);
	
	// Get Audio Manager
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			AudioManager = GameInstance->GetSubsystem<UProjectVisibleAudioManager>();
		}
	}
	
	// Initialize settings values from AudioManager
	if (AudioManager)
	{
		// Initialize audio manager if needed
		AudioManager->InitializeAudioSystem();
		
		// Load current audio settings
		MasterVolume = AudioManager->GetChannelVolume(EAudioChannelType::Master);
		MusicVolume = AudioManager->GetChannelVolume(EAudioChannelType::Music);
		SFXVolume = AudioManager->GetChannelVolume(EAudioChannelType::SFX);
	}
	else
	{
		UE_LOG(LogProjectVisible, Warning, TEXT("AudioManager not available, using default values"));
	}
	
	UE_LOG(LogProjectVisible, Warning, TEXT("Settings Widget constructed with Audio Manager"));
	
	// Show on screen debug message
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Settings Menu Opened!"));
	}
}

int32 UProjectVisibleSettingsWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Draw background overlay
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor(0.0f, 0.0f, 0.0f, 0.8f)
	);

	// Calculate center panel
	FVector2D ScreenSize = AllottedGeometry.GetLocalSize();
	FVector2D PanelSize = FVector2D(600.0f, 500.0f);
	FVector2D PanelPos = (ScreenSize - PanelSize) * 0.5f;

	// Draw main panel background
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(PanelSize, FSlateLayoutTransform(PanelPos)),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor(0.15f, 0.15f, 0.2f, 0.95f)
	);

	// Draw panel border
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(PanelSize, FSlateLayoutTransform(PanelPos)),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor(0.4f, 0.6f, 0.8f, 1.0f)
	);

	// Draw settings interface
	DrawSettingsInterface(OutDrawElements, AllottedGeometry, LayerId, PanelPos, PanelSize);

	return LayerId;
}

FReply UProjectVisibleSettingsWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		FVector2D ScreenSize = InGeometry.GetLocalSize();
		FVector2D PanelSize = FVector2D(600.0f, 500.0f);
		FVector2D PanelPos = (ScreenSize - PanelSize) * 0.5f;

		// Check if clicked inside the panel
		if (LocalPosition.X >= PanelPos.X && LocalPosition.X <= PanelPos.X + PanelSize.X &&
			LocalPosition.Y >= PanelPos.Y && LocalPosition.Y <= PanelPos.Y + PanelSize.Y)
		{
			// Check button clicks
			FSettingsButton* ClickedButton = GetSettingsButtonAtPosition(LocalPosition, PanelPos, PanelSize);
			if (ClickedButton)
			{
				HandleSettingsButtonClick(ClickedButton->ButtonID);
				return FReply::Handled();
			}
		}
		else
		{
			// Clicked outside panel - close settings
			CloseSettings();
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

void UProjectVisibleSettingsWidget::InitializeWidget()
{
	Super::InitializeWidget();
	InitializeSettingsButtons();
	RefreshSettingsData();
}

void UProjectVisibleSettingsWidget::RefreshSettingsData()
{
	// Load current game settings
	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		QualityLevel = GameSettings->GetOverallScalabilityLevel();
		bFullscreen = GameSettings->GetFullscreenMode() == EWindowMode::Fullscreen;
	}

	UE_LOG(LogProjectVisible, Log, TEXT("Settings data refreshed"));
}

void UProjectVisibleSettingsWidget::InitializeSettingsButtons()
{
	SettingsButtons.Empty();

	// Button settings
	const float ButtonWidth = 150.0f;
	const float ButtonHeight = 30.0f;
	const float ButtonSpacing = 40.0f;
	const float StartY = 80.0f;
	const float Column1X = 50.0f;
	const float Column2X = 250.0f;
	const float Column3X = 450.0f;

	// Audio settings
	SettingsButtons.Add(FSettingsButton(TEXT("Master Vol -"), FVector2D(Column1X, StartY), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.7f, 0.3f, 0.3f, 0.9f), 1));
	SettingsButtons.Add(FSettingsButton(TEXT("Master Vol +"), FVector2D(Column1X, StartY + ButtonSpacing), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.3f, 0.7f, 0.3f, 0.9f), 2));
	SettingsButtons.Add(FSettingsButton(TEXT("Music Vol -"), FVector2D(Column1X, StartY + ButtonSpacing * 2), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.7f, 0.3f, 0.3f, 0.9f), 3));
	SettingsButtons.Add(FSettingsButton(TEXT("Music Vol +"), FVector2D(Column1X, StartY + ButtonSpacing * 3), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.3f, 0.7f, 0.3f, 0.9f), 4));

	// Graphics settings
	SettingsButtons.Add(FSettingsButton(TEXT("Quality Low"), FVector2D(Column2X, StartY), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.5f, 0.5f, 0.3f, 0.9f), 5));
	SettingsButtons.Add(FSettingsButton(TEXT("Quality Med"), FVector2D(Column2X, StartY + ButtonSpacing), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.6f, 0.6f, 0.3f, 0.9f), 6));
	SettingsButtons.Add(FSettingsButton(TEXT("Quality High"), FVector2D(Column2X, StartY + ButtonSpacing * 2), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.7f, 0.7f, 0.3f, 0.9f), 7));
	SettingsButtons.Add(FSettingsButton(TEXT("Toggle Fullscreen"), FVector2D(Column2X, StartY + ButtonSpacing * 3), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.4f, 0.4f, 0.7f, 0.9f), 8));

	// Control buttons
	SettingsButtons.Add(FSettingsButton(TEXT("Reset Defaults"), FVector2D(Column3X, StartY), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.8f, 0.6f, 0.2f, 0.9f), 9));
	SettingsButtons.Add(FSettingsButton(TEXT("Apply Settings"), FVector2D(Column3X, StartY + ButtonSpacing), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.2f, 0.8f, 0.2f, 0.9f), 10));
	SettingsButtons.Add(FSettingsButton(TEXT("Close Settings"), FVector2D(Column3X, StartY + ButtonSpacing * 2), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.7f, 0.3f, 0.7f, 0.9f), 11));

	UE_LOG(LogProjectVisible, Log, TEXT("Settings buttons initialized: %d buttons"), SettingsButtons.Num());
}

void UProjectVisibleSettingsWidget::DrawSettingsInterface(FSlateWindowElementList& OutDrawElements, const FGeometry& AllottedGeometry, int32 LayerId, const FVector2D& PanelPos, const FVector2D& PanelSize) const
{
	// Draw title
	FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 20);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(FVector2D(PanelSize.X, 50.0f), FSlateLayoutTransform(PanelPos + FVector2D(20.0f, 15.0f))),
		FText::FromString(TEXT("Project Visible - Settings")),
		TitleFont,
		ESlateDrawEffect::None,
		FLinearColor::White
	);

	// Draw current values
	FSlateFontInfo ValueFont = FCoreStyle::GetDefaultFontStyle("Regular", 14);
	
	// Master Volume
	FString MasterVolText = FString::Printf(TEXT("Master Volume: %.0f%%"), MasterVolume * 100.0f);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(200.0f, 20.0f), FSlateLayoutTransform(PanelPos + FVector2D(50.0f, 50.0f))),
		FText::FromString(MasterVolText),
		ValueFont,
		ESlateDrawEffect::None,
		FLinearColor(0.0f, 1.0f, 1.0f, 1.0f)
	);

	// Music Volume
	FString MusicVolText = FString::Printf(TEXT("Music Volume: %.0f%%"), MusicVolume * 100.0f);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(200.0f, 20.0f), FSlateLayoutTransform(PanelPos + FVector2D(250.0f, 50.0f))),
		FText::FromString(MusicVolText),
		ValueFont,
		ESlateDrawEffect::None,
		FLinearColor(0.0f, 1.0f, 1.0f, 1.0f)
	);

	// Quality Level
	FString QualityNames[] = { TEXT("Low"), TEXT("Medium"), TEXT("High"), TEXT("Epic") };
	FString QualityText = FString::Printf(TEXT("Quality: %s"), QualityLevel < 4 ? *QualityNames[QualityLevel] : TEXT("Custom"));
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(200.0f, 20.0f), FSlateLayoutTransform(PanelPos + FVector2D(450.0f, 50.0f))),
		FText::FromString(QualityText),
		ValueFont,
		ESlateDrawEffect::None,
		FLinearColor(0.0f, 1.0f, 1.0f, 1.0f)
	);

	// Draw each settings button
	for (const FSettingsButton& Button : SettingsButtons)
	{
		FVector2D ButtonPos = PanelPos + Button.Position;

		// Button background
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId++,
			AllottedGeometry.ToPaintGeometry(Button.Size, FSlateLayoutTransform(ButtonPos)),
			FCoreStyle::Get().GetBrush("WhiteBrush"),
			ESlateDrawEffect::None,
			Button.Color
		);

		// Button text
		FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Regular", 12);
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(Button.Size, FSlateLayoutTransform(ButtonPos + FVector2D(5.0f, 8.0f))),
			FText::FromString(Button.Text),
			ButtonFont,
			ESlateDrawEffect::None,
			FLinearColor::White
		);
	}
}

void UProjectVisibleSettingsWidget::HandleSettingsButtonClick(int32 ButtonID)
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Settings button clicked: %d"), ButtonID);

	switch (ButtonID)
	{
		case 1: // Master Vol -
			AdjustMasterVolume(-0.1f);
			break;
		case 2: // Master Vol +
			AdjustMasterVolume(0.1f);
			break;
		case 3: // Music Vol -
			AdjustMusicVolume(-0.1f);
			break;
		case 4: // Music Vol +
			AdjustMusicVolume(0.1f);
			break;
		case 5: // Quality Low
			ChangeQualityLevel(0);
			break;
		case 6: // Quality Med
			ChangeQualityLevel(1);
			break;
		case 7: // Quality High
			ChangeQualityLevel(2);
			break;
		case 8: // Toggle Fullscreen
			ToggleFullscreen();
			break;
		case 9: // Reset Defaults
			ResetToDefaults();
			break;
		case 10: // Apply Settings
			ApplySettings();
			break;
		case 11: // Close Settings
			CloseSettings();
			break;
		default:
			UE_LOG(LogProjectVisible, Warning, TEXT("Unknown settings button ID: %d"), ButtonID);
			break;
	}

	// Show feedback
	if (GEngine)
	{
		FString ButtonName = (ButtonID >= 1 && ButtonID <= SettingsButtons.Num()) ? SettingsButtons[ButtonID - 1].Text : TEXT("Unknown");
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Settings: %s"), *ButtonName));
	}
}

UProjectVisibleSettingsWidget::FSettingsButton* UProjectVisibleSettingsWidget::GetSettingsButtonAtPosition(const FVector2D& LocalPosition, const FVector2D& PanelPos, const FVector2D& PanelSize)
{
	for (FSettingsButton& Button : SettingsButtons)
	{
		FVector2D ButtonPos = PanelPos + Button.Position;
		if (LocalPosition.X >= ButtonPos.X && LocalPosition.X <= ButtonPos.X + Button.Size.X &&
			LocalPosition.Y >= ButtonPos.Y && LocalPosition.Y <= ButtonPos.Y + Button.Size.Y)
		{
			return &Button;
		}
	}
	return nullptr;
}

void UProjectVisibleSettingsWidget::ToggleFullscreen()
{
	bFullscreen = !bFullscreen;
	
	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		EWindowMode::Type NewMode = bFullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed;
		GameSettings->SetFullscreenMode(NewMode);
		GameSettings->RequestResolutionChange(GameSettings->GetDesktopResolution().X, GameSettings->GetDesktopResolution().Y, NewMode);
	}
	
	UE_LOG(LogProjectVisible, Warning, TEXT("Fullscreen toggled: %s"), bFullscreen ? TEXT("ON") : TEXT("OFF"));
}

void UProjectVisibleSettingsWidget::AdjustMasterVolume(float Delta)
{
	MasterVolume = FMath::Clamp(MasterVolume + Delta, 0.0f, 1.0f);
	
	// Apply to Audio Manager
	if (AudioManager)
	{
		AudioManager->SetChannelVolume(EAudioChannelType::Master, MasterVolume);
		AudioManager->TestPlayButtonSound(); // Test sound
	}
	
	UE_LOG(LogProjectVisible, Warning, TEXT("Master Volume: %.2f"), MasterVolume);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
			FString::Printf(TEXT("Master Volume: %.0f%%"), MasterVolume * 100.0f));
	}
}

void UProjectVisibleSettingsWidget::AdjustMusicVolume(float Delta)
{
	MusicVolume = FMath::Clamp(MusicVolume + Delta, 0.0f, 1.0f);
	
	// Apply to Audio Manager
	if (AudioManager)
	{
		AudioManager->SetChannelVolume(EAudioChannelType::Music, MusicVolume);
		AudioManager->TestPlayNotificationSound(); // Test sound
	}
	
	UE_LOG(LogProjectVisible, Warning, TEXT("Music Volume: %.2f"), MusicVolume);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue,
			FString::Printf(TEXT("Music Volume: %.0f%%"), MusicVolume * 100.0f));
	}
}

void UProjectVisibleSettingsWidget::AdjustSFXVolume(float Delta)
{
	SFXVolume = FMath::Clamp(SFXVolume + Delta, 0.0f, 1.0f);
	
	// Apply to Audio Manager
	if (AudioManager)
	{
		AudioManager->SetChannelVolume(EAudioChannelType::SFX, SFXVolume);
		AudioManager->PlaySFXSound(TEXT("VolumeTest"), 1.0f); // Test sound
	}
	
	UE_LOG(LogProjectVisible, Warning, TEXT("SFX Volume: %.2f"), SFXVolume);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange,
			FString::Printf(TEXT("SFX Volume: %.0f%%"), SFXVolume * 100.0f));
	}
}

void UProjectVisibleSettingsWidget::ChangeQualityLevel(int32 NewLevel)
{
	QualityLevel = FMath::Clamp(NewLevel, 0, 3);
	
	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetOverallScalabilityLevel(QualityLevel);
	}
	
	UE_LOG(LogProjectVisible, Warning, TEXT("Quality Level changed to: %d"), QualityLevel);
}

void UProjectVisibleSettingsWidget::ResetToDefaults()
{
	MasterVolume = 0.8f;
	MusicVolume = 0.7f;
	SFXVolume = 0.9f;
	QualityLevel = 1; // Medium
	bFullscreen = false;
	
	// Reset graphics settings
	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->SetToDefaults();
		GameSettings->SetOverallScalabilityLevel(QualityLevel);
		GameSettings->SetFullscreenMode(EWindowMode::Windowed);
	}
	
	// Reset audio settings
	if (AudioManager)
	{
		AudioManager->ResetAudioSettings();
		
		// Update local values to match reset values
		MasterVolume = AudioManager->GetChannelVolume(EAudioChannelType::Master);
		MusicVolume = AudioManager->GetChannelVolume(EAudioChannelType::Music);
		SFXVolume = AudioManager->GetChannelVolume(EAudioChannelType::SFX);
	}
	
	UE_LOG(LogProjectVisible, Warning, TEXT("Settings reset to defaults"));
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("Settings Reset to Defaults!"));
	}
}

void UProjectVisibleSettingsWidget::ApplySettings()
{
	// Apply graphics settings
	if (UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameSettings->ApplySettings(false);
		GameSettings->SaveSettings();
	}
	
	// Apply audio settings
	if (AudioManager)
	{
		AudioManager->SaveAudioSettings();
		AudioManager->TestPlayButtonSound(); // Confirmation sound
	}
	
	UE_LOG(LogProjectVisible, Warning, TEXT("Settings applied and saved"));
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Settings Applied Successfully!"));
	}
}

void UProjectVisibleSettingsWidget::CloseSettings()
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Closing Settings menu"));
	
	// Get UI Manager and close this modal
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
		if (UProjectVisibleUIManager* UIManagerSystem = GameInstance->GetSubsystem<UProjectVisibleUIManager>())
		{
			UIManagerSystem->ClearAllModals();
				
				// Restore game input
				if (APlayerController* PC = World->GetFirstPlayerController())
				{
					FInputModeGameOnly InputMode;
					PC->SetInputMode(InputMode);
					PC->SetShowMouseCursor(false);
				}
			}
		}
	}
}
