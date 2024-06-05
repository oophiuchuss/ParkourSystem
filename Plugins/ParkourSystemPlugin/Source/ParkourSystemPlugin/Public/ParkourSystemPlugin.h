// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FParkourSystemPluginModule : public IModuleInterface
{
private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
