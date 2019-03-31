// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "MCGraspAnimStructs.generated.h"

//This Struct represents one bone with all their values
USTRUCT()
struct FMCBoneData
{
	GENERATED_BODY()

	/** Input value for a constraints angular drive motor.*/
	UPROPERTY(EditAnywhere)
	FRotator AngularOrientationTarget;

	/** Array of local-space (relative to parent bone) rotation for each bone. 
	Is used to load Animation back into editor.*/
	UPROPERTY(EditAnywhere)
	FRotator BoneSpaceRotation;

	// Default ctor
	FMCBoneData() {}

	// Ctor with AngularOrientationTarget
	FMCBoneData(const FRotator& InAngularDriveInput) : AngularOrientationTarget(InAngularDriveInput)
	{}

	// Ctor with 
	FMCBoneData(const FRotator& InAngularDriveInput, const FRotator& InBoneSpace)
		: AngularOrientationTarget(InAngularDriveInput), BoneSpaceRotation(InBoneSpace)
	{}

	//Create the operator == to check for equality
	FORCEINLINE bool operator==(const FMCBoneData &arg1) const
	{
		//Checks if the values are equal
		return (arg1.BoneSpaceRotation == BoneSpaceRotation && arg1.AngularOrientationTarget == AngularOrientationTarget);
	}
};

//This structure represents one frame with all their bone data
USTRUCT()
struct FMCFrame
{
	GENERATED_USTRUCT_BODY()
public:
	//Standard constructor
	FMCFrame()
	{
		BonesData = TMap<FString, FMCBoneData>();
	}

	//map with all data for all bones
	UPROPERTY(EditAnywhere)
	TMap<FString, FMCBoneData> BonesData;

	//Checks for equality
	FORCEINLINE bool operator==(const FMCFrame &arg1) const
	{
		//Go through all BoneDatas and check for equality
		for (auto Elem : BonesData)
		{
			//Checks if the other struct contains data for the current key
			bool bContains = arg1.BonesData.Contains(Elem.Key);
			if (!bContains) return false;

			//Checks for equality
			const FMCBoneData ConstData = Elem.Value;
			bContains = *arg1.BonesData.Find(Elem.Key) == ConstData;
			if (!bContains) return false;
		}
		return true;
	}
	
	//Constructor to set all bone datas directly
	FMCFrame(const TMap<FString, FMCBoneData>& Map)
	{
		BonesData = Map;
	}

	//function to set all bone data
	void SetAllData(const TMap<FString, FMCBoneData> & NewMap)
	{
		BonesData = NewMap;
	}

	//sets one data for a bone
	void AddNewBoneData(const FString& Name, const FMCBoneData& Data)
	{
		BonesData.Add(Name, Data);
	}

	//returns one bone data
	FMCBoneData* GetBoneData(const FString& Name)
	{
		return BonesData.Find(Name);
	}

	//Function that returns the complete map with all bones and their data
	TMap<FString, FMCBoneData>* GetMap()
	{
		return &BonesData;
	}
};

//This struct represents one Animation. It holds the data for every frame 
//and also some general inforamtions
USTRUCT()
struct FMCAnimationData
{
	GENERATED_USTRUCT_BODY()
public:

	//The name for this Animation
	FString AnimationName;

	//All Bone informations
	TArray<FString> BoneNames;

	//All frames
	TArray<FMCFrame> Frames;

	//Standard constructor
	FMCAnimationData()
	{
		Frames = TArray<FMCFrame>();
		BoneNames = TArray<FString>();
	}

	//adds a new frame
	void AddNewPositionData(const FMCFrame& Data)
	{
		Frames.Add(Data);
	}
	
	//returns the number of frames in this animation
	int GetNumberOfFrames()
	{
		return Frames.Num();
	}

	//replaces one frame with another one
	bool ReplaceFrame(const FMCFrame& OldData, const FMCFrame& NewData)
	{
		int32 Index = RemoveFrame(OldData);
		if (Index < 0) return false;
		int32 IndexNew = Frames.Insert(NewData, Index);

		//Checks if it was added to the right position
		return Index == IndexNew;
	}

	//replaces one frame with another one. He also creates a new struct out of the map
	bool ReplaceFrame(const int32& RemoveIndex, const TMap<FString, FMCBoneData>& BoneData)
	{
		//Checks if the index is valid and also if the number of bones that are saved in BoneNames
		//are equal with the new map (if not then there are missing bones)
		if (RemoveIndex < GetNumberOfFrames() && BoneData.Num() != BoneNames.Num()) return false;
		Frames.RemoveAt(RemoveIndex);
		return AddOneFrame(BoneData, RemoveIndex);
	}

	//removes one frame
	int32 RemoveFrame(const FMCFrame& OldData)
	{
		//Checks if the data exists 
		if (!Frames.Contains(OldData)) return -1;

		int32 Index = Frames.IndexOfByKey(OldData);
		Frames.RemoveAt(Index);
		return Index;
	}

	//returns one frame for a specific index
	FMCFrame GetPositionDataWithIndex(const int& Index)
	{
		if (Index >= Frames.Num() || Index < 0) return FMCFrame();
		return Frames[Index];
	}

	//Adds a new frame
	bool AddNewFrame(const TMap<FString, FMCBoneData>& BoneData)
	{
		/*
		checks if the number of bones that are saved in BoneNames
		are equal with the new map (if not then there are missing bones)
		*/
		if (BoneData.Num() != BoneNames.Num()) return false;
		return AddOneFrame(BoneData, -1);
	}

	//returns all used bone names for this animation
	TArray<FString> GetAllBoneNames()
	{
		return BoneNames;
	}

	//helper function to add a new frame
	bool AddOneFrame(const TMap<FString, FMCBoneData>& BoneData, const int32& Index)
	{
		//Create a new frame for all bones
		FMCFrame FrameData = FMCFrame();
		FrameData.SetAllData(BoneData);

		//If the Index is not -1 insert this frame to this position
		if (Index >= 0 && Index <= Frames.Num())
		{
			Frames.Insert(FrameData,Index);
		}
		else
		{
			//Add the frame to the end
			Frames.Add(FrameData);
		}
		return true;
	}
};