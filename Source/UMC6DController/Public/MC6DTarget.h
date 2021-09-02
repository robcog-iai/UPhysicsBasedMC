// Copyright 2017-present, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "MotionControllerComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Animation/SkeletalMeshActor.h"
#include "MC6DController.h"
#include "MC6DControlType.h"
#include "MC6DTarget.generated.h"

/**
* Hand type
*/
UENUM()
enum class EMC6DHandType : uint8
{
	Left					UMETA(DisplayName = "Left"),
	Right					UMETA(DisplayName = "Right"),
};

//#if WITH_EDITOR
UENUM()
enum class EMC6DTermSelection : uint8
{
	P						UMETA(DisplayName = "P"),
	I						UMETA(DisplayName = "I"),
	D						UMETA(DisplayName = "D"),
	Max						UMETA(DisplayName = "Max"),
	Delta					UMETA(DisplayName = "Delta"),
};

UENUM()
enum class EMC6DMovementTypeSelection : uint8
{
	Rot						UMETA(DisplayName = "Rot"),
	Loc						UMETA(DisplayName = "Loc"),
};
//#endif // WITH_EDITOR

// #if UMC_WITH_CHART // USTRUCT must not be inside preprocessor blocks, except for WITH_EDITORONLY_DATA
//#if WITH_EDITORONLY_DATA // Blueprint exposed struct members cannot be editor only
USTRUCT(BlueprintType)
struct FMCChartData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MC Loc")
	FVector LocErr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MC Loc")
	FVector LocPID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MC Rot")
	FVector RotErr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MC Rot")
	FVector RotPID;
};
//#endif // WITH_EDITORONLY_DATA

/**
 * 6D physics based movement applied to the skeletal or static mesh pointed to
 */
UCLASS(ClassGroup=(MC), meta=(BlueprintSpawnableComponent, DisplayName = "MC 6D Target"), hidecategories = (Physics, Collision, Lighting))
class UMC6DCONTROLLER_API UMC6DTarget : public UMotionControllerComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UMC6DTarget();
	
	// Dtor
	~UMC6DTarget() = default;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when actor removed from game or game ended
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Reset the location PID
	UFUNCTION(BlueprintCallable)
	void ResetLocationPID(bool bClearErrors = true);

	// Reset the location PID
	UFUNCTION(BlueprintCallable)
	void ResetRotationPID(bool bClearErrors = true);

// #if UMC_WITH_CHART // UPROPERTY must not be inside preprocessor blocks, except for WITH_EDITORONLY_DATA
//#if WITH_EDITORONLY_DATA // Blueprint exposed struct members cannot be editor only
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MC Chart")
	FMCChartData ChartData;
//#endif // WITH_EDITORONLY_DATA

#if WITH_EDITORONLY_DATA
public:
	// Hand type, to point to the right XRMotionControllers
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	EMC6DHandType HandType;
#endif // WITH_EDITORONLY_DATA

	// Check references
	void Init();

	// Start controller
	void Start();

	// Stop the controller
	void Finish(bool bForced = false);

	// Get init state
	bool IsInit() const { return bIsInit; };

	// Get started state
	bool IsStarted() const { return bIsStarted; };

	// Get finished state
	bool IsFinished() const { return bIsFinished; };

private:
	// Initial teleport the hands to the motion controller location, 
	// has to be called after a delay since at begin play the controller is not tracked yet
	void TeleportToInitialPose();

public:
	// Control type location 
	UPROPERTY(EditAnywhere, Category = "Movement Control|Location")
	EMC6DControlType LocControlType;

	// Location PID controller values
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Control|Location", meta = (ClampMin = 0))
	float PLoc;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Control|Location", meta = (ClampMin = 0))
	float ILoc;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Control|Location", meta = (ClampMin = 0))
	float DLoc;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Control|Location", meta = (ClampMin = 0))
	float MaxLoc;

	// Control type (location and rotation)
	UPROPERTY(EditAnywhere, Category = "Movement Control|Rotation")
	EMC6DControlType RotControlType;

	// Rotation PID controller values
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Control|Rotation", meta = (ClampMin = 0))
	float PRot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Control|Rotation", meta = (ClampMin = 0))
	float IRot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Control|Rotation", meta = (ClampMin = 0))
	float DRot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement Control|Rotation", meta = (ClampMin = 0))
	float MaxRot;

private:
	// True when all references are set and it is connected to the server
	uint8 bIgnore : 1;

	// True when all references are set and it is connected to the server
	uint8 bIsInit : 1;

	// True when active
	uint8 bIsStarted : 1;

	// True when done 
	uint8 bIsFinished : 1;

	// Start controllers after a delay
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	float StartDelay = 0.45f;

	// Control a skeletal mesh
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	uint8 bUseSkeletalMesh : 1;
	
	// Apply movement control to all bones of the skeletal mesh
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bUseSkeletalMesh"))
	uint8 bApplyToAllSkeletalBodies : 1;

	// Skeletal mesh actor to control
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bUseSkeletalMesh"))
	ASkeletalMeshActor* SkeletalMeshActor;

	// Static mesh actor to control
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "!bUseSkeletalMesh"))
	AStaticMeshActor* StaticMeshActor;

	// Use another target (e.g. instead of the controller the hand bone of the fully tracked body)
	UPROPERTY(EditAnywhere, Category = "Movement Control")
	bool bOverwriteTargetLocation;

	// Skeletal mesh actor to use as target
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bOverwriteTargetLocation"))
	ASkeletalMeshActor* OverwriteSkeletalMeshActor;

	// Skeletal mesh actor to use as target
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bOverwriteTargetLocation"))
	FName OverwriteBoneName;

	// Move hand to the bone location button hack
	UPROPERTY(EditAnywhere, Category = "Movement Control", meta = (editcondition = "bOverwriteTargetLocation"))
	bool bUpdateLocationButtonHack;

	// Update callback function binding
	FMC6DController Controller;

	/* Constants */
	// Loc
	constexpr static float DEF_PLoc_Vel = 20.f;
	constexpr static float DEF_ILoc_Vel = 0.f;
	constexpr static float DEF_DLoc_Vel = 1.f;
	constexpr static float DEF_MaxLoc_Vel = 20.f;

	constexpr static float DEF_PLoc_Acc = 2000.f;
	constexpr static float DEF_ILoc_Acc = 100.f;
	constexpr static float DEF_DLoc_Acc = 50.f;
	constexpr static float DEF_MaxLoc_Acc = 10000.f;

	// Rot
	constexpr static float DEF_PRot_Vel = 250.f;
	constexpr static float DEF_IRot_Vel = 0.f;
	constexpr static float DEF_DRot_Vel = 35.f;
	constexpr static float DEF_MaxRot_Vel = 1000.f;

	constexpr static float DEF_PRot_Acc = 2000.f;
	constexpr static float DEF_IRot_Acc = 100.f;
	constexpr static float DEF_DRot_Acc = 50.f;
	constexpr static float DEF_MaxRot_Acc = 10000.f;
};
