// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "SpellCastingController.h"
#include "MotionControllerComponent.h"
#include "Spell_Wall.h"
#include "Spell_Ball.h"
#include "Spell_Beam.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
USpellCastingController::USpellCastingController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void USpellCastingController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void USpellCastingController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	// Zero all elements if no spell is set
	if (ActiveRHSpell == SpellID::None && ActiveLHSpell == SpellID::None) {
		ActiveElement = SpellID::None;
		ActiveModifier = SpellID::None;
	}
}

void USpellCastingController::ApplyRHSpell(SpellID spell)
{
	UE_LOG(LogTemp, Warning, TEXT("Applying RH Spell: %s"), *UEnum::GetValueAsString(spell));
	switch (spell) {
	case SpellID::Wall:
		if (ActiveRHSpell != SpellID::Wall) { // If wall is not the current spell
			if (ActiveRHSpell != SpellID::None) { // If there is a currently active spell
				// Remove current actor
			}

			// Add correct wall actor to RH (includes element and modifier)

			ActiveRHSpell = SpellID::Wall;
			UE_LOG(LogTemp, Warning, TEXT("\nWall Spell Applied in SCController\n "));
		}
		break;
	case SpellID::Ball:
		if (ActiveRHSpell != SpellID::Ball) { // If ball is not the current spell
			if (ActiveRHSpell != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated ball animation to RH (includes element and modifier)

			ActiveRHSpell = SpellID::Ball;
		}
		break;
	case SpellID::Beam:
		if (ActiveRHSpell != SpellID::Beam) { // If Beam is not the current spell
			if (ActiveRHSpell != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated Beam animation to RH (includes element and modifier)

			ActiveRHSpell = SpellID::Beam;
		}
		break;
	case SpellID::Atune:
		if (ActiveRHSpell != SpellID::Atune) { // If Atune is not the current spell
			if (ActiveRHSpell != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated Atune animation to RH (includes element and modifier)

			ActiveRHSpell = SpellID::Atune;
		}
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("\nIncorrect SpellID for ApplyRHSpell(): %s\n "), *UEnum::GetValueAsString(spell));
		break;
	}
}

void USpellCastingController::ApplyLHSpell(SpellID spell)
{
	UE_LOG(LogTemp, Warning, TEXT("Applying LH Spell: %s"), *UEnum::GetValueAsString(spell));
	switch (spell) {
	case SpellID::Wall:
		if (ActiveLHSpell != SpellID::Wall) { // If wall is not the current spell
			if (ActiveLHSpell != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated wall animation to LH (includes element and modifier)

			ActiveLHSpell = SpellID::Wall;
		}
		break;
	case SpellID::Ball:
		if (ActiveLHSpell != SpellID::Ball) { // If ball is not the current spell
			if (ActiveLHSpell != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated ball animation to LH (includes element and modifier)

			ActiveLHSpell = SpellID::Ball;
		}
		break;
	case SpellID::Beam:
		if (ActiveLHSpell != SpellID::Beam) { // If Beam is not the current spell
			if (ActiveLHSpell != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated Beam animation to LH (includes element and modifier)

			ActiveLHSpell = SpellID::Beam;
		}
		break;
	case SpellID::Atune:
		if (ActiveLHSpell != SpellID::Atune) { // If Atune is not the current spell
			if (ActiveLHSpell != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated Atune animation to LH (includes element and modifier)

			ActiveLHSpell = SpellID::Atune;
		}
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("\nIncorrect SpellID for ApplyLHSpell(): %S\n "), *UEnum::GetValueAsString(spell));
		break;
	}
}

void USpellCastingController::ApplyDualHSpell(SpellID spell)
{
	UE_LOG(LogTemp, Warning, TEXT("Applying DualH Spell: %s"), *UEnum::GetValueAsString(spell));
	switch (spell) {
	case SpellID::Wall: // || SpellID::Ball || SpellID::Beam || SpellID::Atune: this does not work?// If any of the base spells
		ApplyRHSpell(spell);
		ApplyLHSpell(spell);
		break;
	case SpellID::Ball: // If any of the base spells
		ApplyRHSpell(spell);
		ApplyLHSpell(spell);
		break;
	case SpellID::Beam: // If any of the base spells
		ApplyRHSpell(spell);
		ApplyLHSpell(spell);
		break;
	case SpellID::Atune: // If any of the base spells
		ApplyRHSpell(spell);
		ApplyLHSpell(spell);
		break;
	case SpellID::Air:
		if (ActiveElement != SpellID::Air) { // If Air is not the current spell
			if (ActiveElement != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated Air animation to both hands (includes element and modifier)

			ActiveElement = SpellID::Air;
		}
		break;
	case SpellID::Water:
		if (ActiveElement != SpellID::Water) { // If Water is not the current spell
			if (ActiveElement != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated Water animation to both hands (includes element and modifier)

			ActiveElement = SpellID::Water;
		}
		break;
	case SpellID::Earth:
		if (ActiveElement != SpellID::Earth) { // If Earth is not the current spell
			if (ActiveElement != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated Earth animation to both hands (includes element and modifier)

			ActiveElement = SpellID::Earth;
		}
		break;
	case SpellID::Fire:
		if (ActiveElement != SpellID::Fire) { // If Fire is not the current spell
			if (ActiveElement != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated Fire animation to both hands (includes element and modifier)

			ActiveElement = SpellID::Fire;
		}
		break;
	case SpellID::IncPwr:
		if (ActiveModifier != SpellID::IncPwr) { // If IncPwr is not the current spell
			if (ActiveModifier != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated IncPwr animation to both hands (includes element and modifier)

			ActiveModifier = SpellID::IncPwr;
		}
		break;
	case SpellID::DecPwr:
		if (ActiveModifier != SpellID::DecPwr) { // If DecPwr is not the current spell
			if (ActiveModifier != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated DecPwr animation to both hands (includes element and modifier)

			ActiveModifier = SpellID::DecPwr;
		}
		break;
	case SpellID::IncDur:
		if (ActiveModifier != SpellID::IncDur) { // If IncDur is not the current spell
			if (ActiveModifier != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated IncDur animation to both hands (includes element and modifier)

			ActiveModifier = SpellID::IncDur;
		}
		break;
	case SpellID::DecDur:
		if (ActiveModifier != SpellID::DecDur) { // If DecDur is not the current spell
			if (ActiveModifier != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated DecDur animation to both hands (includes element and modifier)

			ActiveModifier = SpellID::DecDur;
		}
		break;
	case SpellID::Explode:
		if (ActiveModifier != SpellID::Explode) { // If Explode is not the current spell
			if (ActiveModifier != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated Explode animation to both hands (includes element and modifier)

			ActiveModifier = SpellID::Explode;
		}
		break;
	case SpellID::Magnet:
		if (ActiveModifier != SpellID::Magnet) { // If Magnet is not the current spell
			if (ActiveModifier != SpellID::None) { // If there is a currently active spell
				// Remove current animation
			}

			// Add updated Magnet animation to both hands (includes element and modifier)

			ActiveModifier = SpellID::Magnet;
		}
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("That DualH spell can not be applied... %s"), *UEnum::GetValueAsString(spell));
		break;
	}
}

void USpellCastingController::LaunchRHSpell()
{
	UE_LOG(LogTemp, Warning, TEXT("Launching RH Spell: %s"), *UEnum::GetValueAsString(ActiveRHSpell));

	if (ActiveRHSpell != SpellID::None) { // i.e. there is a spell in RH
		// Commence RHSpell launch

		UWorld* TheWorld{ GetWorld() };
		if (TheWorld) { // If relevant objects exist

			// Spawn setup data
			FVector DesiredStartPos{ RHand->GetComponentLocation() };
			FRotator DesiredStartRotation{ (DesiredStartPos - hmdCamera->GetComponentLocation()).Rotation() }; // Relative from eyes to start pos
			FTransform StartTransform{ DesiredStartRotation, DesiredStartPos, FVector{0.01f, 0.01f, 0.01f} }; // Rotation, Location, Scale
			FActorSpawnParameters spawnParams{};
			
			// Figure out where player is pointing the spell and set targetPos to that location
			// Set up linetrace for spell target
			FHitResult HitResult;
			FVector StartPos{ hmdCamera->GetComponentLocation() };
			FCollisionQueryParams LineTraceParams{ FName{}, false, GetOwner() }; // No name required, not complex collision, ignore player character

			// Spawn type selector
			switch (ActiveRHSpell) {
			case SpellID::Wall:
				// Check RH wall limit
				for (size_t i{}; i < 2; i++) {
					//bool hasWall{ true };
					//try {
					//	if (ActiveRHWalls[i] == nullptr || !IsValid(ActiveRHWalls[i]) || !ActiveRHWalls[i]->IsValidLowLevel()) {
					//		hasWall = false;
					//	}
					//}
					//catch (...) {
					//	hasWall = false;
					//}
					//if (!hasWall) { // If there is an available slot for wall
						// Spawn the wall with relevant setup parameters
						if (WallSpell_BP) {
							// Spawn new actor
							ActiveRHWalls[i] = TheWorld->SpawnActor<ASpell_Wall>(WallSpell_BP, StartTransform, spawnParams);
							// Set actor rotation pitch/yaw to desired aimpoint location of player
							ActiveRHWalls[i]->SetActorRotation((DesiredStartPos - StartPos).Rotation());

							// Set line trace endpoint to direction player is pointing at + range of spell
							FVector EndPos{ StartPos + (DesiredStartRotation.Vector() * ActiveRHWalls[i]->SpellRange) };

							// Get LineTrace Result if collided
							if (TheWorld->LineTraceSingleByObjectType(
								OUT HitResult,
								StartPos,
								EndPos,
								ECollisionChannel::ECC_WorldStatic,
								LineTraceParams)) {
								// Finalise the spell setup
								ActiveRHWalls[i]->SpellSetup(ActiveElement, ActiveModifier, HitResult.ImpactPoint, false, 0.2);
							}
							else {
								ActiveRHWalls[i]->Destroy();
								UE_LOG(LogTemp, Warning, TEXT("SpellCastingController.LineTrace failed to find a valid end position for Spell_Wall"));
								return; // prevents ActiveRHSpell from being changed - i.e. player keeps spell if it failed to launch due to bad targetting
							}
						}
						else {
							UE_LOG(LogTemp, Error, TEXT("Failed to find Spell_Wall_BP"));
						}
						ActiveRHSpell = SpellID::None;
						break; // Out of for loop
					//}
				}
				break;
			case SpellID::Ball:
				if (BallSpell_BP) {
					// Spawn new actor
					ASpell_Ball* NewBall = TheWorld->SpawnActor<ASpell_Ball>(BallSpell_BP, StartTransform, spawnParams);
					// Set actor rotation pitch/yaw to desired aimpoint location of player
					NewBall->SetActorRotation((DesiredStartPos - StartPos).Rotation());

					// Set line trace endpoint to direction player is pointing at + range of spell
					FVector EndPos{ StartPos + (DesiredStartRotation.Vector() * NewBall->SpellRange) };

					// Get LineTrace Result if collided
					if (TheWorld->LineTraceSingleByObjectType(
						OUT HitResult,
						StartPos,
						EndPos,
						ECollisionChannel::ECC_WorldStatic,
						LineTraceParams)) {
						// Finalise the spell setup
						NewBall->SpellSetup(ActiveElement, ActiveModifier, HitResult.ImpactPoint, false, 0.2);
					}
					else {
						NewBall->Destroy();
						UE_LOG(LogTemp, Warning, TEXT("SpellCastingController.LineTrace failed to find a valid end position for Spell_Ball"));
						return; // prevents ActiveRHSpell from being changed - i.e. player keeps spell if it failed to launch due to bad targetting
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("Failed to find Spell_Ball_BP"));
				}
				ActiveRHSpell = SpellID::None;
				break;
			case SpellID::Beam:
				/*bool hasBeam{ true };
				try {
					if (ActiveRHBeam == nullptr || !IsValid(ActiveRHBeam) || !ActiveRHBeam->IsValidLowLevel()) {
						hasBeam = false;
					}
				}
				catch (...) {
					hasBeam = false;
				}
				if (!hasBeam) {*/
					if (BeamSpell_BP) {
						StartTransform.SetScale3D(FVector{ 1,1,1 });

						// Spawn new actor
						ActiveRHBeam = TheWorld->SpawnActor<ASpell_Beam>(BeamSpell_BP, StartTransform, spawnParams);

						ActiveRHBeam->SpellSetup(ActiveElement, ActiveModifier, FVector{}, false, 0.2);
						ActiveRHBeam->ConnectMotionControllers(nullptr, RHand, nullptr); // Spell specific setup code
						ActiveRHSpell = SpellID::None;
					}
					else {
						UE_LOG(LogTemp, Error, TEXT("Failed to find Spell_Beam_BP"));
					}
				//}
				break;
			}

			// Reset elements and modifiers if required
			ResetSecondarySpells();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Failed to find world to spawn into... Where have all the flowers gone?"));
		}
	}
}

void USpellCastingController::LaunchLHSpell()
{
	UE_LOG(LogTemp, Warning, TEXT("Launching LH Spell: %s"), *UEnum::GetValueAsString(ActiveRHSpell));

	if (ActiveLHSpell != SpellID::None) { // i.e. there is a spell in LH
		// Commence LHSpell launch

		UWorld* TheWorld{ GetWorld() };
		if (TheWorld) { // If relevant objects exist

			// Spawn setup data
			FVector DesiredStartPos{ LHand->GetComponentLocation() };
			FRotator DesiredStartRotation{ (DesiredStartPos - hmdCamera->GetComponentLocation()).Rotation() }; // Relative from eyes to start pos
			FTransform StartTransform{ DesiredStartRotation, DesiredStartPos, FVector{0.01f, 0.01f, 0.01f} }; // Rotation, Location, Scale
			FActorSpawnParameters spawnParams{};

			// Figure out where player is pointing the spell and set targetPos to that location
			// Set up linetrace for spell target
			FHitResult HitResult;
			FVector StartPos{ hmdCamera->GetComponentLocation() };
			FCollisionQueryParams LineTraceParams{ FName{}, false, GetOwner() }; // No name required, not complex collision, ignore player character

			// Spawn type selector
			switch (ActiveLHSpell) {
			case SpellID::Wall:
				for (size_t i{}; i < 2; i++) {
					//bool hasWall{ true };
					//try {
					//	if (ActiveLHWalls[i] == nullptr || !IsValid(ActiveLHWalls[i]) || !ActiveLHWalls[i]->IsValidLowLevel()) {
					//		hasWall = false;
					//	}
					//}
					//catch (...) {
					//	hasWall = false;
					//}
					//if (!hasWall) { // If there is an available slot for wall
						// Spawn the wall with relevant setup parameters
						if (WallSpell_BP) {
							// Spawn new actor
							ActiveLHWalls[i] = TheWorld->SpawnActor<ASpell_Wall>(WallSpell_BP, StartTransform, spawnParams);
							// Set actor rotation pitch/yaw to desired aimpoint location of player
							ActiveLHWalls[i]->SetActorRotation((DesiredStartPos - StartPos).Rotation());

							// Set line trace endpoint to direction player is pointing at + range of spell
							FVector EndPos{ StartPos + (DesiredStartRotation.Vector() * ActiveLHWalls[i]->SpellRange) };

							// Get LineTrace Result if collided
							if (TheWorld->LineTraceSingleByObjectType(
								OUT HitResult,
								StartPos,
								EndPos,
								ECollisionChannel::ECC_WorldStatic,
								LineTraceParams)) {
								// Finalise the spell setup
								ActiveLHWalls[i]->SpellSetup(ActiveElement, ActiveModifier, HitResult.ImpactPoint, false, 0.2);
							}
							else {
								ActiveLHWalls[i]->Destroy();
								UE_LOG(LogTemp, Warning, TEXT("SpellCastingController.LineTrace failed to find a valid end position for Spell_Wall"));
								return; // prevents ActiveLHSpell from being changed - i.e. player keeps spell if it failed to launch due to bad targetting
							}
						}
						else {
							UE_LOG(LogTemp, Error, TEXT("Failed to find Spell_Wall_BP"));
						}
						ActiveLHSpell = SpellID::None;
						break; // Out of for loop
					//}
				}
				break;
			case SpellID::Ball:
				if (BallSpell_BP) {
					// Spawn new actor
					ASpell_Ball* NewBall = TheWorld->SpawnActor<ASpell_Ball>(BallSpell_BP, StartTransform, spawnParams);
					// Set actor rotation pitch/yaw to desired aimpoint location of player
					NewBall->SetActorRotation((DesiredStartPos - StartPos).Rotation());

					// Set line trace endpoint to direction player is pointing at + range of spell
					FVector EndPos{ StartPos + (DesiredStartRotation.Vector() * NewBall->SpellRange) };

					// Get LineTrace Result if collided
					if (TheWorld->LineTraceSingleByObjectType(
						OUT HitResult,
						StartPos,
						EndPos,
						ECollisionChannel::ECC_WorldStatic,
						LineTraceParams)) {
						// Finalise the spell setup
						NewBall->SpellSetup(ActiveElement, ActiveModifier, HitResult.ImpactPoint, false, 0.2);
					}
					else {
						NewBall->Destroy();
						UE_LOG(LogTemp, Warning, TEXT("SpellCastingController.LineTrace failed to find a valid end position for Spell_Ball"));
						return; // prevents ActiveRHSpell from being changed - i.e. player keeps spell if it failed to launch due to bad targetting
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("Failed to find Spell_Ball_BP"));
				}
				ActiveLHSpell = SpellID::None;
				break;
			case SpellID::Beam:
				/*bool hasBeam{ true };
				try {
					if (ActiveLHBeam == nullptr || !IsValid(ActiveLHBeam) || !ActiveLHBeam->IsValidLowLevel()) {
						hasBeam = false;
					}
				}
				catch (...) {
					hasBeam = false;
				}
				if (!hasBeam) {*/
					if (BeamSpell_BP) {
						StartTransform.SetScale3D(FVector{ 1,1,1 });

						// Spawn new actor
						ActiveLHBeam = TheWorld->SpawnActor<ASpell_Beam>(BeamSpell_BP, StartTransform, spawnParams);

						ActiveLHBeam->SpellSetup(ActiveElement, ActiveModifier, FVector{}, false, 0.2);
						ActiveLHBeam->ConnectMotionControllers(LHand, nullptr, nullptr); // Spell specific setup code
						ActiveLHSpell = SpellID::None;
					}
					else {
						UE_LOG(LogTemp, Error, TEXT("Failed to find Spell_Beam_BP"));
					}
				//}
				break;
			}

			// Reset elements and modifiers if required
			ResetSecondarySpells();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Failed to find world to spawn into... Where have all the flowers gone?"));
		}
	}
}

void USpellCastingController::LaunchDualHSpell()
{
	if (ActiveRHSpell != ActiveLHSpell) { // If dual casting was called, but each hand contains a different spell, launch each hand separately
		LaunchRHSpell();
		LaunchLHSpell();
	}
	else { // Otherwise launch as a dual hand spell

		UE_LOG(LogTemp, Warning, TEXT("Launching DualH Spell: %s"), *UEnum::GetValueAsString(ActiveRHSpell));
		
		if (ActiveLHSpell != SpellID::None) { // i.e. there is a spell in LH (RH==LH)
		// Commence Dual Hand Spell launch

			UWorld* TheWorld{ GetWorld() };
			if (TheWorld) { // If relevant objects exist

			// Spawn setup data
				FVector DesiredStartPos{ LHand->GetComponentLocation() + ((RHand->GetComponentLocation() - LHand->GetComponentLocation()) / 2) }; // Midpoint between both hands
				FRotator DesiredStartRotation{ (DesiredStartPos - hmdCamera->GetComponentLocation()).Rotation() }; // Relative from eyes to start pos
				FTransform StartTransform{ DesiredStartRotation, DesiredStartPos, FVector{0.01f, 0.01f, 0.01f} }; // Rotation, Location, Scale
				FActorSpawnParameters spawnParams{};

				// Figure out where player is pointing the spell and set targetPos to that location
				// Set up linetrace for spell target
				FHitResult HitResult;
				FVector StartPos{ hmdCamera->GetComponentLocation() };
				FCollisionQueryParams LineTraceParams{ FName{}, false, GetOwner() }; // No name required, not complex collision, ignore player character

				// Spawn type selector
				switch (ActiveRHSpell) {
				case SpellID::Wall:
					for (size_t i{}; i < 2; i++) {
						//bool hasRHWall{ true };
						//try {
						//	if (ActiveRHWalls[i] == nullptr || !IsValid(ActiveRHWalls[i]) || !ActiveRHWalls[i]->IsValidLowLevel()) {
						//		hasRHWall = false;
						//	}
						//}
						//catch (...) {
						//	hasRHWall = false;
						//}
						//if (!hasRHWall) { // If there is an available slot for wall
						//	// Spawn the wall with relevant setup parameters
							for (size_t j{}; j < 2; j++) {
						//		bool hasLHWall{ true };
						//		try {
						//			if (ActiveLHWalls[j] == nullptr || !IsValid(ActiveLHWalls[j]) || !ActiveLHWalls[j]->IsValidLowLevel()) {
						//				hasLHWall = false;
						//			}
						//		}
						//		catch (...) {
						//			hasLHWall = false;
						//		}
						//		if (!hasLHWall) { // If there is an available slot for wall
									// Spawn the wall with relevant setup parameters
									if (WallSpell_BP) {
										// Spawn new actor
										ActiveRHWalls[i] = TheWorld->SpawnActor<ASpell_Wall>(WallSpell_BP, StartTransform, spawnParams);
										ActiveLHWalls[j] = ActiveRHWalls[i];

										// Set actor rotation pitch/yaw to desired aimpoint location of player
										ActiveRHWalls[i]->SetActorRotation((DesiredStartPos - StartPos).Rotation());

										// Set line trace endpoint to direction player is pointing at + range of spell
										FVector EndPos{ StartPos + (DesiredStartRotation.Vector() * ActiveRHWalls[i]->SpellRange) };

										// Get LineTrace Result if collided
										if (TheWorld->LineTraceSingleByObjectType(
											OUT HitResult,
											StartPos,
											EndPos,
											ECollisionChannel::ECC_WorldStatic,
											LineTraceParams)) {
											// Finalise the spell setup
											ActiveRHWalls[i]->SpellSetup(ActiveElement, ActiveModifier, HitResult.ImpactPoint, true, 0.2);
										}
										else {
											ActiveRHWalls[i]->Destroy();
											UE_LOG(LogTemp, Warning, TEXT("SpellCastingController.LineTrace failed to find a valid end position for Spell_Wall"));
											return; // prevents ActiveRHSpell from being changed - i.e. player keeps spell if it failed to launch due to bad targetting
										}
									}
									else {
										UE_LOG(LogTemp, Error, TEXT("Failed to find Spell_Wall_BP"));
									}
									ActiveRHSpell = SpellID::None;
									ActiveLHSpell = SpellID::None;
									break; // Out of for loop
						//		}
							}
							break; // Out of for loop
						//}
					}
					break;
				case SpellID::Ball:
					if (BallSpell_BP) {
						// Spawn new actor
						ASpell_Ball* NewBall = TheWorld->SpawnActor<ASpell_Ball>(BallSpell_BP, StartTransform, spawnParams);
						// Set actor rotation pitch/yaw to desired aimpoint location of player
						NewBall->SetActorRotation((DesiredStartPos - StartPos).Rotation());

						// Set line trace endpoint to direction player is pointing at + range of spell
						FVector EndPos{ StartPos + (DesiredStartRotation.Vector() * NewBall->SpellRange) };

						// Get LineTrace Result if collided
						if (TheWorld->LineTraceSingleByObjectType(
							OUT HitResult,
							StartPos,
							EndPos,
							ECollisionChannel::ECC_WorldStatic,
							LineTraceParams)) {
							// Finalise the spell setup
							NewBall->SpellSetup(ActiveElement, ActiveModifier, HitResult.ImpactPoint, true, 0.2);
						}
						else {
							NewBall->Destroy();
							UE_LOG(LogTemp, Warning, TEXT("SpellCastingController.LineTrace failed to find a valid end position for Spell_Ball"));
							return; // prevents ActiveRHSpell from being changed - i.e. player keeps spell if it failed to launch due to bad targetting
						}
					}
					else {
						UE_LOG(LogTemp, Error, TEXT("Failed to find Spell_Ball_BP"));
					}
					ActiveRHSpell = SpellID::None;
					ActiveLHSpell = SpellID::None;
					break;
				case SpellID::Beam:
					/*bool hasBeam{ true };
					try {
						if ((ActiveLHBeam == nullptr || !IsValid(ActiveLHBeam) || !ActiveLHBeam->IsValidLowLevel()) &&
							(ActiveRHBeam == nullptr || !IsValid(ActiveRHBeam) || !ActiveRHBeam->IsValidLowLevel())) {
							hasBeam = false;
						}
					}
					catch (...) {
						hasBeam = false;
					}
					if (!hasBeam) {*/
						if (BeamSpell_BP) {
							StartTransform.SetScale3D(FVector{ 2,2,2 });

							// Spawn new actor
							ActiveLHBeam = TheWorld->SpawnActor<ASpell_Beam>(BeamSpell_BP, StartTransform, spawnParams);
							ActiveRHBeam = ActiveLHBeam;

							ActiveLHBeam->SpellSetup(ActiveElement, ActiveModifier, FVector{}, true, 0.2);
							ActiveLHBeam->ConnectMotionControllers(LHand, RHand, hmdCamera); // Spell specific setup code
							ActiveLHSpell = SpellID::None;
							ActiveRHSpell = SpellID::None;
						}
						else {
							UE_LOG(LogTemp, Error, TEXT("Failed to find Spell_Beam_BP"));
						}
					//}
					break;
				}

				ResetSecondarySpells();
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Failed to find world to spawn into... Where have all the flowers gone?"));
			}
		}
	}
}

// Updates the aimpoints for the relevant spells
void USpellCastingController::UpdateAimpoints()
{
}

// Transforms relevant hand animations as required
void USpellCastingController::UpdateHandAnimations()
{
}

// Checks current spell-chain limit, and resets all current spells if required
void USpellCastingController::ResetSecondarySpells()
{
	CurrentChainCount += 1;
	if ((ActiveRHSpell == SpellID::None && ActiveLHSpell == SpellID::None) || (CurrentChainCount >= MaxChainCount)) { // No spell is currently set, or maxchaincount reached
		CurrentChainCount = 0;
		ActiveElement = SpellID::None;
		ActiveModifier = SpellID::None;
	}
}

void USpellCastingController::ConnectMotionControllers(UMotionControllerComponent* LeftController, UMotionControllerComponent* RightController, UCameraComponent* HMD)
{
	LHand = LeftController;
	RHand = RightController;
	hmdCamera = HMD;
}