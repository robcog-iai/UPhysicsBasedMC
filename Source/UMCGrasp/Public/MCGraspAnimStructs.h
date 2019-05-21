// Copyright 2017-2019, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "MCGraspAnimStructs.generated.h"

//This Struct represents one bone with all their values
USTRUCT()
struct FMCGraspAnimBoneData
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
	FMCGraspAnimBoneData() {}

	// Ctor with AngularOrientationTarget
	FMCGraspAnimBoneData(const FRotator& InAngularDriveInput) : AngularOrientationTarget(InAngularDriveInput)
	{}

	// Ctor with 
	FMCGraspAnimBoneData(const FRotator& InAngularDriveInput, const FRotator& InBoneSpace)
		: AngularOrientationTarget(InAngularDriveInput), BoneSpaceRotation(InBoneSpace)
	{}

	//Create the operator == to check for equality
	FORCEINLINE bool operator==(const FMCGraspAnimBoneData &arg1) const
	{
		//Checks if the values are equal
		return (arg1.BoneSpaceRotation == BoneSpaceRotation && arg1.AngularOrientationTarget == AngularOrientationTarget);
	}
};

//This structure represents one frame with all their bone data
USTRUCT()
struct FMCGraspAnimFrame
{
	GENERATED_USTRUCT_BODY()
public:
	//Standard constructor
	FMCGraspAnimFrame()
	{
		BonesData = TMap<FString, FMCGraspAnimBoneData>();
	}

	//map with all data for all bones
	UPROPERTY(EditAnywhere)
	TMap<FString, FMCGraspAnimBoneData> BonesData;

	//Checks for equality
	FORCEINLINE bool operator==(const FMCGraspAnimFrame &arg1) const
	{
		//Go through all BoneDatas and check for equality
		for (auto Elem : BonesData)
		{
			//Checks if the other struct contains data for the current key
			bool bContains = arg1.BonesData.Contains(Elem.Key);
			if (!bContains) return false;

			//Checks for equality
			const FMCGraspAnimBoneData ConstData = Elem.Value;
			bContains = *arg1.BonesData.Find(Elem.Key) == ConstData;
			if (!bContains) return false;
		}
		return true;
	}
	
	//Constructor to set all bone datas directly
	FMCGraspAnimFrame(const TMap<FString, FMCGraspAnimBoneData>& Map)
	{
		BonesData = Map;
	}

	//function to set all bone data
	void SetAllData(const TMap<FString, FMCGraspAnimBoneData> & NewMap)
	{
		BonesData = NewMap;
	}

	//sets one data for a bone
	void AddNewBoneData(const FString& Name, const FMCGraspAnimBoneData& Data)
	{
		BonesData.Add(Name, Data);
	}

	//returns one bone data
	FMCGraspAnimBoneData* GetBoneData(const FString& Name)
	{
		return BonesData.Find(Name);
	}

	//Function that returns the complete map with all bones and their data
	TMap<FString, FMCGraspAnimBoneData>* GetMap()
	{
		return &BonesData;
	}
};

//This struct represents one Animation. It holds the data for every frame 
//and also some general inforamtions
USTRUCT()
struct FMCGraspAnimData
{
	GENERATED_USTRUCT_BODY()
public:

	//The name for this Animation
	FString AnimationName;

	//All Bone informations
	TArray<FString> BoneNames;

	//All frames
	TArray<FMCGraspAnimFrame> Frames;

	//Standard constructor
	FMCGraspAnimData()
	{
		Frames = TArray<FMCGraspAnimFrame>();
		BoneNames = TArray<FString>();
	}

	//adds a new frame
	void AddNewFrame(const FMCGraspAnimFrame& InFrame)
	{
		Frames.Add(InFrame);
	}
	
	//returns the number of frames in this animation
	int32 FramesNum() const
	{
		return Frames.Num();
	}

	// Replaces frames
	bool ReplaceFrame(const FMCGraspAnimFrame& OldFrame, const FMCGraspAnimFrame& NewFrame)
	{
		int32 Index = RemoveFrame(OldFrame);
		if (Index < 0) return false;
		int32 IndexNew = Frames.Insert(NewFrame, Index);

		//Checks if it was added to the right position
		return Index == IndexNew;
	}

	// Replaces frames. He also creates a new struct out of the map
	bool ReplaceFrame(const int32& RemoveIndex, const TMap<FString, FMCGraspAnimBoneData>& BoneData)
	{
		//Checks if the index is valid and also if the number of bones that are saved in BoneNames
		//are equal with the new map (if not then there are missing bones)
		if (RemoveIndex < FramesNum() && BoneData.Num() != BoneNames.Num()) return false;
		Frames.RemoveAt(RemoveIndex);
		return AddOneFrame(BoneData, RemoveIndex);
	}

	//removes one frame
	int32 RemoveFrame(const FMCGraspAnimFrame& OldData)
	{
		//Checks if the data exists 
		if (!Frames.Contains(OldData)) return -1;

		int32 Index = Frames.IndexOfByKey(OldData);
		Frames.RemoveAt(Index);
		return Index;
	}

	//returns one frame for a specific index
	FMCGraspAnimFrame GetPositionDataWithIndex(const int& Index)
	{
		if (Index >= Frames.Num() || Index < 0) return FMCGraspAnimFrame();
		return Frames[Index];
	}

	//Adds a new frame
	bool AddNewFrame(const TMap<FString, FMCGraspAnimBoneData>& BoneData)
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
	bool AddOneFrame(const TMap<FString, FMCGraspAnimBoneData>& BoneData, const int32& Index)
	{
		//Create a new frame for all bones
		FMCGraspAnimFrame FrameData = FMCGraspAnimFrame();
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