// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "SpellContainer.h"
#include "CastingDemo.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
USpellContainer::USpellContainer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	AllSpells = initSpellData();
}


// Called when the game starts
void USpellContainer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	for (TActorIterator<ACastingDemo> DemoActor(GetWorld()); DemoActor; ++DemoActor) {
		for (auto& spell : AllSpells) {
			if (DemoActor->SpellToDisplay == spell.ID) {
				DemoActor->initDemo(spell);
			}
		}
	}
}


// Spell Initialisation Functions
// The following intitialiser functions represent the vertex transformations for each spell's casting shape
TArray<FKeyPoint> USpellContainer::initBall() // it's a ball, one round circle
{
	/* Spellcrafting instructions/Rules:
	* There must always be at least two points
	* There must be one keypoint only every 90Deg or 1/4 turn around a curved movement (no in-betweens, no gaps) - We only work in circles here, no fancy ovals etc...
	* Though feel free to add that functionality if you like
	* Every first straight line can not be followed by an identical straight line (there must be a significant difference in hand rotation or line direction)
	* The movements can be anything in any order, however, scale is only confirmed after the first positional change is completed
	* Except - the left hand must have the same movement type (i.e. if RH moves in straight line, so does left, or stationary or curved)
	* All positional vectors are multiplied by the scale.
	* First position vectors are always 0,0,0 (Distance from starting hand position MUST be 0 in starting position)
	* I mean, you can try to be clever if you like, but good luck...
	* Remember Unreal Engine Grid works backwards to normal mathematics - because they like to keep mathematicians away
	* X represents forward/backward. Y Represents left/right. Z is familiar... up/down
	*/
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,0 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,0 },
			// Default move type is Point - and there is no point... in changing something that does not need checking
		},

		FKeyPoint{
			FVector { 0,0.5,-1 },
			FRotator { 0,0,0 },
			FVector { 0,-0.5,-1 },
			FRotator { 0,0,0 },
			MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,-0.5,-1 },
				FRotator{ 0,0,0 },
				FVector{ 0,0.5,-1 },
				FRotator{ 0,0,0 },
				MoveType::Line
		},

			FKeyPoint{ // Return to origin
				FVector{ 0,0,0 },
				FRotator{ 0,0,0 },
				FVector{ 0,0,0 },
				FRotator{ 0,0,0 },
				MoveType::Line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initWall() // It's a wall... one straight line
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,-90 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,90 }
		},

			FKeyPoint{
				FVector{ 0,1,0 },
				FRotator{ 0,0,-90 },
				FVector{ 0,-1,0 },
				FRotator{ 0,0,90 },
				MoveType::Line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initBeam() // Like punching, only more magical
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,0 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,0 }
		},

			// It was a tough decision whether to ensure the rotation is uniform along the length of the line or not
			// I went for, "I can't be bothered to implement that many checks."
			FKeyPoint{
				FVector{ 1,0,0 },
				FRotator{ 0,0,-90 },
				FVector{ 1,0,0 },
				FRotator{ 0,0,90 },
				MoveType::Line // I know, it's still called a straight line, even if you twist your wrist while forming it
		}
	};
}

TArray<FKeyPoint> USpellContainer::initAtune() // I am one with the energies
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,90 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,-90 }
		},

			FKeyPoint{
				FVector{ -1,-1,0.75 },
				FRotator{ 60,-90,0 },
				FVector{ -1,0.95,0.75 },
				FRotator{ 60,90,0 },
				MoveType::Line // how can an arc be 90Deg but arm angle only 60Deg I hear you ask... Try twisting your wrist and drawing a curved line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initAir() // Tumble dryer - maths this if you can
{
	// Special case spell - only do something like this if you know what's going on

	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,-90,-90 },
			FVector{ 0,0,0 },
			FRotator{ 0,90,90 },
			MoveType::Line // THIS TELLS SCALE CHECKER TO SET SCALE RELATIVE TO STARTING HAND POSITIONS instead of waiting for first movement!
			// All other spells first keypoint default to MoveType::Point)
		},

			FKeyPoint{
				FVector{ -0.5,0,-0.5 },
				FRotator{ 0,-90,-90 },
				FVector{ 0.5,0,0.5 },
				FRotator{ 0,90,90 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ -1,0,0 },
				FRotator{ 0,-90,-90 },
				FVector{ 1,0,0 },
				FRotator{ 0,90,90 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ -0.5,0,0.5 },
				FRotator{ 0,-90,-90 },
				FVector{ 0.5,0,-0.5 },
				FRotator{ 0,90,90 },
				MoveType::Line
		},

			FKeyPoint{ // Return to origin
				FVector{ 0,0,0 },
				FRotator{ 0,-90,-90 },
				FVector{ 0,0,0 },
				FRotator{ 0,90,90 },
				MoveType::Line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initWater()
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,0 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,0 }
		},

			FKeyPoint{
				FVector{ 0,1,-1 },
				FRotator{ 0,0,0 },
				FVector{ 0,-1,-1 },
				FRotator{ 0,0,0 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,1,0 },
				FRotator{ 0,0,0 },
				FVector{ 0,-1,0 },
				FRotator{ 0,0,0 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,2,-1 },
				FRotator{ 0,0,0 },
				FVector{ 0,-2,-1 },
				FRotator{ 0,0,0 },
				MoveType::Line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initEarth()
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,-90 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,90 }
		},

			FKeyPoint{
				FVector{ 0,0,1 },
				FRotator{ 0,0,-90 },
				FVector{ 0,0,1 },
				FRotator{ 0,0,90 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,0,1 },
				FRotator{ 0,0,0 },
				FVector{ 0,0,1 },
				FRotator{ 0,0,0 },
				MoveType::Point
		},

			FKeyPoint{
				FVector{ 0,-1,1 },
				FRotator{ 0,0,0 },
				FVector{ 0,1,1 },
				FRotator{ 0,0,0 },
				MoveType::Line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initFire()
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,0 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,0 }
		},

			FKeyPoint{
				FVector{ 0,-1,1 },
				FRotator{ 0,0,0 },
				FVector{ 0,1,1 },
				FRotator{ 0,0,0 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,-0.5,1.5 },
				FRotator{ 0,0,0 },
				FVector{ 0,0.5,1.5 },
				FRotator{ 0,0,0 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,-1,2 },
				FRotator{ 0,0,0 },
				FVector{ 0,1,2 },
				FRotator{ 0,0,0 },
				MoveType::Line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initIncDur()
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,90 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,-90 } // Start palms up
		},

			FKeyPoint{
				FVector{ 0,1,1 },
				FRotator{ 0,0,0 },
				FVector{ 0,-1,1 },
				FRotator{ 0,0,0 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,2,0 },
				FRotator{ 0,0,-90 },
				FVector{ 0,-2,0 },
				FRotator{ 0,0,90 }, // End palms down
				MoveType::Line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initDecDur()
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,-90 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,90 }
		},

			FKeyPoint{
				FVector{ 0,-1,1 },
				FRotator{ 0,0,0 },
				FVector{ 0,1,1 },
				FRotator{ 0,0,0 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,-1,0 },
				FRotator{ 0,0,90 },
				FVector{ 0,1,0 },
				FRotator{ 0,0,-90 },
				MoveType::Line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initIncPwr() // Not quite Gangnam Style
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,-135 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,135 }
		},

			FKeyPoint{
				FVector{ 0,-1,1 },
				FRotator{ 0,0,-135 },
				FVector{ 0,1,1 },
				FRotator{ 0,0,135 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,-1,1 },
				FRotator{ 0,0,-45 },
				FVector{ 0,1,1 },
				FRotator{ 0,0,45 },
				MoveType::Point
		},

			FKeyPoint{
				FVector{ 0,0,2 },
				FRotator{ 0,0,-45 },
				FVector{ 0,0,2 },
				FRotator{ 0,0,45 },
				MoveType::Line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initDecPwr()
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,0,-135 },
			FVector{ 0,0,0 },
			FRotator{ 0,0,135 }
		},

			FKeyPoint{
				FVector{ 0,1,-1 },
				FRotator{ 0,0,-135 },
				FVector{ 0,-1,-1 },
				FRotator{ 0,0,135 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,1,-1 },
				FRotator{ 0,0,-45 },
				FVector{ 0,-1,-1 },
				FRotator{ 0,0,45 },
				MoveType::Point
		},

			FKeyPoint{
				FVector{ 0,0,-2 },
				FRotator{ 0,0,-45 },
				FVector{ 0,0,-2 },
				FRotator{ 0,0,45 },
				MoveType::Line
		}
	};
}

TArray<FKeyPoint> USpellContainer::initExplosive()
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,-90,-90 },
			FVector{ 0,0,0 },
			FRotator{ 0,90,0 }
		},

			FKeyPoint{
				FVector{ 0,-1,0 },
				FRotator{ 0,-90,-90 },
				FVector{ 0,1,0 },
				FRotator{ 0,90,0 },
				MoveType::Line
		},

			FKeyPoint{
				FVector{ 0,-1,0 },
				FRotator{ 0,-90,0 },
				FVector{ 0,1,0 },
				FRotator{ 0,90,90 },
				MoveType::Point
		}
	};
}

TArray<FKeyPoint> USpellContainer::initMagnetic()
{
	// Spell Keypoint configuration
	return TArray<FKeyPoint>{
		FKeyPoint{
			FVector{ 0,0,0 },
			FRotator{ 0,-90,0 },
			FVector{ 0,0,0 },
			FRotator{ 0,90,90 }
		},

			FKeyPoint{
				FVector{ 0,0,0 },
				FRotator{ 0,-90,-90 },
				FVector{ 0,0,0 },
				FRotator{ 0,90,0 },
				MoveType::Point
		},

			FKeyPoint{
				FVector{ 0,1,0 },
				FRotator{ 0,-90,-90 },
				FVector{ 0,-1,0 },
				FRotator{ 0,90,0 },
				MoveType::Line
		}
	};
}

// This is where tolerances are set up for the various spell casts
// And Where the full spell list is defined
TArray<FSpellData> USpellContainer::initSpellData()
{
	// If you wish to ignore any tolerance simply set to 0
	// Eg. If it doesn't matter how much Yaw there is, set Yaw to 0
	// Note all tolerances are +&- i.e. a value of 15Deg results in a total tolerance of 30Deg
	return TArray<FSpellData>{
		//		    KPArray, RelStartPosLH->RH, Tolerance%ofScale, RotToleranceDeg, SpellID,	isDualOnly
		FSpellData{ initBall(), FVector{0,1,0}, FVector{1,1,1}, FRotator{0,45,30}, SpellID::Ball, false },   // Can be cast one handed
			FSpellData{ initWall(), FVector{0,1,0}, FVector{1,1,1}, FRotator{45,0,30}, SpellID::Wall, false },   // Can be cast one handed
			FSpellData{ initBeam(), FVector{0,1,0}, FVector{1,1,1}, FRotator{45,0,30}, SpellID::Beam, false },   // Can be cast one handed
			FSpellData{ initAtune(), FVector{0,1,0}, FVector{0,1,1}, FRotator{45,45,45}, SpellID::Atune, false }, // Can be cast one handed

			FSpellData{ initAir(), FVector{1,0,0}, FVector{1,1,1}, FRotator{0,45,45}, SpellID::Air, true },
			FSpellData{ initWater(), FVector{0,1,0}, FVector{1,1,1}, FRotator{0,0,45}, SpellID::Water, true },
			FSpellData{ initEarth(), FVector{0,1,0}, FVector{1,1,1}, FRotator{0,30,30}, SpellID::Earth, true },
			FSpellData{ initFire(), FVector{0,1,0}, FVector{1,1,1}, FRotator{45,0,30}, SpellID::Fire, true },

			FSpellData{ initIncDur(), FVector{0,1,0}, FVector{1,1,1}, FRotator{0,0,30}, SpellID::IncDur, true },
			FSpellData{ initDecDur(), FVector{0,1,0}, FVector{1,1,1}, FRotator{0,0,30}, SpellID::DecDur, true },
			FSpellData{ initIncPwr(), FVector{0,1,0}, FVector{1,1,1}, FRotator{0,0,40}, SpellID::IncPwr, true },
			FSpellData{ initDecPwr(), FVector{0,1,0}, FVector{1,1,1}, FRotator{0,0,40}, SpellID::DecPwr, true },
			FSpellData{ initExplosive(), FVector{0,1,0}, FVector{1,1,1}, FRotator{0,40,30}, SpellID::Explode, true },
			FSpellData{ initMagnetic(), FVector{0,1,0}, FVector{1,1,1}, FRotator{0,40,30}, SpellID::Magnet, true },
	};
}