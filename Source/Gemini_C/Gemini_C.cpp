// Project Visible - Landscape of Mind
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Gemini_C.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogProjectVisible);

IMPLEMENT_PRIMARY_GAME_MODULE(FProjectVisibleModule, Gemini_C, "Gemini_C");

void FProjectVisibleModule::StartupModule()
{
	UE_LOG(LogProjectVisible, Log, TEXT("Project Visible module starting up..."));
	InitializeProjectSystems();
}

void FProjectVisibleModule::ShutdownModule()
{
	UE_LOG(LogProjectVisible, Log, TEXT("Project Visible module shutting down..."));
	ShutdownProjectSystems();
}

void FProjectVisibleModule::InitializeProjectSystems()
{
	// Initialize core systems for Project Visible
	UE_LOG(LogProjectVisible, Log, TEXT("Initializing Project Visible core systems..."));
	
	// TODO: Initialize subsystems when they are created
}

void FProjectVisibleModule::ShutdownProjectSystems()
{
	// Cleanup Project Visible systems
	UE_LOG(LogProjectVisible, Log, TEXT("Shutting down Project Visible core systems..."));
}
 