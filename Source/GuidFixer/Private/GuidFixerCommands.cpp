// Copyright Epic Games, Inc. All Rights Reserved.

#include "GuidFixerCommands.h"

#define LOCTEXT_NAMESPACE "FGuidFixerModule"

void FGuidFixerCommands::RegisterCommands()
{
	UI_COMMAND(FixMaterialGuids, "Fix Material GUIDs",
	           "Fixes material lighting GUIDs so that there are no materials with empty or duplicate GUIDs.",
	           EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(FixTextureGuids, "Fix Texture GUIDs",
	           "Fixes texture lighting GUIDs so that there are no textures with empty or duplicate GUIDs.",
	           EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(FixEngineTextureGuids, "Fix Engine Texture GUIDs",
	           "Try this if you still have issues after fixing texture GUIDs.\n"
	           "Since this touches engine files, it will report making changes after each editor restart.",
	           EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
