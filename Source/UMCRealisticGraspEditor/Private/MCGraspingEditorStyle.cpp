// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "MCGraspingEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "SlateGameResources.h"
#include "IPluginManager.h"

TSharedPtr<FSlateStyleSet> UMCGraspingEditorStyle::StyleInstance = NULL;

void UMCGraspingEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		return;
	}
}
void UMCGraspingEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName UMCGraspingEditorStyle::GetStyleSetName()
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

TSharedRef<FSlateStyleSet> UMCGraspingEditorStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("GraspingStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("URealisticGrasping")->GetBaseDir() / TEXT("Resources"));

	Style->Set("GraspingEditor.ShowInstructions", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), Icon40x40));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT


void UMCGraspingEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& UMCGraspingEditorStyle::Get()
{
	return *StyleInstance;
}