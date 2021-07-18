// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "Spell_Wall.h"

ASpell_Wall::ASpell_Wall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!LeftSideWall) {
		LeftSideWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallStaticMesh1 (identicaltoMainMesh)"));
		LeftSideWall->SetupAttachment(RootSceneComponent);
	}
	if (!RightSideWall) {
		RightSideWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall2StaticMesh2 (identicaltoMainMesh)"));
		RightSideWall->SetupAttachment(RootSceneComponent);
	}
}

// Called when the game starts or when spawned
void ASpell_Wall::BeginPlay()
{
	Super::BeginPlay();

	// Make main mesh invisible to user (re-activates within launch animation)
	SpellAnimationMesh->SetHiddenInGame(false);
	SpellMesh->SetHiddenInGame(true);
	LeftSideWall->SetHiddenInGame(true);
	RightSideWall->SetHiddenInGame(true);

	UE_LOG(LogTemp, Warning, TEXT("Wall Has Been Spawned..."));
}

// Called every frame
void ASpell_Wall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isLaunchComplete) {
		UpdatePosition();
	}
	else {
		UpdateCollision();
	}
}

// Updates what happens upon collision of the static mesh (provided launch animation is complete)
void ASpell_Wall::UpdateCollision()
{
	// Get all 'colliding' objects
	// If stoney, for all colliding objects
		// Damage 100% and reduce SpellEnergy by total health of other 'object' (friendly or enemy - non alive only)
	// If normal, for all colliding objects
		// If enemy
			// Accellerate towards wall
			// NOTE: Stoney spells are not affected by energy wall friction, though still take energy damage
			// NOTE: Energy beam spells are completely blocked by the wall (therefore beam damage is applied to SpellEnergy directly)
			// Apply (target max energy * damage factor) damage when target specific timer reaches 0
			// Reduce SpellEnergy by amount of damage applied
		// If friendly
			// Nothing happens to any friendly spell (unless fire is applied)
}

// While Position is not yet at final position, update the launch animation
void ASpell_Wall::UpdatePosition()
{
	// Start updating the launch animation mesh (start at 0Deg and miniature, then scale up and rotate to 180Deg )
	// Once half way to destination switch (spectacularly) to main spell mesh (half scale, 180Deg from final orientation)
	// Finish updating launch mesh - rotate to final orientation by final position
	
	// Ensure forward direction is correct

	// Move actor forward by speed
	AddActorLocalOffset(FVector{ LaunchSpeed, 0, 0 }, true);

	float AnimCompleteFactor{ GetDistanceFactor(StartTransform.GetLocation(), TargetPosition, GetActorLocation()) };// 0->1 value representing how far along animation should be
	if (!(AnimCompleteFactor <= 0)) { // i.e. factor is not garbage
		if (AnimCompleteFactor < 0.5) { // First movement half
			UpdateLaunchAnimation(AnimCompleteFactor);
		}
		else { // Second movement half
			// (De)Activate relevant meshes
			if (!SpellAnimationMesh->bHiddenInGame) {
				SpellAnimationMesh->SetHiddenInGame(true);
				SpellMesh->SetHiddenInGame(false);

				if (isDualHandSpell) { // Activate the extra meshes
					LeftSideWall->SetHiddenInGame(false);
					RightSideWall->SetHiddenInGame(false);
				}
			}
			else { // Actually do the update
				UpdateWallPosition(AnimCompleteFactor);
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Garbage passed into Spell_Wall.UpdatePosition().AnimCompleteFactor"));
	}
	if (AnimCompleteFactor >= 1) {
		isLaunchComplete = true; // Stop updating launch animation (in this case, stop moving the wall around)

		// Set current position/rotation to correct end position (minor detail, but in the name of perfection)
		SetActorLocation(TargetPosition);
		SetActorRotation(FRotator{ 0, GetActorRotation().Yaw, 0 });
	}
}

void ASpell_Wall::EndSpell()
{
	// If explosive and has energy left, spawn an explosion at all mesh origins

	Super::EndSpell(); // Destroys Actor
}

// Scales/Rotates the animation phase
void ASpell_Wall::UpdateLaunchAnimation(float CompletionFactor)
{	
	// Completion factor represents scale - NOTE: This means that actual size of HandAnimationMesh must be equal to actual size of Wall
	float DesiredScale{ (CompletionFactor < AnimationScaleFactor) ? AnimationScaleFactor : CompletionFactor };
	float DesiredRoll{ 5 * 360 * CompletionFactor }; // 5 because I want the launch animation to spin around 4.5 times

	FRotator NewRotation{ GetActorRotation() };
	NewRotation.Roll = DesiredRoll;
	SetActorRotation(NewRotation);

	if (isDualHandSpell) {
		SetActorScale3D(FVector{ DesiredScale * 2.f, DesiredScale, DesiredScale * 1.2f });
	}
	else {
		SetActorScale3D(FVector{ DesiredScale, DesiredScale, DesiredScale });
	}
}

// Scales/Rotates the spell
void ASpell_Wall::UpdateWallPosition(float CompletionFactor)
{
	float DesiredRoll{ 360 * CompletionFactor }; // The final rotation is only 1/2 a turn

	FRotator NewRotation{ GetActorRotation() };
	NewRotation.Roll = DesiredRoll;
	SetActorRotation(NewRotation);

	if (isDualHandSpell) {
		SetActorScale3D(FVector{ CompletionFactor * 2.f, CompletionFactor, CompletionFactor * 1.2f });
	}
	else {
		SetActorScale3D(FVector{ CompletionFactor, CompletionFactor, CompletionFactor });
	}
}

void ASpell_Wall::SetMeshMaterial(UMaterialInterface* NewMaterial)
{
	if (NewMaterial != nullptr) {
		if (SpellAnimationMesh) {
			SpellAnimationMesh->SetMaterial(0, NewMaterial);
		}
		if (SpellMesh) {
			SpellMesh->SetMaterial(0, NewMaterial);
			LeftSideWall->SetMaterial(0, NewMaterial);
			RightSideWall->SetMaterial(0, NewMaterial);
		}
	}
}