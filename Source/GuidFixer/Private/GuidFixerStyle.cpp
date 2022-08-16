// Copyright Epic Games, Inc. All Rights Reserved.

#include "GuidFixerStyle.h"
#include "GuidFixer.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FGuidFixerStyle::StyleInstance = nullptr;

void FGuidFixerStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FGuidFixerStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FGuidFixerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("GuidFixerStyle"));
	return StyleSetName;
}


const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon64x64(64.0f, 64.0f);

TSharedRef< FSlateStyleSet > FGuidFixerStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("GuidFixerStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("GuidFixer")->GetBaseDir() / TEXT("Resources"));

	Style->Set("GuidFixer.FixMaterialGuids", new IMAGE_BRUSH(TEXT("MaterialButtonIcon_64x"), Icon64x64));
	Style->Set("GuidFixer.FixTextureGuids", new IMAGE_BRUSH(TEXT("TextureButtonIcon_40x"), Icon40x40));
	Style->Set("GuidFixer.FixEngineTextureGuids", new IMAGE_BRUSH(TEXT("EngineTextureButtonIcon_40x"), Icon40x40));
	return Style;
}

void FGuidFixerStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FGuidFixerStyle::Get()
{
	return *StyleInstance;
}
