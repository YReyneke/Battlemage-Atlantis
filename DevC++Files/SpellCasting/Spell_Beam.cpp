// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "Spell_Beam.h"
#include "EarthBeamSpikePlate.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "EnergyBeam.h"

ASpell_Beam::ASpell_Beam()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	PrimaryActorTick.bCanEverTick = true;
}

void ASpell_Beam::BeginPlay()
{
	Super::BeginPlay();
	SpikeTimer = SpikeDelay;
}

void ASpell_Beam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveElement == SpellID::Earth) {
		// Spawn spikeplates one after the other with delay
		SpikeTimer += DeltaTime;
		if (SpikeTimer >= SpikeDelay && NextSpike < SpikePlateCount) {
			SpawnNextSpikePlate(NextSpike);
			NextSpike++;
			SpikeTimer = 0;
		}
	}
	else {
		UpdatePosition(); // Of actor

		// Spawn next mesh as required
		if (BeamSegments.Num() == 0) {
			BeamSegments.Add(GetWorld()->SpawnActor<AEnergyBeam>(EnergyBeam_BP, GetActorTransform())); // Spawn first moving instance
			BeamSegments[BeamSegments.Num() - 1]->SetupDefaults(LaunchSpeed);
		}
		else {
			// work out how far the latest beam segment has gone
			if ((BeamSegments[BeamSegments.Num() - 1]->GetActorLocation() - GetActorLocation()).Size() >= EnergyBeamLength - EnergyBeamSeparation) { // If movement is greater than length of mesh - separation
				BeamSegments.Add(GetWorld()->SpawnActor<AEnergyBeam>(EnergyBeam_BP, GetActorTransform())); // Spawn next mesh
				BeamSegments[BeamSegments.Num() - 1]->SetupDefaults(LaunchSpeed);
			}
		}

		// Remove and destroy any segments that have gone out of spell range
		for (size_t i{ 0 }; i < BeamSegments.Num(); i++) {
			if ((BeamSegments[i]->GetActorLocation() - GetActorLocation()).Size() > SpellRange) {
				BeamSegments[i]->Destroy();
				BeamSegments.RemoveAt(i);
			}
		}
	}

	// Check beam collision
}

void ASpell_Beam::ConnectMotionControllers(UMotionControllerComponent* LeftHand, UMotionControllerComponent* RightHand, UCameraComponent* HeadCam)
{
	hmdCamera = HeadCam;
	LHand = LeftHand;
	RHand = RightHand;

	if (ActiveElement == SpellID::Earth) {
		DefaultDuration = EarthSpellDuration;
		// Disable default components and prep actor for spawning more plates
		SpellMesh->SetHiddenInGame(true);
		SpellAnimationMesh->SetHiddenInGame(true);
		SetActorRotation(FRotator{ 0,GetActorRotation().Yaw, 0 });
		// Setup spikes and add them to the spell
		SetupSpikePlates();
	} else {
		//SpellMesh->SetRelativeScale3D(FVector{ SpellRange / EnergyBeamLength,1,1 }); //
	}

	isLaunchComplete = true;
}

void ASpell_Beam::UpdateCollision()
{
}

void ASpell_Beam::UpdatePosition()
{
	if (LHand == nullptr && RHand == nullptr) return;

	FVector BeamPos{};
	FRotator BeamRot{};
	if (LHand != nullptr && RHand != nullptr) { // Dual casting
		BeamPos = LHand->GetComponentLocation() + ((RHand->GetComponentLocation() - LHand->GetComponentLocation()) / 2); // Half way between both hands
		BeamRot = (BeamPos - hmdCamera->GetComponentLocation()).Rotation();
		BeamRot.Pitch = 0;
	}
	else if (LHand != nullptr) { // Left hand casting
		BeamPos = LHand->GetComponentLocation();
		BeamRot = LHand->GetComponentRotation();
	}
	else if (RHand != nullptr) { // Right hand casting
		BeamPos = RHand->GetComponentLocation();
		BeamRot = RHand->GetComponentRotation();
	}
	BeamRot.Roll = 0;

	// Update Actor
	SetActorLocation(BeamPos);
	SetActorRotation(BeamRot);
}

void ASpell_Beam::EndSpell()
{

	// Destroy actor
	LHand = nullptr;
	RHand = nullptr;
	hmdCamera = nullptr;

	if (ActiveElement == SpellID::Earth) {
		for (auto baseplate : EarthSpikes) {
			if (baseplate != nullptr) {
				baseplate->Destroy();
			}
		}
	}
	else {
		// Remove and destroy any remaining segments
		if (BeamSegments.Num() > 0) {
			for (size_t i{ 0 }; i < BeamSegments.Num(); i++) {
				BeamSegments[i]->Destroy();
				BeamSegments.RemoveAt(i);
			}
		}
	}

	Super::EndSpell();
}
// Called by SpellSetup.....
void ASpell_Beam::SetMeshMaterial(UMaterialInterface* NewMaterial)
{
	if (ActiveElement != SpellID::Earth) {
		Super::SetMeshMaterial(NewMaterial);
	}
}

void ASpell_Beam::SpawnNextSpikePlate(int PlateID)
{
	// Spawn new base plate and add it to list of baseplates
	SpawnTF.SetLocation(SpawnLocations[PlateID]);

	AEarthBeamSpikePlate* NewSpike = GetWorld()->SpawnActor<AEarthBeamSpikePlate>(EarthSpike_BP, SpawnTF);
	NewSpike->SetupPlate(PlateMaterial, SpikeMaterial);
	EarthSpikes.Add(NewSpike);
}

void ASpell_Beam::SetupSpikePlates()
{
	// Figure out ground height to spawn the base plate (we already have orientation)
	FHitResult HitResult;
	FVector TraceStart{ GetActorLocation() - FVector{0,0,20} }; // -FVector added to start trace outside of hand controller
	FVector TraceEnd{ GetActorLocation() - FVector{0,0,SpellRange} }; // Spell range used for now... might be a little OTT
	FCollisionQueryParams LineTraceParams{ FName{}, false, GetOwner() }; // No name required, not complex collision, ignore owning actor (TBDebugged)

	// Spawn spikeplates as required
	SpawnTF = FTransform{ GetActorRotation(), GetActorLocation(), GetActorScale() }; // Rotation, Location, Scale
	SetSpikeMaterials();
	
	if (SpikeMaterial == nullptr || PlateMaterial == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Spell_Beam failed to load at least one earth material!"));
		return; // Cancel spawning of base plates
	}
	
	if (GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		TraceStart, TraceEnd,
		ECollisionChannel::ECC_WorldStatic,
		LineTraceParams
	)) {
		SetActorLocation(HitResult.ImpactPoint);
		for (size_t i{ 0 }; i < SpikePlateCount; i++) {
			AddActorLocalOffset(FVector{ 100.f * GetActorScale().Z,0,0 }); // 100cm = width of spikeplate
			SpawnLocations.Add(GetActorLocation());
		}
	}
}

void ASpell_Beam::SetSpikeMaterials()
{
	switch (ActiveModifier) {
	case SpellID::IncPwr:
		SpikeMaterial = EarthHPow_SpellMaterial;
		PlateMaterial = EarthBasePlateHPow_SpellMaterial;
		break;
	case SpellID::DecPwr:
		SpikeMaterial = EarthLPow_SpellMaterial;
		PlateMaterial = EarthBasePlateLPow_SpellMaterial;
		break;
	case SpellID::IncDur:
		SpikeMaterial = EarthHDur_SpellMaterial;
		PlateMaterial = EarthBasePlateHDur_SpellMaterial;
		break;
	case SpellID::DecDur:
		SpikeMaterial = EarthLDur_SpellMaterial;
		PlateMaterial = EarthBasePlateLDur_SpellMaterial;
		break;
	case SpellID::Explode:
		SpikeMaterial = EarthExplode_SpellMaterial;
		PlateMaterial = EarthBasePlateExplode_SpellMaterial;
		break;
	case SpellID::Magnet:
		SpikeMaterial = EarthMagnet_SpellMaterial;
		PlateMaterial = EarthBasePlateMagnet_SpellMaterial;
		break;
	default:
		SpikeMaterial = Earth_SpellMaterial;
		PlateMaterial = EarthBasePlate_SpellMaterial;
		break;
	}
}
