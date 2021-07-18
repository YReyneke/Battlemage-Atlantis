// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellContainer.generated.h"

/*
* Data structure that holds all the spells and their base casting requirements
*/

UENUM()
enum SpellID {
	Ball, Wall, Beam, Atune,
	Air, Water, Earth, Fire,
	IncDur, DecDur, IncPwr, DecPwr, Explode, Magnet,
	None, Multiple
};

UENUM()
enum MoveType {
	Point, // There is no movement between this keypoint and the previous (Also used for the first keypoint in any spell)
	Line, // Used when the movement from the previous keypoint is a straight line
	//Arc1, Arc2, Arc3, Arc4 // Used when the movement from the previous keypoint is a curved line ** currently not implemented
		// for YZ plane Z = sin(Y * 90); For XZ plane Z = sin(X * 90); For XY plane Y = sin(X * 90)!
		// Arc type represents which quadrant the start of the motion is based on
};

// Structure used to store the evaluation points of all spells
// Structure Contents: RHPosition, RHRotation, LHPosition, LHRotation
USTRUCT()
struct FKeyPoint {
	GENERATED_BODY()

	// The keypoint transformation data used by the brains of this operation
	FVector RHPosition;
	FRotator RHRotation;
	FVector LHPosition;
	FRotator LHRotation;
	MoveType Motion{ MoveType::Point };
	bool RHComplete{ false };
	bool LHComplete{ false };

	// The graphical content presented to the user - see CastingNode.h/.cpp for more details
	class ACastingNode* CastingNodeRH{ nullptr };
	class ACastingNode* CastingNodeLH{ nullptr };
};

// Structure Contents: Key Spellcasting Points, Positional Tolerance, Rotational Tolerance
USTRUCT()
struct FSpellData {
	GENERATED_BODY()

		TArray<FKeyPoint> KeyPoints;
	FVector LtoRRelativeStartPos; // NOTE: Vector represents required relative start direction from LH to RH at start of spell
	FVector PositionalTolerance; // NOTE: Set tolerance to 0 to ignore axis
	FRotator RotationalTolerance; // NOTE: Set tolerance to 0 to ignore axis
	SpellID ID; // See SpellCastingController.h
	bool isDualOnly{ true };
	float Scale{ 5.f }; // Default to the same as MIN_MOVE_SCALE
	bool isScaleSet{ false };
	bool canCast{ true };
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEMAGEATLANTIS01_API USpellContainer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpellContainer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	// Array holding the spell data of all the game's spells - basically the memory bank
	// Yes it's hard coded. No I'm not currently planning on changing that.
	TArray<FSpellData> AllSpells{};

	/*// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
*/
private: // List of spells and spell components

	// The Initialisation functions
	TArray<FKeyPoint> initBall();
	TArray<FKeyPoint> initWall();
	TArray<FKeyPoint> initBeam();
	TArray<FKeyPoint> initAtune();

	TArray<FKeyPoint> initAir();
	TArray<FKeyPoint> initWater();
	TArray<FKeyPoint> initEarth();
	TArray<FKeyPoint> initFire();

	TArray<FKeyPoint> initIncDur();
	TArray<FKeyPoint> initDecDur();
	TArray<FKeyPoint> initIncPwr();
	TArray<FKeyPoint> initDecPwr();
	TArray<FKeyPoint> initExplosive();
	TArray<FKeyPoint> initMagnetic();

	TArray<FSpellData> initSpellData();
};
