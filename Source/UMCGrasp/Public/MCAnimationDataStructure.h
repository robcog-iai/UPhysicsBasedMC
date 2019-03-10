// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "MCAnimationDataStructure.generated.h"

//This Struct represents one bone with all their values
USTRUCT()
struct FMCBoneData
{
	GENERATED_BODY()

	// The bonespace rotation after the calculation
	FRotator AngularDriveInput;

	// The bonespace rotation before the calculation
	FRotator BoneSpace;

	// Default ctor
	FMCBoneData() {}

	// Ctor with AngularDriveInput
	FMCBoneData(const FRotator& InAngularDriveInput) : AngularDriveInput(InAngularDriveInput)
	{}

	// Ctor with 
	FMCBoneData(const FRotator& InAngularDriveInput, const FRotator& InBoneSpace)
		: AngularDriveInput(InAngularDriveInput), BoneSpace(InBoneSpace)
	{}

	//Create the operator == to check for equality
	FORCEINLINE bool operator==(const FMCBoneData &arg1) const
	{
		//Checks if the values are equal
		return (arg1.BoneSpace == BoneSpace && arg1.AngularDriveInput == AngularDriveInput);
	}
};

//This structure represents one Episode with all their finger datas
USTRUCT()
struct FMCEpisodeData
{
	GENERATED_USTRUCT_BODY()
public:
	//Standard constructor
	FMCEpisodeData()
	{
		PositionData = TMap<FString, FMCBoneData>();
	}

	//Checks for equality
	FORCEINLINE bool operator==(const FMCEpisodeData &arg1) const
	{
		//Go through all FingerDatas and check for equality
		for (auto Elem : PositionData)
		{
			//Checks if the other struct contains data for the current key
			bool bContains = arg1.PositionData.Contains(Elem.Key);
			if (!bContains) return false;

			//Checks for equality
			const FMCBoneData ConstData = Elem.Value;
			bContains = *arg1.PositionData.Find(Elem.Key) == ConstData;
			if (!bContains) return false;
		}
		return true;
	}
	
	//Constructor to set all finger datas directly
	FMCEpisodeData(const TMap<FString, FMCBoneData>& Map)
	{
		PositionData = Map;
	}

	//function to set all finger data
	void SetAllData(const TMap<FString, FMCBoneData> & NewMap)
	{
		PositionData = NewMap;
	}

	//sets one data for a finger
	void AddNewBoneData(const FString& Name, const FMCBoneData& Data)
	{
		PositionData.Add(Name, Data);
	}

	//returns one finger data
	FMCBoneData* GetBoneData(const FString& Name)
	{
		return PositionData.Find(Name);
	}

	//Function that returns the complete map with all fingers and their datas.
	TMap<FString, FMCBoneData>* GetMap()
	{
		return &PositionData;
	}

private:
	//map with all data for all fingers
	TMap<FString, FMCBoneData> PositionData;

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

	//The name for the used skeletal mesh 
	FString SkeletalName;

	//All Bone informations
	TArray<FString> BoneNames;

	//All episodes
	TArray<FMCEpisodeData> PositionEpisode;

	//Standard constructor
	FMCAnimationData()
	{
		PositionEpisode = TArray<FMCEpisodeData>();
		BoneNames = TArray<FString>();
	}

	//adds a new episode
	void AddNewPositionData(const FMCEpisodeData& Data)
	{
		PositionEpisode.Add(Data);
	}

	
	//returns the number of episodes in this animation
	int GetNumberOfEpisodes()
	{
		return PositionEpisode.Num();
	}

	//replaces one Episode with another one
	bool ReplaceEpisode(const FMCEpisodeData& OldData, const FMCEpisodeData& NewData)
	{
		int32 Index = RemoveEpisode(OldData);
		if (Index < 0) return false;
		int32 IndexNew = PositionEpisode.Insert(NewData, Index);

		//Checks if it was added to the right position
		return Index == IndexNew;
	}

	//replaces one episode with another one. He also creates a new struct out of the map
	bool ReplaceEpisode(const int32& RemoveIndex, const TMap<FString, FMCBoneData>& BoneData)
	{
		//Checks if the index is valid and also if the number of bones that are saved in BoneFingerNames
		//are equal with the new map (if not then there are missing bones)
		if (RemoveIndex < GetNumberOfEpisodes() && BoneData.Num() != BoneNames.Num()) return false;
		PositionEpisode.RemoveAt(RemoveIndex);
		return AddOneEpisode(BoneData, RemoveIndex);
	}

	//removes one episode
	int32 RemoveEpisode(const FMCEpisodeData& OldData)
	{
		//Checks if the data exists 
		if (!PositionEpisode.Contains(OldData)) return -1;

		int32 Index = PositionEpisode.IndexOfByKey(OldData);
		PositionEpisode.RemoveAt(Index);
		return Index;
	}

	//returns one episode for a specific index
	FMCEpisodeData GetPositionDataWithIndex(const int& Index)
	{
		if (Index >= PositionEpisode.Num() || Index < 0) return FMCEpisodeData();
		return PositionEpisode[Index];
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
		FMCEpisodeData EpisodeData = FMCEpisodeData();
		EpisodeData.SetAllData(BoneData);

		//If the Index is not -1 insert this episode to this position
		if (Index >= 0 && Index <= PositionEpisode.Num())
		{
			PositionEpisode.Insert(EpisodeData,Index);
		}
		else
		{
			//Add the episode to the end
			PositionEpisode.Add(EpisodeData);
		}
		return true;
	}
};