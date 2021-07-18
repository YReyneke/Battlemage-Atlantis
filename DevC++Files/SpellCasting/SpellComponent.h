// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.
/**
 * NOTE: All spell-keypoints positions are relative to the location and rotation of this spell component -
 *		 Therefore, this components position must be linked to VRRootComponent, however rotation must be updated based on start of cast
 * Reminder: VRRootComponent only rotates in Yaw.
 * NOTE: Scale at which spell cast accuracy is defined does not affect rotation calculation only translation
 * NOTE: All Spell casting scales are dynamic
 */

//newActor = World->SpawnActor<AActor>(*actorClass, Location, Rotation, ActorSpawnParams);

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellCastingController.h"
#include "SpellComponent.generated.h"


UCLASS( Blueprintable )
class BATTLEMAGEATLANTIS01_API USpellComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpellComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: // List of spells and spell components

	// Array holding the spell data of all the game's spells - basically the memory bank
	// Yes it's hard coded. No I'm not currently planning on changing that.
	TArray<FSpellData> AllSpells{};

	UPROPERTY(VisibleAnywhere, category = "Setup")
	USpellContainer* SpellContainer;

	// Used to store blueprint of CastingNode
	UPROPERTY(EditAnywhere, category="Setup")
	TSubclassOf<class ACastingNode> CastingNodeBlueprint;

	UPROPERTY(EditAnywhere, category="Setup")
	TSubclassOf<class USpellCastingController> SpellControllerBlueprint;
	class USpellCastingController* SpellCastingController;

public: // Functions to handle spellcasting

	// Public initialisation functions - must be called from constructor of parent class
	bool SetupHands(class UMotionControllerComponent* LeftHand, class UMotionControllerComponent* RightHand, class UCameraComponent* hmdCamera);

	// Public input functions - must be bound to user input in parent class
	void RightHandCast();
	void LeftHandCast();
	void RightHandStopCast();
	void LeftHandStopCast();
	void RightHandLaunch();
	void LeftHandLaunch();
	void RightHandStopLaunch();
	void LeftHandStopLaunch();

	// Public interface functions - the stuff I thought might be useful elsewhere
	FVector ToSpellcastingGrid(FVector Pos);
	FRotator ToSpellcastingGrid(FRotator Rot);
	
	// Returns full datastructure of all spells in a string.
	//FString ToString(); // FString.append() everything...

private: // Operating Variables

	// Pointers to components
	class UMotionControllerComponent* RHand;
	class UMotionControllerComponent* LHand;
	class UCameraComponent* hmdCamera;

	/*UPROPERTY(EditDefaultsOnly)
	class USpellCastingController* SpellcastingController;*/

	// Default setup values
	const float MAX_MOVE_TOLERANCE = 8.f; // Constant used as the maximum movement allowed from ideal line for tolerance checks
	const float MIN_MOVE_SCALE = 8.f; // Constant used as minimum movement for spellcasting scale to be updated
	const float MAX_DUAL_HAND_DELAY = 0.2f; // Time in seconds to wait for second hand to decide whether to cancel dual-casting
	float CurrentDualHandDelay{ 0 }; // Time in seconds since first hand started casting

	// Connected to user inputs A/X (oculus touch)
	bool isRHCasting{ false };
	bool isLHCasting{ false };
	// NOTE: Dual handed casting happens whenever both of these are set to true in short succession
	// Right/LeftHandCast functions do not allow both to be true if MAX_DUAL_HAND delay has been reached

	bool isLaunching{false};
	bool isRHLaunching{ false };
	bool isLHLaunching{ false };
	float CurrentDualLaunchDelay{ 0.f };

	// Spellcasting state boolean values
	bool isCasting{ false };
	bool isComplete{ false };

	// Start position and orientation of both hands for this cast period in terms of spellcasting grid
	FVector RHStartPos{};
	FVector LHStartPos{};
	// NOTE: All positional tolerance calculations must be passed values relative to the start position

	// Enum containing ID of currently active spell
	// Set to Multiple if more than one spell could be cast given the players current movement
	// Set to None if no spell can be cast right now
	SpellID CurrentSpell{ SpellID::None };

private: // Operating Functions, where the main logic goes

	void SetFrameStartPosAndRot();

	// Returns spellID of current spell being cast.
	SpellID UpdateSpellList(); // Will return None if no spells can be cast and Multiple if the spell which player is casting can not yet be decided
	SpellID GetActiveSpells();

	bool SpellSetup();
	bool UpdateSpellStates();
	void UpdateCastingNodes();
	void EndCast();

	void UpdateSpellScale(FSpellData& spell, int kpID);
	FVector SpellcastingGridToWorld(FVector gridPosition);
	bool CheckRHToLHDirection(FSpellData& referenceSpell, FVector PosTolerance);

private: // Calculations, where we get the maths done

	bool CheckRHStaticTolerance(FSpellData& spell, FKeyPoint& kp);
	bool CheckLHStaticTolerance(FSpellData& spell, FKeyPoint& kp);
	bool CheckRHMoveTolerance(FSpellData& spell, int& kpID);
	bool CheckLHMoveTolerance(FSpellData& spell, int& kpID);

	// Functions dealing with single points
	bool PointEqual(FVector posToCheck, FVector refPos, FVector posTolerance);
	bool PointEqual(FRotator rotToCheck, FRotator refRot, FRotator rotTolerance);
	bool PointEqual(float PointToCheck, float refPoint, float PointTolerance);

	// Functions dealing with a movement from point A->B
	
	bool RotationMoveInTolerance(FRotator RotToCheck, FRotator StartRot, FRotator EndRot, FRotator RotTolerance); // NOTE: All units are Deg, there is no rotational scaling required
	bool LineMoveInTolerance(FVector PosToCheck, FVector StartPos, FVector EndPos, FVector PosTolerance); // NOTE: All values must be converted to the unit grid type (cannot be world size co-ordinates)
	bool DiagonalMoveInTolerance(float iPos, float jPos, float DeltaI, float DeltaJ, float WidthTolerance); // Part of LineMoveInTolerance()
	//bool ArcMoveInTolerance(FVector PosToCheck, FVector StartPos, FVector EndPos, FVector PosTolerance, MoveType Quadrant); // NOTE: All values must be converted to the unit grid type (cannot be worldsize coordinates)
	//bool CurveMoveInTolerance(int quadrant, float aCheckPos, float bCheckPos, float tolerance) // Part of ArcMoveInTolerance()

private: // *** Test Section ***//
	// THIS IS WHERE ANY TEST CODE CAN BE FOUND //

	// Move Data File Output Section
	IPlatformFile& FileManager{FPlatformFileManager::Get().GetPlatformFile()};
	FString FileID{ (FPaths::ProjectLogDir()).Append(TEXT("YR_Beam_MotionToleranceLog.txt")) }; // FDateTime::Today()/Now().ToString(); - today = date only, now includes current time also
	FString FileOutput{};
	
	FVector MinLHPos{};
	FVector MaxLHPos{};
	FRotator MinLHRot{};
	FRotator MaxLHRot{};

	FVector MinRHPos{};
	FVector MaxRHPos{};
	FRotator MinRHRot{};
	FRotator MaxRHRot{};

	bool isLoggingLHData{ false };
	bool isLoggingRHData{ false };
	int castNo{ 0 };

	void RunDevTests();
	void UpdateMoveDetails();
	void SendMoveDetailsToLog(FVector MinPos, FVector MaxPos, FRotator MinRot, FRotator MaxRot, bool isRH);
};