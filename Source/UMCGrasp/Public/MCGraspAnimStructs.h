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

	/** Temporary array of local-space (relative to parent bone) rotation for each bone. 
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

//This structure represents one Episode with all their finger datas
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

	//map with all data for all fingers
	UPROPERTY(EditAnywhere)
	TMap<FString, FMCBoneData> BonesData;

	//Checks for equality
	FORCEINLINE bool operator==(const FMCFrame &arg1) const
	{
		//Go through all FingerDatas and check for equality
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
	
	//Constructor to set all finger datas directly
	FMCFrame(const TMap<FString, FMCBoneData>& Map)
	{
		BonesData = Map;
	}

	//function to set all finger data
	void SetAllData(const TMap<FString, FMCBoneData> & NewMap)
	{
		BonesData = NewMap;
	}

	//sets one data for a finger
	void AddNewBoneData(const FString& Name, const FMCBoneData& Data)
	{
		BonesData.Add(Name, Data);
	}

	//returns one finger data
	FMCBoneData* GetBoneData(const FString& Name)
	{
		return BonesData.Find(Name);
	}

	//Function that returns the complete map with all fingers and their datas.
	TMap<FString, FMCBoneData>* GetMap()
	{
		return &BonesData;
	}
};

//This struct represents one Animation. It holds the data for every episode 
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

	//All episodes
	TArray<FMCFrame> Frames;

	//Standard constructor
	FMCAnimationData()
	{
		Frames = TArray<FMCFrame>();
		BoneNames = TArray<FString>();
	}

	//adds a new episode
	void AddNewPositionData(const FMCFrame& Data)
	{
		Frames.Add(Data);
	}

	
	//returns the number of episodes in this animation
	int GetNumberOfEpisodes()
	{
		return Frames.Num();
	}

	//replaces one Episode with another one
	bool ReplaceEpisode(const FMCFrame& OldData, const FMCFrame& NewData)
	{
		int32 Index = RemoveEpisode(OldData);
		if (Index < 0) return false;
		int32 IndexNew = Frames.Insert(NewData, Index);

		//Checks if it was added to the right position
		return Index == IndexNew;
	}

	//replaces one episode with another one. He also creates a new struct out of the map
	bool ReplaceEpisode(const int32& RemoveIndex, const TMap<FString, FMCBoneData>& BoneData)
	{
		//Checks if the index is valid and also if the number of bones that are saved in BoneFingerNames
		//are equal with the new map (if not then there are missing bones)
		if (RemoveIndex < GetNumberOfEpisodes() && BoneData.Num() != BoneNames.Num()) return false;
		Frames.RemoveAt(RemoveIndex);
		return AddOneEpisode(BoneData, RemoveIndex);
	}

	//removes one episode
	int32 RemoveEpisode(const FMCFrame& OldData)
	{
		//Checks if the data exists 
		if (!Frames.Contains(OldData)) return -1;

		int32 Index = Frames.IndexOfByKey(OldData);
		Frames.RemoveAt(Index);
		return Index;
	}

	//returns one episode for a specific index
	FMCFrame GetPositionDataWithIndex(const int& Index)
	{
		if (Index >= Frames.Num() || Index < 0) return FMCFrame();
		return Frames[Index];
	}

	//Adds a new episode
	bool AddNewEpisode(const TMap<FString, FMCBoneData>& BoneData)
	{
		/*
		checks if the number of bones that are saved in BoneFingerNames
		are equal with the new map (if not then there are missing bones)
		*/
		if (BoneData.Num() != BoneNames.Num()) return false;
		return AddOneEpisode(BoneData, -1);
	}

	//returns all used bone names for this animation
	TArray<FString> GetAllBoneNames()
	{
		return BoneNames;
	}

	//helper function to add a new episode
	bool AddOneEpisode(const TMap<FString, FMCBoneData>& BoneData, const int32& Index)
	{
		//Create a new episode for all fingers
		FMCFrame EpisodeData = FMCFrame();
		EpisodeData.SetAllData(BoneData);

		//If the Index is not -1 insert this episode to this position
		if (Index >= 0 && Index <= Frames.Num())
		{
			Frames.Insert(EpisodeData,Index);
		}
		else
		{
			//Add the episode to the end
			Frames.Add(EpisodeData);
		}
		return true;
	}
};