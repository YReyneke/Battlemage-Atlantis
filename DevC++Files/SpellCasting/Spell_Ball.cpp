// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "Spell_Ball.h"


ASpell_Ball::ASpell_Ball()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!AnimationMesh2) {
		AnimationMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Animation mesh2 (identicaltoMainAnimationMesh)"));
		AnimationMesh2->SetupAttachment(RootSceneComponent);
	}
	if (!AnimationMesh3) {
		AnimationMesh3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Animation mesh3 (identicaltoMainAnimationMesh)"));
		AnimationMesh3->SetupAttachment(RootSceneComponent);
	}
}

// Called when the game starts or when spawned
void ASpell_Ball::BeginPlay()
{
	Super::BeginPlay();

	// Make main mesh invisible to user (re-activates within launch animation)
	SpellAnimationMesh->SetHiddenInGame(false);
	SpellMesh->SetHiddenInGame(true);
	AnimationMesh2->SetHiddenInGame(false);
	AnimationMesh3->SetHiddenInGame(false);

	UE_LOG(LogTemp, Warning, TEXT("Ball Has Been Spawned..."));
}

// Called every frame
void ASpell_Ball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePosition();
	if (isLaunchComplete) {
		UpdateCollision();
	}
	else {
		UpdateLaunch();
	}
}

// Updates what happens upon collision of the static mesh (provided launch animation is complete)
void ASpell_Ball::UpdateCollision()
{
	// Get all 'colliding' objects
	// If stoney, for all colliding objects
		// Damage 100% and reduce SpellEnergy by total health of other 'object' (friendly or enemy - non alive only)
	// If normal, for all colliding objects
		// If enemy
			// Accellerate towards Ball
			// NOTE: Stoney spells are not affected by energy Ball friction, though still take energy damage
			// NOTE: Energy beam spells are completely blocked by the Ball (therefore beam damage is applied to SpellEnergy directly)
			// Apply (target max energy * damage factor) damage when target specific timer reaches 0
			// Reduce SpellEnergy by amount of damage applied
		// If friendly
			// Nothing happens to any friendly spell (unless fire is applied)
}

// While Position is not yet at final position, update the launch animation
void ASpell_Ball::UpdatePosition()
{
	// Start updating the launch animation mesh (start at 0Deg and miniature, then scale up and rotate to 180Deg )
	// Once half way to destination switch (spectacularly) to main spell mesh (half scale, 180Deg from final orientation)
	// Finish updating launch mesh - rotate to final orientation by final position

	// Ensure forward direction is correct

	// Move actor forward by speed
	if (isLaunchComplete) {
		AddActorLocalOffset(FVector{ SpellSpeed, 0, 0 }, true);
	}
	else {
		AddActorLocalOffset(FVector{ LaunchSpeed, 0, 0 }, true);
	}
}

void ASpell_Ball::EndSpell()
{
	// If explosive and has energy left, spawn an explosion at ball mesh origins (grenade)

	Super::EndSpell(); // Destroys Actor
}

void ASpell_Ball::UpdateLaunch() {
	float AnimCompleteFactor{ GetDistanceFactor(StartTransform.GetLocation(), GetActorLocation(), LaunchRange) };// 0->1 value representing how far along animation should be
	if (!(AnimCompleteFactor <= 0)) { // i.e. factor is not garbage
		if (AnimCompleteFactor < 1) { // First movement half
			UpdateLaunchAnimation(AnimCompleteFactor);
		}
		else { // Second movement half
			// (De)Activate relevant meshes
			if (!SpellAnimationMesh->bHiddenInGame) {
				SpellAnimationMesh->SetHiddenInGame(true);
				SpellMesh->SetHiddenInGame(false);
				AnimationMesh2->SetHiddenInGame(true);
				AnimationMesh3->SetHiddenInGame(true);
				isLaunchComplete = true;
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Garbage passed into Spell_Ball.UpdateLaunch().AnimCompleteFactor"));
	}
}

// Scales/Rotates the animation phase
void ASpell_Ball::UpdateLaunchAnimation(float CompletionFactor)
{
	// Completion factor represents radius and scale - NOTE: This means that actual size of HandAnimationMesh must be equal to actual size of Ball
	float desiredScale{ (CompletionFactor < AnimationScaleFactor) ? AnimationScaleFactor : CompletionFactor };
	float desiredRadius{ LaunchRadius * (1 - desiredScale) };
	float desiredRoll{ 360 * CompletionFactor}; // 3 because I want the animation to rotate three times before launch completes

	if (isDualHandSpell) {
		//desiredRadius *= 2;
		desiredScale *= 2;
	}

	// Update rotation of the actor
	FRotator SetRot{ GetActorRotation() };
	SetRot.Roll = desiredRoll;
	SetActorRotation(SetRot);

	// Update Scale of launch balls
	SetActorScale3D(FVector{ desiredScale, desiredScale, desiredScale });

	// Update Radial position of launch balls
	SpellAnimationMesh->SetRelativeLocation(FVector{ 0, 0, desiredRadius });
	AnimationMesh2->SetRelativeLocation(FVector{ 0, (FMath::Sin(1.047197551) * desiredRadius), -(FMath::Sin(0.5235987756) * desiredRadius) });
	AnimationMesh3->SetRelativeLocation(FVector{ 0, -(FMath::Sin(1.047197551) * desiredRadius), -(FMath::Sin(0.5235987756) * desiredRadius) });
	//UE_LOG(LogTemp, Warning, TEXT("Sin(60): %f, Sin(30): %f"), FMath::Sin(1.047197551), FMath::Sin(0.5235987756));
}

void ASpell_Ball::UpdateBallPosition(float CompletionFactor)
{
	
}

void ASpell_Ball::SetMeshMaterial(UMaterialInterface* NewMaterial)
{
	if (NewMaterial != nullptr) {
		if (SpellAnimationMesh) {
			SpellAnimationMesh->SetMaterial(0, NewMaterial);
			AnimationMesh2->SetMaterial(0, NewMaterial);
			AnimationMesh3->SetMaterial(0, NewMaterial);
		}
		if (SpellMesh) {
			SpellMesh->SetMaterial(0, NewMaterial);
		}
	}
}