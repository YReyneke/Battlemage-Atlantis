// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "SpellComponent.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "CastingNode.h"
#include "SpellCastingController.h"


// Sets default values for this component's properties
USpellComponent::USpellComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SpellContainer = CreateDefaultSubobject<USpellContainer>(TEXT("SpellContainer"));

	// Setup Spells
	AllSpells = SpellContainer->AllSpells;

	// ******* Dev section *******
	//SpellNodeList()

	// Casting motion tolerance recorder
	FileOutput = (FDateTime::Now().ToString()).Append(TEXT("\nMotion Log:\nContains the full list of recorded max/min positions and rotations of player casts relative to casting component\n\n"));
	FFileHelper::SaveStringToFile(FileOutput, *FileID);
}


// Called when the game starts
void USpellComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (SpellControllerBlueprint) {
		SpellCastingController = NewObject<USpellCastingController>(this, SpellControllerBlueprint);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to create new SpellCastingController... SCC_BP Not Found!"))
	}

	if (SpellCastingController) {
		SpellCastingController->ConnectMotionControllers(LHand, RHand, hmdCamera);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to connect motion controllers to Spellcasting controller. SCC Note Found!"))
	}
}

// Called every frame
void USpellComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update dual hand delay as required
	if (isRHCasting != isLHCasting) { // If one hand is casting
		if (CurrentDualHandDelay <= MAX_DUAL_HAND_DELAY) {
			CurrentDualHandDelay += DeltaTime;
		}
	}
	if (isLaunching) {
		if (!(isRHLaunching && isLHLaunching) && CurrentDualLaunchDelay <= MAX_DUAL_HAND_DELAY) {
			CurrentDualLaunchDelay += DeltaTime;
		}
		else {
			if (isRHLaunching && isLHLaunching) {
				SpellCastingController->LaunchDualHSpell();
				isLaunching = false;
			}
			else if (isRHLaunching && !isLHLaunching) {
				SpellCastingController->LaunchRHSpell();
				isLaunching = false;
			}
			else if (!isRHLaunching && isLHLaunching) {
				SpellCastingController->LaunchLHSpell();
				isLaunching = false;
			}
		}
	}

	if ((isRHCasting && isLHCasting) || // If both hands are casting OR
		((isRHCasting != isLHCasting) && (CurrentDualHandDelay > MAX_DUAL_HAND_DELAY))) { // If one hand is casting and dual hand delay has been passed
		CurrentSpell = UpdateSpellList();
	}

	UpdateCastingNodes();

	//UE_LOG(LogTemp, Warning, TEXT("Current Spellgrid Rotation: %s!!!"), *GetComponentRotation().ToString());

	// *** DEV Section *** //
	RunDevTests();
	//UE_LOG(LogTemp, Warning, TEXT("Current Rotation: LH: %s; RH: %s"), *ToSpellcastingGrid(LHand->GetComponentRotation()).ToString(), *ToSpellcastingGrid(RHand->GetComponentRotation()).ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Current Location: LH: %s; RH: %s"), *ToSpellcastingGrid(LHand->GetComponentLocation()).ToString(), *ToSpellcastingGrid(RHand->GetComponentLocation()).ToString());
}

bool USpellComponent::SetupHands(UMotionControllerComponent* LeftHand, UMotionControllerComponent* RightHand, UCameraComponent* hmdCam) {
	if (LeftHand && RightHand && hmdCam) {
		LHand = LeftHand;
		RHand = RightHand;
		hmdCamera = hmdCam;

		return true;
	}
	return false;
}

// These four functions activate and de-activate the LH/RH casting variables
void USpellComponent::RightHandCast() { 
	if (isLHCasting) { // If LH is already casting, check dual hand delay
		if (CurrentDualHandDelay <= MAX_DUAL_HAND_DELAY) {
			isRHCasting = true;
			UE_LOG(LogTemp, Warning, TEXT("Dual Casting Started..."));
		}
	}
	else { // Otherwise
		isRHCasting = true;
		UE_LOG(LogTemp, Warning, TEXT("Right Hand Casting Started..."));
	}

	CurrentDualHandDelay = 0;
}

void USpellComponent::LeftHandCast() {
	if (isRHCasting) { // If RH is already casting, check dual hand delay
		if (CurrentDualHandDelay <= MAX_DUAL_HAND_DELAY) {
			isLHCasting = true;
			UE_LOG(LogTemp, Warning, TEXT("Dual Casting Started..."));
		}
	}
	else { // Otherwise
		isLHCasting = true;
		UE_LOG(LogTemp, Warning, TEXT("Left Hand Casting Started..."));
	}

	CurrentDualHandDelay = 0;
}

void USpellComponent::RightHandStopCast() {
	if (isComplete){
		EndCast();
	}

	isRHCasting = false;
	isCasting = false; // Reset so that spellchecker/setup functions can update which spell to cast

	UE_LOG(LogTemp, Warning, TEXT("Right Hand Casting Stopped...\n "));
}

void USpellComponent::LeftHandStopCast() {
	if (isComplete) {
		EndCast();
	}

	isLHCasting = false;
	isCasting = false; // Reset so that spellchecker/setup functions can update which spell to cast

	UE_LOG(LogTemp, Warning, TEXT("Left Hand Casting Stopped...\n "));
}

void USpellComponent::RightHandLaunch()
{
	if (!isLaunching) {
		CurrentDualLaunchDelay = 0;
	}

	isRHLaunching = true;
	isLaunching = true;
}

void USpellComponent::LeftHandLaunch()
{
	if (!isLaunching) {
		CurrentDualLaunchDelay = 0;
	}

	isLHLaunching = true;
	isLaunching = true;
}

void USpellComponent::RightHandStopLaunch()
{
	isRHLaunching = false;
	isLaunching = false;
}

void USpellComponent::LeftHandStopLaunch()
{
	isLHLaunching = false;
	isLaunching = false;
}

// Projects vector onto spellcasting grid coordinate system
FVector USpellComponent::ToSpellcastingGrid(FVector Pos) {
	//UE_LOG(LogTemp, Warning, TEXT("SpellComponent Location: %s; Comparing Location: %s"), *GetComponentLocation().ToString(), *Pos.ToString());
	return (Pos - GetComponentLocation()).RotateAngleAxis(-GetComponentRotation().Yaw, GetUpVector()); // Works because spellcasting grid (i.e. SpellComponent) only rotates in Yaw
	// This is the fifth attempt, learning on the job i guess - but I feel like I'm starting to understand - everything works backwards i.e. the cake is a lie.
}

// Converts rotation relative to spellcasting grid coordinate system
FRotator USpellComponent::ToSpellcastingGrid(FRotator Rot) {
	FRotator convertedRot{ Rot - GetComponentRotation() };

	// Something Unreal.... Not entirely sure, but it's required
	if (convertedRot.Pitch < -260) convertedRot.Pitch += 360;
	if (convertedRot.Yaw < -260) convertedRot.Yaw += 360;
	if (convertedRot.Roll < -260) convertedRot.Roll += 360;
	if (convertedRot.Pitch > 260) convertedRot.Pitch -= 360;
	if (convertedRot.Yaw > 260) convertedRot.Yaw -= 360;
	if (convertedRot.Roll > 260) convertedRot.Roll -= 360;

	return convertedRot;
}

// Converts from spellcasting grid to world coordinates
FVector USpellComponent::SpellcastingGridToWorld(FVector gridPosition) {
	return gridPosition.RotateAngleAxis(GetComponentRotation().Yaw, GetUpVector()) + GetComponentLocation(); // Works because spellcasting grid (i.e. SpellComponent) only rotates in Yaw
}

// Sets the reference location for all spellcasting - without this, no tolerances will be right
// If one handed casting - sets location and rot to casting hand location and rot (Yaw only)
// If dual casting sets location to avg between hands and rot to hmcCamera rot (Yaw only)
void USpellComponent::SetFrameStartPosAndRot() {
	if (isLHCasting && !isRHCasting) {
		SetWorldLocation(LHand->GetComponentLocation());
		SetWorldRotation(FRotator{ 0,LHand->GetComponentRotation().Yaw,0 });
		return;
	}
	else if (!isLHCasting && isRHCasting) {
		SetWorldLocation(RHand->GetComponentLocation());
		SetWorldRotation(FRotator{ 0,RHand->GetComponentRotation().Yaw,0 });
		return;
	}
	else {
		FVector MaxPos{ RHand->GetComponentLocation().ComponentMax(LHand->GetComponentLocation()) };
		FVector MinPos{ RHand->GetComponentLocation().ComponentMin(LHand->GetComponentLocation()) };
		SetWorldLocation(MinPos + ((MaxPos - MinPos) / 2));
		SetWorldRotation(FRotator{ 0,hmdCamera->GetComponentRotation().Yaw, 0 });
	}
}

SpellID USpellComponent::UpdateSpellList()
{
	if (!isCasting) { // If player just started casting a spell or spell completed
		if (!isComplete) { // If player just started casting
			if (SpellSetup()) { // If a spell can be cast from the start position and orientation player has chosen
				isCasting = true;
			}
			else {
				return SpellID::None;
			}
		}
	}
	else { // If a spell is already being cast
		isComplete = UpdateSpellStates();
	}

	return GetActiveSpells();
}

// If only one spell canCast returns that spell, otherwise returns Multiple
// Returns None if no spell can be cast
SpellID USpellComponent::GetActiveSpells() {
	SpellID id{ SpellID::None };
	int activeSpellCount{ 0 };

	for (const auto& spell : AllSpells) {
		if (spell.canCast) { 
			activeSpellCount++;
			id = spell.ID;
			//UE_LOG(LogTemp, Warning, TEXT("%s spell is active..."), *UEnum::GetValueAsString(id));
		}
	}
	return (activeSpellCount > 1) ? SpellID::Multiple : id;
}

// The starting point for spell position calculations - if this doesn't run, nothing that follows will run correctly
// Returns true if a spell can be cast from the start position and orientation player has chosen
// If dual casting, it is OK to have hands complete points asynchronously, they must just stay in tolerance until casting for that hand completes
bool USpellComponent::SpellSetup()
{
	UE_LOG(LogTemp, Warning, TEXT("Running SpellSetup()"));

	// Setup up the reference point for all spell casting calculations
	SetFrameStartPosAndRot();

	// Save start position and orientation of hands for calculations
	RHStartPos = ToSpellcastingGrid(RHand->GetComponentLocation());
	LHStartPos = ToSpellcastingGrid(LHand->GetComponentLocation());

	// Reset all spell complete states to start settings
	for (auto& spell : AllSpells) {
		spell.canCast = true;
		spell.isScaleSet = false;
		spell.Scale = MIN_MOVE_SCALE;
		for (auto& kp : spell.KeyPoints) {
			kp.LHComplete = false;
			kp.RHComplete = false;
		}
	}

	// If only one hand is casting disable all dual hand spells
	if (isLHCasting != isRHCasting)
	{
		for (auto& spell : AllSpells) {
			if (spell.isDualOnly) {
				spell.canCast = false;
				//UE_LOG(LogTemp, Warning, TEXT("%s spell deactivated -> isDualCasting"), *UEnum::GetValueAsString(spell.ID));
			}
		}
	}
	
	// Check that remaining spell start positions are in tolerance - i.e. has player started with hands in correct orientation for a spell
	for (auto& spell : AllSpells) {
		if (spell.canCast) {
			bool inTolerance{ true };
			if (isRHCasting) {
				// Check RH in tolerance
				inTolerance = CheckRHStaticTolerance(spell, spell.KeyPoints[0]);
				if (inTolerance) {
					spell.KeyPoints[0].RHComplete = true;
					UE_LOG(LogTemp, Warning, TEXT("Starting pos/rot ACCEPTED for spell: %s"), *UEnum::GetValueAsString(spell.ID));
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Starting pos/rot REJECTED for spell: %s"), *UEnum::GetValueAsString(spell.ID));
				}
			}
			if (isLHCasting && inTolerance) { // if previous check returned true
				// Check LH in tolerance
				inTolerance = CheckLHStaticTolerance(spell, spell.KeyPoints[0]);
				if (inTolerance) {
					spell.KeyPoints[0].LHComplete = true;
					UE_LOG(LogTemp, Warning, TEXT("Starting LH pos/rot ACCEPTED for spell: %s"), *UEnum::GetValueAsString(spell.ID));
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Starting LH pos/rot REJECTED for spell: %s"), *UEnum::GetValueAsString(spell.ID));
				}
			}
			if (isLHCasting && isRHCasting && inTolerance) { // If dual casting and previous check returned true
				// Check hands are correctly positioned relative to each other i.e. if RH should be above/in front of/next to LH
				inTolerance = CheckRHToLHDirection(spell, spell.PositionalTolerance * MAX_MOVE_TOLERANCE);
				if (inTolerance) {
					UE_LOG(LogTemp, Warning, TEXT("Starting relative H position ACCEPTED for spell: %s"), *UEnum::GetValueAsString(spell.ID));
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Starting relative H position REJECTED for spell: %s"), *UEnum::GetValueAsString(spell.ID));
				}
				UE_LOG(LogTemp, Warning, TEXT("LH Start Pos: %s; RH Start Pos: %s; Reltaive Pos: %s"), *LHStartPos.ToString(), *RHStartPos.ToString(), *(RHStartPos - LHStartPos).ToString())
			}
			spell.canCast = inTolerance;
		}
	}

	// Return true if at least one spell can be cast
	for (const auto& spell : AllSpells) {
		if (spell.canCast) return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("No spells can be cast from that starting configuration!\n "));

	// If all the above checks fail return false - i.e. no spell can be cast from start position and orientation player has chosen
	return false;
}

// The overarching logic for the tolerance checker code - updates canCast to false if motion/orientation goes out of tolerance
bool USpellComponent::UpdateSpellStates() {
	for (auto& spell : AllSpells) {
		if (spell.canCast) {

			// What is next point that needs to be completed for LH and RH
			int RHNextPointID{ 0 };
			int LHNextPointID{ 0 };
			bool allRHPointsComplete{ false };
			bool allLHPointsComplete{ false };

			if (isRHCasting) {
				for (auto& kp : spell.KeyPoints) {
					RHNextPointID += 1;
					if (!kp.RHComplete) {
						//UE_LOG(LogTemp, Warning, TEXT("RHKP loop stopped at point: %d for spell %s"), RHNextPointID, *UEnum::GetValueAsString(spell.ID));
						break;// Stop counting if keypoint is not complete
					}
				}
			}

			if (isLHCasting) {
				for (auto& kp : spell.KeyPoints) {
					LHNextPointID += 1;
					if (!kp.LHComplete) {
						//UE_LOG(LogTemp, Warning, TEXT("LHKP loop stopped at point: %d for spell %s"), RHNextPointID, *UEnum::GetValueAsString(spell.ID));
						break;// Stop counting if keypoint is not complete
					}
				}
			}

			// Fix the logic for computers
			RHNextPointID -= 1;
			LHNextPointID -= 1;

			// Update scale if required
			if (!spell.isScaleSet) {
				UpdateSpellScale(spell, (RHNextPointID > LHNextPointID) ? RHNextPointID : LHNextPointID);
			}

			if (isRHCasting) {
				// If this is the last point and it is already completed
				if (RHNextPointID == spell.KeyPoints.Num() - 1 && spell.KeyPoints[RHNextPointID].RHComplete) {
					allRHPointsComplete = true;
				}
				else {
					// Set RHComplete status true if hand in positional tolerance with the point
					spell.KeyPoints[RHNextPointID].RHComplete = CheckRHStaticTolerance(spell, spell.KeyPoints[RHNextPointID]);
				}
			}

			if (isLHCasting) {
				// If this is the last point and it is already completed
				if (LHNextPointID == spell.KeyPoints.Num() - 1 && spell.KeyPoints[LHNextPointID].LHComplete) {
					allLHPointsComplete = true;
				}
				else {
					// Set LHComplete status if hand in positional tolerance with the point
					spell.KeyPoints[LHNextPointID].LHComplete = CheckLHStaticTolerance(spell, spell.KeyPoints[LHNextPointID]);
				}
			}

			// If all required keypoints have been completed, set all other spell canCast to false and return true
			if ((allRHPointsComplete && isRHCasting && allLHPointsComplete && isLHCasting) || // If dual handed casting AND BOTH hands completed OR
			   (((allRHPointsComplete && isRHCasting) || (allLHPointsComplete && isLHCasting)) && isRHCasting != isLHCasting)) { // One handed casting AND one hand completed
				for (auto& spl : AllSpells) {
					if (spl.ID != spell.ID) spl.canCast = false;
				}
				return true;
			}

			// If keypoint not yet complete check hand movement is still in tolerance
			if (isRHCasting && !spell.KeyPoints[RHNextPointID].RHComplete) {
				spell.canCast = CheckRHMoveTolerance(spell, RHNextPointID); // Disable can cast if right hand out of tolerance
			}

			// If keypoint not yet complete check hand movement is still in tolerance
			if (isLHCasting && !spell.KeyPoints[LHNextPointID].LHComplete) { // keypoint 0 check required due to dual hand casting
				spell.canCast = CheckLHMoveTolerance(spell, LHNextPointID); // Disable canCast if left hand out of tolerance
			}

			// Set isScaleSet true if no longer in tolerance with end point of first move (NOTE: That point would be complete at this stage)
			if (!spell.isScaleSet && spell.canCast && ((RHNextPointID > LHNextPointID) ? RHNextPointID : LHNextPointID) > 0) {
				if (RHNextPointID > LHNextPointID) {
					if (spell.KeyPoints[RHNextPointID - 1].Motion != MoveType::Point && // If the previously completed keypoint was an end of a movement point AND
						!CheckRHStaticTolerance(spell, spell.KeyPoints[RHNextPointID - 1])) { // Orientation of previous keypoint no longer in tolerance
						spell.isScaleSet = true;
					}
				}
				else {
					if (spell.KeyPoints[LHNextPointID - 1].Motion != MoveType::Point && // If the previously completed keypoint was an end of a movement point AND
						!CheckLHStaticTolerance(spell, spell.KeyPoints[LHNextPointID - 1])) { // Orientation of previous keypoint no longer in tolerance
						spell.isScaleSet = true;
					}
				}
			}

			if (!spell.canCast) {
				UE_LOG(LogTemp, Warning, TEXT("Spell Deactivated: %s!!"), *UEnum::GetValueAsString(spell.ID));
			}
		}
	}
	return false;
}

// Displays/'hides' the spellcasting nodes depending on what is going on
// *** Currently Extremely inefficient, upgrade to much better code at somepoint ***
void USpellComponent::UpdateCastingNodes() {
	if ((isRHCasting || isLHCasting)) { // If any hand is casting
		// Update the visible casting node actors
		for (auto& spell : AllSpells) {
			int CurrentKP{ 0 };
			for (auto& kp : spell.KeyPoints) {
				if (spell.canCast) { // Show nodes
					if (isRHCasting) {
						if (kp.CastingNodeRH == nullptr) { // If casting node doesn't exist yet
							kp.CastingNodeRH = GetWorld()->SpawnActor<ACastingNode>(CastingNodeBlueprint, SpellcastingGridToWorld((kp.RHPosition * spell.Scale) + RHStartPos), kp.RHRotation + GetComponentRotation());
							if (spell.ID == SpellID::Wall || spell.ID == SpellID::Atune || spell.ID == SpellID::Beam) {
								kp.CastingNodeRH->StaticMesh->SetMaterial(0, kp.CastingNodeRH->OrangeMaterial);
							}
							else if (spell.ID == SpellID::Fire || spell.ID == SpellID::IncPwr || spell.ID == SpellID::IncDur || spell.ID == SpellID::Explode) {
								kp.CastingNodeRH->StaticMesh->SetMaterial(0, kp.CastingNodeRH->RedMaterial);
							}
							else if (spell.ID == SpellID::Water || spell.ID == SpellID::DecDur || spell.ID == SpellID::Air || spell.ID == SpellID::Magnet) {
								kp.CastingNodeRH->StaticMesh->SetMaterial(0, kp.CastingNodeRH->BlueMaterial);
							}
							else if (spell.ID == SpellID::Ball || spell.ID == SpellID::Earth) {
								kp.CastingNodeRH->StaticMesh->SetMaterial(0, kp.CastingNodeRH->GreenMaterial);
							}
							else if (spell.ID == SpellID::DecPwr) {
								kp.CastingNodeRH->StaticMesh->SetMaterial(0, kp.CastingNodeRH->PurpleMaterial);
							}
						}
						else { // Update position of casting nodes that are already in place
							kp.CastingNodeRH->SetActorLocation(SpellcastingGridToWorld((kp.RHPosition * spell.Scale) + RHStartPos));
							kp.CastingNodeRH->SetActorRotation(kp.RHRotation + GetComponentRotation());

							// Increase size of next keypoint in list
							if (CurrentKP > 0 && spell.KeyPoints[CurrentKP - 1].RHComplete && !kp.RHComplete) {
								kp.CastingNodeRH->StaticMesh->SetWorldScale3D(FVector{ 2, 2, 2 });
							}
							else {
								kp.CastingNodeRH->StaticMesh->SetWorldScale3D(FVector{ 1, 1, 1 });
							}
						}
					}
					else {
						if (kp.CastingNodeLH != nullptr) {
							kp.CastingNodeLH->SetActorLocation(FVector{ 0,0,-100 });
						}
					}
					if (isLHCasting) {
						if (kp.CastingNodeLH == nullptr) { // If casting node doesn't exist yet
							kp.CastingNodeLH = GetWorld()->SpawnActor<ACastingNode>(CastingNodeBlueprint, SpellcastingGridToWorld((kp.LHPosition * spell.Scale) + LHStartPos), kp.LHRotation + GetComponentRotation());
							if (spell.ID == SpellID::Wall || spell.ID == SpellID::Atune || spell.ID == SpellID::Beam) {
								kp.CastingNodeLH->StaticMesh->SetMaterial(0, kp.CastingNodeRH->OrangeMaterial);
							}
							else if (spell.ID == SpellID::Fire || spell.ID == SpellID::IncPwr || spell.ID == SpellID::IncDur || spell.ID == SpellID::Explode) {
								kp.CastingNodeLH->StaticMesh->SetMaterial(0, kp.CastingNodeLH->RedMaterial);
							}
							else if (spell.ID == SpellID::Water || spell.ID == SpellID::DecDur || spell.ID == SpellID::Air || spell.ID == SpellID::Magnet) {
								kp.CastingNodeLH->StaticMesh->SetMaterial(0, kp.CastingNodeLH->BlueMaterial);
							}
							else if (spell.ID == SpellID::Ball || spell.ID == SpellID::Earth) {
								kp.CastingNodeLH->StaticMesh->SetMaterial(0, kp.CastingNodeLH->GreenMaterial);
							}
							else if (spell.ID == SpellID::DecPwr) {
								kp.CastingNodeLH->StaticMesh->SetMaterial(0, kp.CastingNodeLH->PurpleMaterial);
							}
						}
						else { // Update position of casting nodes that are already in place
							kp.CastingNodeLH->SetActorLocation(SpellcastingGridToWorld((kp.LHPosition * spell.Scale) + LHStartPos));
							kp.CastingNodeLH->SetActorRotation(kp.LHRotation + GetComponentRotation());

							// Increase size of next keypoint in list
							if (CurrentKP > 0 && spell.KeyPoints[CurrentKP - 1].LHComplete && !kp.LHComplete) {
								kp.CastingNodeLH->StaticMesh->SetWorldScale3D(FVector{ 2, 2, 2 });
							}
							else {
								kp.CastingNodeLH->StaticMesh->SetWorldScale3D(FVector{ 1, 1, 1 });
							}
						}
					}
					else {
						if (kp.CastingNodeLH != nullptr) {
							kp.CastingNodeLH->SetActorLocation(FVector{ 0,0,-100 });
						}
					}
				}
				else { // Hide nodes
					if (kp.CastingNodeLH != nullptr) {
						kp.CastingNodeLH->SetActorLocation(FVector{ 0,0,-100 });
					}
					if (kp.CastingNodeRH != nullptr) {
						kp.CastingNodeRH->SetActorLocation(FVector{ 0,0,-100 });
					}
				}
				CurrentKP++;
			}
		}
	}
	else { // If player is not casting, hide all nodes
		for (auto& spell : AllSpells) {
			for (auto& kp : spell.KeyPoints) {
				if (kp.CastingNodeLH != nullptr) {
					kp.CastingNodeLH->SetActorLocation(FVector{ 0,0,-100 });
				}
				if (kp.CastingNodeRH != nullptr) {
					kp.CastingNodeRH->SetActorLocation(FVector{ 0,0,-100 });
				}
			}
		}
	}
}

void USpellComponent::EndCast() {
	if (isRHCasting && isLHCasting) { // If ended dualcasting
		SpellCastingController->ApplyDualHSpell(CurrentSpell);
		isLHCasting = false;
		isRHCasting = false;
		UE_LOG(LogTemp, Warning, TEXT("Dual Handed Spell Complete: %s"), *UEnum::GetValueAsString(CurrentSpell));
	}
	else if (isRHCasting && !isLHCasting) { // If ended RH casting
		SpellCastingController->ApplyRHSpell(CurrentSpell);
		isRHCasting = false;
		UE_LOG(LogTemp, Warning, TEXT("Right Handed Spell Complete: %s"), *UEnum::GetValueAsString(CurrentSpell));
	}
	else if (!isRHCasting && isLHCasting) { // If ended LH casting
		SpellCastingController->ApplyLHSpell(CurrentSpell);
		isLHCasting = false;
		UE_LOG(LogTemp, Warning, TEXT("Left Handed Spell Complete: %s"), *UEnum::GetValueAsString(CurrentSpell));
	}
	isComplete = false;
}

// By axis scaling is used in this project - so max movement in one axis sets the current scale
// Until the first movement is complete, then scale is 'set' until this round of casting is complete
void USpellComponent::UpdateSpellScale(FSpellData& spell, int kpID) {
	float NewScale{ MIN_MOVE_SCALE };
	float MaxMoveFromStart{ 0.f };

	if (spell.ID == SpellID::Air) { // Special case
		MaxMoveFromStart = (RHStartPos - LHStartPos).Size();
		NewScale = (MaxMoveFromStart > NewScale) ? MaxMoveFromStart : NewScale;
		spell.isScaleSet = true;
	}
	else {
		// Check if one axis' movement in relevant hands is above MIN_MOVE_SCALE
		if (isLHCasting) { // Check if movement from start in left hand is greater than current scale
			MaxMoveFromStart = (ToSpellcastingGrid(LHand->GetComponentLocation()) - LHStartPos).GetAbsMax();
			NewScale = (MaxMoveFromStart > NewScale) ? MaxMoveFromStart : NewScale;
		}
		if (isRHCasting) { // Check if movement from start in right hand is greater than current scale
			MaxMoveFromStart = (ToSpellcastingGrid(RHand->GetComponentLocation()) - RHStartPos).GetAbsMax();
			NewScale = (MaxMoveFromStart > NewScale) ? MaxMoveFromStart : NewScale;
		}
	}

	// Update Scale to maximum hand movement from start position
	spell.Scale = (spell.Scale > NewScale) ? spell.Scale : NewScale;
}

// Checks whether left hand start pos and right hand start pos are at the valid start position from each other
// i.e. should RH be above/next to/in front of LH, if yes, is it
// NOTE: Axis based calculation, will not work well for relPos FVec{0,1,1} where there is a 1 in more than one axis
// LHStartPos and RHStartPos must be set before this function is called
// Will always take MaxAbs(PosTolerance) for all axes - will never ignore 0 axes
bool USpellComponent::CheckRHToLHDirection(FSpellData& referenceSpell, FVector PosTolerance) {
	float Tolerance{ PosTolerance.GetAbsMax() };
	FVector actRelativePos{ RHStartPos - LHStartPos };

	if (referenceSpell.LtoRRelativeStartPos.X == 0) { // If x Axis position (in spellcasting grid) needs to be in tolerance
		if (actRelativePos.X < -Tolerance || actRelativePos.X > Tolerance) {
			return false;
		}
	}
	if (referenceSpell.LtoRRelativeStartPos.Y == 0) { // If y Axis position (in spellcasting grid) needs to be in tolerance
		if (actRelativePos.Y < -Tolerance || actRelativePos.Y > Tolerance) {
			return false;
		}
	}
	if (referenceSpell.LtoRRelativeStartPos.Z == 0) { // If z Axis position (in spellcasting grid) needs to be in tolerance
		if (actRelativePos.Z < -Tolerance || actRelativePos.Z > Tolerance) {
			return false;
		}
	}
	return true;
}

// Conversion functions - these decide which type of tolerance check is required, then convert to the relevant units... The logic brains of the operation

// Returns true if RH is within tolerance of relevant keypoint
bool USpellComponent::CheckRHStaticTolerance(FSpellData& spell, FKeyPoint& kp) {
	bool rotationPassed{};
	bool positionPassed{};

	FRotator handRotation{ ToSpellcastingGrid(RHand->GetComponentRotation()) };
	FVector handLocation{ ToSpellcastingGrid(RHand->GetComponentLocation()) };
	FVector requiredLocation{ (kp.RHPosition * spell.Scale) + RHStartPos };
	FVector posTolerance{ spell.PositionalTolerance * MAX_MOVE_TOLERANCE / 2};
	
	rotationPassed = PointEqual(handRotation, kp.RHRotation, spell.RotationalTolerance);
	positionPassed = PointEqual(handLocation, requiredLocation, posTolerance);

	/*if (!rotationPassed) {
		UE_LOG(LogTemp, Warning, TEXT("RH static point rotation failed\n     RH Rot: %s\n     KP Rot: %s\n     Rot Tolerance: %s"),
			*handRotation.ToString(), *kp.RHRotation.ToString(), *spell.RotationalTolerance.ToString());
	}
	if (!positionPassed) {
		UE_LOG(LogTemp, Warning, TEXT("RH static point position failed\n     RH Pos: %s\n     KP Pos: %s\n     Pos Tolerance: %s"),
			*handLocation.ToString(), *requiredLocation.ToString(), *posTolerance.ToString());
	}*/

	if (rotationPassed && positionPassed) {
		return true;
	}
	else {
		return false;
	}
}

// Returns true if LH is within tolerance of relevant keypoint
bool USpellComponent::CheckLHStaticTolerance(FSpellData& spell, FKeyPoint& kp) {
	bool rotationPassed{};
	bool positionPassed{};

	FRotator handRotation{ ToSpellcastingGrid(LHand->GetComponentRotation()) };
	FVector handLocation{ ToSpellcastingGrid(LHand->GetComponentLocation()) };
	FVector requiredLocation{ (kp.LHPosition * spell.Scale) + LHStartPos };
	FVector posTolerance{ spell.PositionalTolerance * MAX_MOVE_TOLERANCE / 2 };

	rotationPassed = PointEqual(handRotation, kp.LHRotation, spell.RotationalTolerance);
	positionPassed = PointEqual(handLocation, requiredLocation, posTolerance);

	if (rotationPassed && positionPassed) {
		return true;
	}
	else {
		return false;
	}
}

// Returns true if RH is within tolerance of current keypoint movement
// NOTE: Movement checks are done in unit space -> everything in spellgrid space must be converted
bool USpellComponent::CheckRHMoveTolerance(FSpellData& spell, int& kpID) {
	FKeyPoint& kp{ spell.KeyPoints[kpID] };
	FKeyPoint& kpPrev{ (kpID == 0) ? spell.KeyPoints[0] : spell.KeyPoints[kpID - 1] };

	// First check that hand rotation is in tolerance
	if (!RotationMoveInTolerance(
		ToSpellcastingGrid(RHand->GetComponentRotation()),
		kpPrev.RHRotation,
		kp.RHRotation,
		spell.RotationalTolerance)
		) {
		return false;
	}

	// Then check if any other movements are out
	switch (kp.Motion) {
	case MoveType::Point: // Position from this point to last point does not change
		if (!PointEqual( // Check if the hand has moved too far from the required position
			ToSpellcastingGrid(RHand->GetComponentLocation()),
			(kp.RHPosition * spell.Scale) + RHStartPos,
			spell.PositionalTolerance * MAX_MOVE_TOLERANCE
		)) {
			return false;
		}
		break;
	case MoveType::Line: // Position from this point to the last point does change (MAX TWO AXES!)
		if (!LineMoveInTolerance( // Check if the movement went out of bounds
			(ToSpellcastingGrid(RHand->GetComponentLocation()) - RHStartPos),
			kpPrev.RHPosition * spell.Scale,
			kp.RHPosition * spell.Scale,
			spell.PositionalTolerance * MAX_MOVE_TOLERANCE // The reason we use MAX_MOVE_TOLERANCE here, is to keep some kind of order to the world - this may need increasing, we shall see
		)) {
			//UE_LOG(LogTemp, Error, TEXT("\nRH move tolerance failed!\n     RHPos %s\n     StartPos: %s; EndPos %s\n     Tolerance: %s"),
			//	*(ToSpellcastingGrid(RHand->GetComponentLocation()) - RHStartPos).ToString(), *(kpPrev.RHPosition * spell.Scale).ToString(), *(kp.RHPosition * spell.Scale).ToString(), *(spell.PositionalTolerance * MAX_MOVE_TOLERANCE).ToString());
			return false;
		}
		break;
	}

	return true;
}

// Returns true if LH is within tolerance of current keypoint movement
// NOTE: Movement checks are done in unit space -> everything in spellgrid space must be converted
bool USpellComponent::CheckLHMoveTolerance(FSpellData& spell, int& kpID) {
	FKeyPoint& kp{ spell.KeyPoints[kpID] };
	FKeyPoint& kpPrev{ (kpID == 0) ? spell.KeyPoints[0] : spell.KeyPoints[kpID - 1] };

	// First check that hand rotation is in tolerance
	if (!RotationMoveInTolerance(
		ToSpellcastingGrid(LHand->GetComponentRotation()),
		kpPrev.LHRotation,
		kp.LHRotation,
		spell.RotationalTolerance)
		) {
		return false;
	}

	// Then check if any other movements are out
	switch (kp.Motion) {
	case MoveType::Point: // Position from this point to last point does not change
		if (!PointEqual( // Check if the hand has moved too far from the required position
			ToSpellcastingGrid(LHand->GetComponentLocation()),
			(kp.LHPosition * spell.Scale) + LHStartPos,
			spell.PositionalTolerance * MAX_MOVE_TOLERANCE
		)) {
			return false;
		}
		break;
	case MoveType::Line: // Position from this point to the last point does change (MAX TWO AXES!)
		if (!LineMoveInTolerance( // Check if the movement went out of bounds
			(ToSpellcastingGrid(LHand->GetComponentLocation()) - LHStartPos),
			kpPrev.LHPosition * spell.Scale,
			kp.LHPosition * spell.Scale,
			spell.PositionalTolerance * MAX_MOVE_TOLERANCE // The reason we use MAX_MOVE_TOLERANCE here, is to keep some kind of order to the world - this may need increasing, we shall see
		)) {
			//UE_LOG(LogTemp, Error, TEXT("\nLH move tolerance failed!\n     LHPos %s\n     StartPos: %s; EndPos %s\n     Tolerance: %s"),
			//	*(ToSpellcastingGrid(LHand->GetComponentLocation()) - LHStartPos).ToString(), *(kpPrev.LHPosition * spell.Scale).ToString(), *(kp.LHPosition * spell.Scale).ToString(), *(spell.PositionalTolerance * MAX_MOVE_TOLERANCE).ToString());
			return false;
		}
		break;
	}

	return true;
}

// Calculcation functions... The maths brains of the operation

// Checks if a position is within tolerance +/- of another position
// Ignores axes where tolerance.axis == 0
bool USpellComponent::PointEqual(FVector posToCheck, FVector refPos, FVector posTolerance) {
	FVector RelativePos{ posToCheck - refPos };

	if (posTolerance.X != 0) {
		if (RelativePos.X > posTolerance.X || RelativePos.X < -posTolerance.X) { // If relative X position is too far away
			return false;
		}
	}
	if (posTolerance.Y != 0) {
		if (RelativePos.Y > posTolerance.Y || RelativePos.Y < -posTolerance.Y) { // If relative Y position is too far away
			return false;
		}
	}
	if (posTolerance.Z != 0) {
		if (RelativePos.Z > posTolerance.Z || RelativePos.Z < -posTolerance.Z) { // If relative Z position is too far away
			return false;
		}
	}

	return true;
}

// Checks if a rotation is within tolerance +/- of another rotation
// Ignores axes where tolerance.axis == 0
bool USpellComponent::PointEqual(FRotator rotToCheck, FRotator refRot, FRotator rotTolerance) {
	
	FRotator RelativeRot{ rotToCheck - refRot };

	// The actual check
	if (rotTolerance.Pitch != 0) {
		if (RelativeRot.Pitch > rotTolerance.Pitch || RelativeRot.Pitch < -rotTolerance.Pitch) { // If relative Pitch position is too far away
			//UE_LOG(LogTemp, Error, TEXT("\nPoint pitch tolerance failed!\n     Pitch to check: %f\n     Ref Pitch: %f\n     Tolerance: %f"), rotToCheck.Pitch, refRot.Pitch, rotTolerance.Pitch)
			return false;
		}
	}
	if (rotTolerance.Yaw != 0) {
		if (RelativeRot.Yaw > rotTolerance.Yaw || RelativeRot.Yaw < -rotTolerance.Yaw) { // If relative Yaw position is too far away
			//UE_LOG(LogTemp, Error, TEXT("\nPoint yaw tolerance failed!\n     yaw to check: %f\n     Ref yaw: %f\n     Tolerance: %f"), rotToCheck.Yaw, refRot.Yaw, rotTolerance.Yaw)
			return false;
		}
	}
	if (rotTolerance.Roll != 0) {
		if (RelativeRot.Roll > rotTolerance.Roll || RelativeRot.Roll < -rotTolerance.Roll) { // If relative Roll position is too far away
			//UE_LOG(LogTemp, Error, TEXT("\nPoint Roll tolerance failed!\n     Roll to check: %f\n     Ref Roll: %f\n     Tolerance: %f"), rotToCheck.Roll, refRot.Roll, rotTolerance.Roll)
			return false;
		}
	}

	return true;
}

// Checks if two floats are within tolerance of each other
bool USpellComponent::PointEqual(float PointToCheck, float refPoint, float PointTolerance) {
	if (PointToCheck > refPoint + PointTolerance ||
		PointToCheck < refPoint - PointTolerance) {
		return false;
	}
	return true;
}

// Checks if RotToCheck is within the bounds created by StartRot and EndRot and tolerance
bool USpellComponent::RotationMoveInTolerance(FRotator RotToCheck, FRotator StartRot, FRotator EndRot, FRotator RotTolerance) {
	// Pivot Check (twist your wrist)
	FRotator MinRot{};
	FRotator MaxRot{};
	FRotator PointRelativeRot{ EndRot - StartRot };

	// Check tolerance for each axis
	if (RotTolerance.Pitch != 0) { // If pitch axis needs checking
		if (PointRelativeRot.Pitch == 0) { // If there is no movement from A->B in pitch axis
			MinRot.Pitch = StartRot.Pitch - RotTolerance.Pitch;
			MaxRot.Pitch = StartRot.Pitch + RotTolerance.Pitch;
		}
		else { // If there is movement from point a -> point b in pitch axis
			if (PointRelativeRot.Pitch < 0) {
				MinRot.Pitch = EndRot.Pitch - RotTolerance.Pitch;
				MaxRot.Pitch = StartRot.Pitch + RotTolerance.Pitch;
			}
			else { // PointRelativeRot.Pitch > 0
				MinRot.Pitch = StartRot.Pitch - RotTolerance.Pitch;
				MaxRot.Pitch = EndRot.Pitch + RotTolerance.Pitch;
			}
		}
		// Return false if out of bounds
		if (RotToCheck.Pitch < MinRot.Pitch || RotToCheck.Pitch > MaxRot.Pitch) {
			//UE_LOG(LogTemp, Error, TEXT("Pitch motion rot (%f) out of bounds\n     Min allowed: %f\n     Max allowed: %f"), RotToCheck.Pitch, MinRot.Pitch, MaxRot.Pitch);
			return false;
		}
	}

	if (RotTolerance.Yaw != 0) { // If yaw axis needs checking
		if (PointRelativeRot.Yaw == 0) { // If there is no movement from A->B in yaw axis
			MinRot.Yaw = StartRot.Yaw - RotTolerance.Yaw;
			MaxRot.Yaw = StartRot.Yaw + RotTolerance.Yaw;
		}
		else { // If there is movement from point a -> point b in yaw axis
			if (PointRelativeRot.Yaw < 0) {
				MinRot.Yaw = EndRot.Yaw - RotTolerance.Yaw;
				MaxRot.Yaw = StartRot.Yaw + RotTolerance.Yaw;
			}
			else { // PointRelativeRot.Yaw > 0
				MinRot.Yaw = StartRot.Yaw - RotTolerance.Yaw;
				MaxRot.Yaw = EndRot.Yaw + RotTolerance.Yaw;
			}
		}
		// Return false if out of bounds
		if (RotToCheck.Yaw < MinRot.Yaw || RotToCheck.Yaw > MaxRot.Yaw) {
			//UE_LOG(LogTemp, Error, TEXT("Yaw motion rot (%f) out of bounds\n     Min allowed: %f\n     Max allowed: %f"), RotToCheck.Yaw, MinRot.Yaw, MaxRot.Yaw);
			return false;
		}
	}

	if (RotTolerance.Roll != 0) { // If Roll axis needs checking
		if (PointRelativeRot.Roll == 0) { // If there is no movement from A->B in Roll axis
			MinRot.Roll = StartRot.Roll - RotTolerance.Roll;
			MaxRot.Roll = StartRot.Roll + RotTolerance.Roll;
		}
		else { // If there is movement from point a -> point b in Roll axis
			if (PointRelativeRot.Roll < 0) {
				MinRot.Roll = EndRot.Roll - RotTolerance.Roll;
				MaxRot.Roll = StartRot.Roll + RotTolerance.Roll;
			}
			else { // PointRelativeRot.Roll > 0
				MinRot.Roll = StartRot.Roll - RotTolerance.Roll;
				MaxRot.Roll = EndRot.Roll + RotTolerance.Roll;
			}
		}

		// Return false if out of bounds
		if (RotToCheck.Roll < MinRot.Roll || RotToCheck.Roll > MaxRot.Roll) {
			//UE_LOG(LogTemp, Error, TEXT("Roll motion rot (%f) out of bounds\n     Min allowed: %f\n     Max allowed: %f"), RotToCheck.Roll, MinRot.Roll, MaxRot.Roll);
			return false;
		}
	}

	return true; // passed all rotational movement checks
}

// Checks if a position is within the bounds created by start pos, end pos and tolerance
// All passed in values must be in unit co-ordinates (ie (worldsize/scale))
bool USpellComponent::LineMoveInTolerance(FVector PosToCheck, FVector StartPos, FVector EndPos, FVector PosTolerance) {
	/* This function performs the following logic:
	* For every axis where tolerance != 0:
	*	Where it is the only axis that changes from a->b check it is in tolerance
	*	If it moves along with one other axis check that the relative movement of both of these is in tolerance (i.e. DeltaX = 3, DeltaY = 5)
	*/

	FVector DeltaPos{ EndPos - StartPos }; // The required motion in unit space from startpos
	FVector RelativePos{ PosToCheck - StartPos }; // Relative position in grid space from strartpos
	
	FVector MinPos{ ((StartPos.X < EndPos.X) ? StartPos.X : EndPos.X) - PosTolerance.X,
					((StartPos.Y < EndPos.Y) ? StartPos.Y : EndPos.Y) - PosTolerance.Y,
					((StartPos.Z < EndPos.Z) ? StartPos.Z : EndPos.Z) - PosTolerance.Z };
	FVector MaxPos{ ((StartPos.X > EndPos.X) ? StartPos.X : EndPos.X) + PosTolerance.X,
					((StartPos.Y > EndPos.Y) ? StartPos.Y : EndPos.Y) + PosTolerance.Y,
					((StartPos.Z > EndPos.Z) ? StartPos.Z : EndPos.Z) + PosTolerance.Z };

	if (PosTolerance.X != 0) { // If x axis needs checking
		if (DeltaPos.X == 0) { // If x value does not change during movement
			if (!PointEqual(PosToCheck.X, StartPos.X, PosTolerance.X)) { // If the tolerance check failes
				//UE_LOG(LogTemp, Error, TEXT("X Move Point Fail"));
				return false;
			}
		}
		else { // if X does change, check if any other axis is moving - NOTE: Spell setup states that a maximum of two axes will be moving - therefore no further checks are needed for all three
			if (PosTolerance.Y != 0 && DeltaPos.Y != 0) { // if the other moving axis is Y axis
				// If width out of tolerance
				if (!DiagonalMoveInTolerance(RelativePos.X, RelativePos.Y, DeltaPos.X, DeltaPos.Y, (PosTolerance.X == 0) ? PosTolerance.Y : PosTolerance.X)) {
					//UE_LOG(LogTemp, Error, TEXT("XY Move Diag Fail\n     RelativePos: %s\n     Delta Pos: %s"), *RelativePos.ToString(), *DeltaPos.ToString());
					return false;
				}
				// If length out of tolerance
				if (PosToCheck.Y > MaxPos.Y || PosToCheck.Y < MinPos.Y || PosToCheck.X > MaxPos.X || PosToCheck.X < MinPos.X) {
					//UE_LOG(LogTemp, Error, TEXT("XY Move Diag Length Fail"));
					return false;
				}
			}
			else if (PosTolerance.Z != 0 && DeltaPos.Z != 0) { // if the other moving axis is Z axis
				// If width out of tolerance
				if (!DiagonalMoveInTolerance(RelativePos.X, RelativePos.Z, DeltaPos.X, DeltaPos.Z, (PosTolerance.X == 0) ? PosTolerance.Z : PosTolerance.X)) {
					//UE_LOG(LogTemp, Error, TEXT("XZ Move Diag Fail"));
					return false;
				}
				// If length out of tolerance
				if (PosToCheck.Z > MaxPos.Z || PosToCheck.Z < MinPos.Z || PosToCheck.X > MaxPos.X || PosToCheck.X < MinPos.X) {
					//UE_LOG(LogTemp, Error, TEXT("XZ Move Diag Length Fail\n     RelativePos: %s\n     Delta Pos: %s"), *RelativePos.ToString(), *DeltaPos.ToString());
					return false;
				}
			}
			else { // if X is the only moving axis - check that movement is not outside max/min
				if (PosToCheck.X < MinPos.X || PosToCheck.X > MaxPos.X) {
					//UE_LOG(LogTemp, Error, TEXT("X Move Straight Length Fail"));
					return false;
				}
			}
		}
	}

	if (PosTolerance.Y != 0) { // If Y axis needs checking - NOTE: X axis has been checked...
		if (DeltaPos.Y == 0) { // If Y value does not change during movement
			if (!PointEqual(PosToCheck.Y, StartPos.Y, PosTolerance.Y)) { // If the tolerance check failes
				//UE_LOG(LogTemp, Error, TEXT("Y Move Point Fail"));
				return false;
			}
		}
		else { // Y does change
			if (PosTolerance.Z != 0 && DeltaPos.Z != 0) { // if the other moving axis is Z (NOTE: X->Y has already been verified)
				if (!DiagonalMoveInTolerance(RelativePos.Y, RelativePos.Z, DeltaPos.Y, DeltaPos.Z, (PosTolerance.Y == 0) ? PosTolerance.Z : PosTolerance.Y)) {
					//UE_LOG(LogTemp, Error, TEXT("YZ Move Diag Fail\n     RelativePos: %s\n     Delta Pos: %s"), *RelativePos.ToString(), *DeltaPos.ToString());
					return false;
				}
				// If length out of tolerance
				if (PosToCheck.Y > MaxPos.Y || PosToCheck.Y < MinPos.Y || PosToCheck.Z > MaxPos.Z || PosToCheck.Z < MinPos.Z) {
					//UE_LOG(LogTemp, Error, TEXT("YZ Move Diag Length Fail"));
					return false;
				}
			}
			else if ((PosTolerance.X != 0 && DeltaPos.X == 0) || PosTolerance.X == 0) { // If Y is the only moving axis
				if (PosToCheck.Y < MinPos.Y || PosToCheck.Y > MaxPos.Y) {
					//UE_LOG(LogTemp, Error, TEXT("Y Move Straight Length Fail"));
					return false;
				}
			}
		}
	}

	if (PosTolerance.Z != 0) { // If Z axis needs checking - NOTE: X && Y have both been checked
		if (DeltaPos.Z == 0) { // If Z value does not change during movement
			if (!PointEqual(PosToCheck.Z, StartPos.Z, PosTolerance.Z)) { // If the tolerance check failes
				//UE_LOG(LogTemp, Error, TEXT("Z Move Point Fail"));
				return false;
			}
		}
		else { // Z does change
			// NOTE: X->Z && Y->Z have already been checked
			if (((PosTolerance.X != 0 && DeltaPos.X == 0) || PosTolerance.X == 0) && 
				((PosTolerance.Y != 0 && DeltaPos.Y == 0) || PosTolerance.Y == 0)) { // If Z is the only moving axis
				if (PosToCheck.Z < MinPos.Z || PosToCheck.Z > MaxPos.Z) {
					//UE_LOG(LogTemp, Error, TEXT("Z Move Straight Length Fail"));
					return false;
				}
			}
		}
	}

	return true;
}

// Part of LineMoveInTolerance()
bool USpellComponent::DiagonalMoveInTolerance(float iPos, float jPos, float DeltaI, float DeltaJ, float WidthTolerance) {
	
	float actMovLen{ FMath::Sqrt(FMath::Square(iPos) + FMath::Square(jPos)) }; // Length of hand movement from startPos
	float theta{ FMath::Atan(DeltaJ / DeltaI) }; // Angle from axis I of required movement
	float idealI{ actMovLen * FMath::Cos(theta) }; // Where i would be on ideal line
	float idealJ{ actMovLen * FMath::Sin(theta) }; // Where j would be on ideal line

	// Fix lack of negativity
	if (DeltaI < 0) {
		idealI = (idealI < 0) ? idealI : - idealI;
	}
	else {
		idealI = (idealI > 0) ? idealI : -idealI;
	}
	if (DeltaJ < 0) {
		idealJ = (idealJ < 0) ? idealJ : -idealJ;
	}
	else {
		idealJ = (idealJ > 0) ? idealJ : -idealJ;
	}

	// Perform tolerance check (basically a positional check at this point)
	if (!PointEqual(iPos, idealI, WidthTolerance) ||
		!PointEqual(jPos, idealJ, WidthTolerance)) {
		return false;
	}

	return true;
}

/*
// Checks if an arc motion is in tolerance
// NOTE: All values must be passed in unit scale
bool USpellComponent::ArcMoveInTolerance(FVector PosToCheck, FVector StartPos, FVector EndPos, FVector PosTolerance, MoveType Quadrant) {	
	FVector RefRelativePos{ EndPos - StartPos }; // As it is an arc movement, there will always be only one axis == 0 here (we are not going 3D with the movements - just the tolerances)
	FVector ChkRelativePos{ PosToCheck - StartPos };
	int QuadMod{ 0 };
	// There are three variations -> either DeltaX = 0, DeltaY = 0 or Delta Z = 0
	// For each variation there are four quadrants in a circle, we need to figure out which quadrant we are in (which 90Deg)	
	// Then we can calculate based on that quadrant's arc type
	
	switch (Quadrant) {
	case MoveType::Arc1:
		QuadMod = 0;
		break;
	case MoveType::Arc2:
		QuadMod = 1;
		break;
	case MoveType::Arc3:
		QuadMod = 2;
		break;
	case MoveType::Arc4:
		QuadMod = 3;
	}


	if (RefRelativePos.X == 0) { // Variation 1: Movement is in YZ plane (RL/UD) - Z = sin(Y * 90) - Quadrant in terms of sine Y
		
	}
	else if (RefRelativePos.Y == 0) { // Variation 2: Movement is in XZ plane (FB/UD) - Z = sin(X * 90) - quadrant in terms of sine X

	}
	else { // Variation 3: Movement is in XY plane (FB/RL) - X = sin(Y * 90)- currently no spells use this variation

	}

	return true;
}
*/

// *** DEV SECTION *** //
void USpellComponent::RunDevTests() {
	if (!isLoggingLHData) {
		if (isLHCasting) { // If LH casting started
			// Setup LH Logging variables
			FVector curPos{ ToSpellcastingGrid(LHand->GetComponentLocation()) };
			FRotator curRot{ ToSpellcastingGrid(LHand->GetComponentRotation()) };

			MinLHPos = FVector{ curPos };
			MaxLHPos = FVector{ curPos };
			MinLHRot = FRotator{ curRot };
			MaxLHRot = FRotator{ curRot };

			isLoggingLHData = true;
		}
	}
	else {
		if (!isLHCasting) { // If LH casting completed
			SendMoveDetailsToLog(MinLHPos, MaxLHPos, MinLHRot, MaxLHRot, false);
			isLoggingLHData = false;
		}
	}
	if (!isLoggingRHData) {
		if (isRHCasting) { // If RH casting started
			// Setup RH Logging variables
			FVector curPos{ ToSpellcastingGrid(RHand->GetComponentLocation()) };
			FRotator curRot{ ToSpellcastingGrid(RHand->GetComponentRotation()) };

			MinRHPos = FVector{ curPos };
			MaxRHPos = FVector{ curPos };
			MinRHRot = FRotator{ curRot };
			MaxRHRot = FRotator{ curRot };

			isLoggingRHData = true;
		}
	}
	else {
		if (!isRHCasting) { // If RH casting completed
			SendMoveDetailsToLog(MinRHPos, MaxRHPos, MinRHRot, MaxRHRot, true);
			isLoggingRHData = false;
		}
	}

	if (isLoggingLHData || isLoggingRHData) { // Minor performance saver
		UpdateMoveDetails();
	}
}

// Updates maximum motion details for casting log output
void USpellComponent::UpdateMoveDetails() {
	if (isLHCasting) {
		FVector handPos{ ToSpellcastingGrid(LHand->GetComponentLocation()) };
		FRotator handRot{ ToSpellcastingGrid(LHand->GetComponentRotation()) };

		MinLHPos.X = (MinLHPos.X < handPos.X) ? MinLHPos.X : handPos.X;
		MaxLHPos.X = (MaxLHPos.X > handPos.X) ? MaxLHPos.X : handPos.X;
		MinLHPos.Y = (MinLHPos.Y < handPos.Y) ? MinLHPos.Y : handPos.Y;
		MaxLHPos.Y = (MaxLHPos.Y > handPos.Y) ? MaxLHPos.Y : handPos.Y;
		MinLHPos.Z = (MinLHPos.Z < handPos.Z) ? MinLHPos.Z : handPos.Z;
		MaxLHPos.Z = (MaxLHPos.Z > handPos.Z) ? MaxLHPos.Z : handPos.Z;

		MinLHRot.Pitch = (MinLHRot.Pitch < handRot.Pitch) ? MinLHRot.Pitch : handRot.Pitch;
		MaxLHRot.Pitch = (MaxLHRot.Pitch > handRot.Pitch) ? MaxLHRot.Pitch : handRot.Pitch;
		MinLHRot.Yaw = (MinLHRot.Yaw < handRot.Yaw) ? MinLHRot.Yaw : handRot.Yaw;
		MaxLHRot.Yaw = (MaxLHRot.Yaw > handRot.Yaw) ? MaxLHRot.Yaw : handRot.Yaw;
		MinLHRot.Roll = (MinLHRot.Roll < handRot.Roll) ? MinLHRot.Roll : handRot.Roll;
		MaxLHRot.Roll = (MaxLHRot.Roll > handRot.Roll) ? MaxLHRot.Roll : handRot.Roll;
	}
	if (isRHCasting) {
		FVector handPos{ ToSpellcastingGrid(RHand->GetComponentLocation()) };
		FRotator handRot{ ToSpellcastingGrid(RHand->GetComponentRotation()) };

		MinRHPos.X = (MinRHPos.X < handPos.X) ? MinRHPos.X : handPos.X;
		MaxRHPos.X = (MaxRHPos.X > handPos.X) ? MaxRHPos.X : handPos.X;
		MinRHPos.Y = (MinRHPos.Y < handPos.Y) ? MinRHPos.Y : handPos.Y;
		MaxRHPos.Y = (MaxRHPos.Y > handPos.Y) ? MaxRHPos.Y : handPos.Y;
		MinRHPos.Z = (MinRHPos.Z < handPos.Z) ? MinRHPos.Z : handPos.Z;
		MaxRHPos.Z = (MaxRHPos.Z > handPos.Z) ? MaxRHPos.Z : handPos.Z;

		MinRHRot.Pitch = (MinRHRot.Pitch < handRot.Pitch) ? MinRHRot.Pitch : handRot.Pitch;
		MaxRHRot.Pitch = (MaxRHRot.Pitch > handRot.Pitch) ? MaxRHRot.Pitch : handRot.Pitch;
		MinRHRot.Yaw = (MinRHRot.Yaw < handRot.Yaw) ? MinRHRot.Yaw : handRot.Yaw;
		MaxRHRot.Yaw = (MaxRHRot.Yaw > handRot.Yaw) ? MaxRHRot.Yaw : handRot.Yaw;
		MinRHRot.Roll = (MinRHRot.Roll < handRot.Roll) ? MinRHRot.Roll : handRot.Roll;
		MaxRHRot.Roll = (MaxRHRot.Roll > handRot.Roll) ? MaxRHRot.Roll : handRot.Roll;
	}
}

void USpellComponent::SendMoveDetailsToLog(FVector MinPos, FVector MaxPos, FRotator MinRot, FRotator MaxRot, bool isRH) {
	if (FileManager.FileExists(*FileID)) {
		FileOutput = ""; // Clear file output
		FFileHelper::LoadFileToString(FileOutput, *FileID); // Load the current file contents
		
		castNo++;
		FString line1{ FString::FromInt(castNo) };
		line1.Append((isRH) ? " Right" : " Left");

		FileOutput.Append(line1);
		FileOutput.Append(TEXT(" Hand Movement Complete...\n     Greatest Movement: ")); FileOutput.Append((MaxPos - MinPos).ToString());
		FileOutput.Append(TEXT("\n     Greatest Rotation: ")); FileOutput.Append((MaxRot - MinRot).ToString());
		FileOutput.Append(TEXT("\n     Min Recorded Position: ")); FileOutput.Append(MinPos.ToString());
		FileOutput.Append(TEXT("\n     Max Recorded Position: ")); FileOutput.Append(MaxPos.ToString());
		FileOutput.Append(TEXT("\n     Min Recorded Rotation: ")); FileOutput.Append(MinRot.ToString());
		FileOutput.Append(TEXT("\n     Max Recorded Rotation: ")); FileOutput.Append(MaxRot.ToString());
		FileOutput.Append(TEXT("\n\n"));

		FFileHelper::SaveStringToFile(FileOutput, *FileID);
	}
}