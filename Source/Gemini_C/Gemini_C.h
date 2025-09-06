// Project Visible - Landscape of Mind
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogProjectVisible, Log, All);

class FProjectVisibleModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	void InitializeProjectSystems();
	void ShutdownProjectSystems();
};
