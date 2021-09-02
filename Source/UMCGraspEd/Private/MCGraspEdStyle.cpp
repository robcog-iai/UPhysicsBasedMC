// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspEdStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "SlateGameResources.h"
#include "IPluginManager.h"

TSharedPtr<FSlateStyleSet> UMCGraspEdStyle::StyleInstance = NULL;

void UMCGraspEdStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		return;
	}
}
void UMCGraspEdStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName UMCGraspEdStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("GraspingStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH(RelativePath, ...) FSlateBorderBrush(Style->RootToContentDir(RelativePath, TEXT(".png") ), __VA_ARGS__)
#define TTF_FONT(RelativePath, ...) FSlateFontInfo(Style->RootToContentDir(RelativePath, TEXT(".ttf")), __VA_ARGS__)
#define OTF_FONT(RelativePath, ...) FSlateFontInfo(Style->RootToContentDir(RelativePath, TEXT(".otf")), __VA_ARGS__)

const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef<FSlateStyleSet> UMCGraspEdStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("GraspingStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("UPhysicsBasedMC")->GetBaseDir() / TEXT("Resources"));

	Style->Set("GraspingEditor.ShowInstructions", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), Icon40x40));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT


void UMCGraspEdStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& UMCGraspEdStyle::Get()
{
	return *StyleInstance;
}