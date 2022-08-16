// Copyright Epic Games, Inc. All Rights Reserved.

#include "GuidFixer.h"

#include "GuidFixerStyle.h"
#include "GuidFixerCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName GuidFixerTabName("GuidFixer");

#define LOCTEXT_NAMESPACE "FGuidFixerModule"

void FGuidFixerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FGuidFixerStyle::Initialize();
	FGuidFixerStyle::ReloadTextures();

	FGuidFixerCommands::Register();

	FixMaterialGuidsCommands = MakeShareable(new FUICommandList);

	FixMaterialGuidsCommands->MapAction(
		FGuidFixerCommands::Get().FixMaterialGuids,
		FExecuteAction::CreateRaw(this, &FGuidFixerModule::FixMaterialGuids),
		FCanExecuteAction());

	FixTextureGuidsCommands = MakeShareable(new FUICommandList);

	FixTextureGuidsCommands->MapAction(
		FGuidFixerCommands::Get().FixTextureGuids,
		FExecuteAction::CreateRaw(this, &FGuidFixerModule::FixTextureGuids),
		FCanExecuteAction());

	FixEngineTextureGuidsCommands = MakeShareable(new FUICommandList);

	FixEngineTextureGuidsCommands->MapAction(
		FGuidFixerCommands::Get().FixEngineTextureGuids,
		FExecuteAction::CreateRaw(this, &FGuidFixerModule::FixEngineTextureGuids),
		FCanExecuteAction());
	
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FGuidFixerModule::RegisterMenus));
}

void FGuidFixerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FGuidFixerStyle::Shutdown();

	FGuidFixerCommands::Unregister();
}

void FGuidFixerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
	{
		FToolMenuSection& Section = Menu->AddSection("GuidFixer", LOCTEXT("GUID Fixer", "GUID Fixer"));
		Section.AddMenuEntryWithCommandList(FGuidFixerCommands::Get().FixMaterialGuids, FixMaterialGuidsCommands);
		Section.AddMenuEntryWithCommandList(FGuidFixerCommands::Get().FixTextureGuids, FixTextureGuidsCommands);
		Section.AddMenuEntryWithCommandList(FGuidFixerCommands::Get().FixEngineTextureGuids, FixEngineTextureGuidsCommands);
	}
}

// This function is a modified version of laggyluk's SwarmGuidFixer
// https://github.com/laggyluk/SwarmGuidFixer
void FGuidFixerModule::FixMaterialGuids() const
{
	TMap<FGuid, UMaterialInterface*> Guids;

	bool bUpdatedGuids = false;
	for (TObjectIterator<UMaterialInterface> Material; Material; ++Material)
	{
		const FGuid& LightingGuid = Material->GetLightingGuid();

		if (!LightingGuid.IsValid())
		{
			if (!IsDefaultObject(*Material->GetPathName()))
			{
				SetMaterialLightingGuid(*Material);
				bUpdatedGuids = true;
			}
			continue;
		}

		UMaterialInterface** Result = Guids.Find(LightingGuid);
		if (Result == nullptr)
		{
			Guids.Add(LightingGuid, *Material);
			continue;
		}

		// Update the initial material with the same GUID if its GUID hasn't been updated already
		if ((*Result)->GetLightingGuid() == LightingGuid && !IsDefaultObject((*Result)->GetPathName()))
		{
			SetMaterialLightingGuid(*Result);
			bUpdatedGuids = true;
		}

		if (!IsDefaultObject(*Material->GetPathName()))
		{
			SetMaterialLightingGuid(*Material);
			bUpdatedGuids = true;
		}
	}

	const FText DialogText = FText::FromString(!bUpdatedGuids
		                                           ? "No duplicate or empty material GUIDs found."
		                                           : "At least one material GUID has been changed.\n"
		                                           "Use \"Save All\" to save these changes.\n\n\n"
		                                           "Check the Output Log to see which materials have been updated.");
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

bool FGuidFixerModule::IsDefaultObject(FString Path) const
{
	return Path.StartsWith("/Engine/")
		|| Path.StartsWith("/Script/LevelSequence.Default__LevelSequenceMediaController");
}

void FGuidFixerModule::SetMaterialLightingGuid(UMaterialInterface* Material) const
{
	if (!Material)
	{
		return;
	}

	Material->SetLightingGuid();
	Material->Modify();
	UE_LOG(LogTemp, Display, TEXT("%s: Material lighting GUID updated."), *Material->GetPathName());
}

void FGuidFixerModule::FixTextureGuids() const
{
	TMap<FGuid, UTexture*> Guids;

	bool bUpdatedGuids = false;
	for (TObjectIterator<UTexture> Texture; Texture; ++Texture)
	{
		const FGuid& LightingGuid = Texture->GetLightingGuid();

		if (!LightingGuid.IsValid())
		{
			if (!IsDefaultObject(*Texture->GetPathName()))
			{
				SetTextureLightingGuid(*Texture);
				bUpdatedGuids = true;
			}
			continue;
		}

		UTexture** Result = Guids.Find(LightingGuid);
		if (Result == nullptr)
		{
			Guids.Add(LightingGuid, *Texture);
			continue;
		}

		// Update the initial material with the same GUID if its GUID hasn't been updated already
		if ((*Result)->GetLightingGuid() == LightingGuid && !IsDefaultObject((*Result)->GetPathName()))
		{
			SetTextureLightingGuid(*Result);
			bUpdatedGuids = true;
		}

		if (!IsDefaultObject(*Texture->GetPathName()))
		{
			SetTextureLightingGuid(*Texture);
			bUpdatedGuids = true;
		}
	}

	const FText DialogText = FText::FromString(!bUpdatedGuids
		                                           ? "No duplicate or empty texture GUIDs found."
		                                           : "At least one texture GUID has been changed.\n"
		                                           "Use \"Save All\" to save these changes.\n\n\n"
		                                           "Check the Output Log to see which textures have been updated.");
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FGuidFixerModule::SetTextureLightingGuid(UTexture* Texture) const
{
	Texture->SetLightingGuid();
	Texture->Modify();
	UE_LOG(LogTemp, Display, TEXT("%s: Texture lighting GUID updated."), *Texture->GetPathName());
}

void FGuidFixerModule::FixEngineTextureGuids() const
{
	TMap<FGuid, UTexture*> Guids;

	bool bUpdatedGuids = false;
	for (TObjectIterator<UTexture> Texture; Texture; ++Texture)
	{
		const FGuid& LightingGuid = Texture->GetLightingGuid();

		if (!LightingGuid.IsValid())
		{
			if (IsDefaultObject(*Texture->GetPathName()))
			{
				SetTextureLightingGuid(*Texture);
				bUpdatedGuids = true;
			}
			continue;
		}

		UTexture** Result = Guids.Find(LightingGuid);
		if (Result == nullptr)
		{
			Guids.Add(LightingGuid, *Texture);
			continue;
		}

		// Update the initial material with the same GUID if its GUID hasn't been updated already
		if ((*Result)->GetLightingGuid() == LightingGuid && IsDefaultObject((*Result)->GetPathName()))
		{
			SetTextureLightingGuid(*Result);
			bUpdatedGuids = true;
		}

		if (IsDefaultObject(*Texture->GetPathName()))
		{
			SetTextureLightingGuid(*Texture);
			bUpdatedGuids = true;
		}
	}

	const FText DialogText = FText::FromString(!bUpdatedGuids
												   ? "No duplicate or empty texture GUIDs found."
												   : "At least one texture GUID has been changed.\n"
												   "Use \"Save All\" to save these changes.\n\n\n"
												   "Check the Output Log to see which textures have been updated.");
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGuidFixerModule, GuidFixer)
