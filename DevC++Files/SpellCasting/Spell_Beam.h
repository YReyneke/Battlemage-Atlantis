// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "Spell_Beam.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEMAGEATLANTIS01_API ASpell_Beam : public ASpell
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpell_Beam();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Must be called immediately after spell setup completes
	// ONLY pass in hand(s) that is/are casting! use nullptr if not required
	void ConnectMotionControllers(class UMotionControllerComponent* LeftHand, class UMotionControllerComponent* RightHand, class UCameraComponent* HeadCam);

private:

	// EarthSpikeBasePlate materials go in here
	// The Spike materials go into the default Earth material slots
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterial* EarthBasePlate_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthBasePlateHPow_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthBasePlateLPow_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthBasePlateHDur_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthBasePlateLDur_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthBasePlateExplode_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthBasePlateMagnet_SpellMaterial;

	UPROPERTY(EditAnywhere, category = "Setup")
	TSubclassOf<class AEarthBeamSpikePlate> EarthSpike_BP;
	TArray<class AEarthBeamSpikePlate*> EarthSpikes{};

	// TSubclassOf AEnergyBeam
	UPROPERTY(EditAnywhere, category = "Setup")
	TSubclassOf<class AEnergyBeam> EnergyBeam_BP;
	TArray<class AEnergyBeam*> BeamSegments{};

	// Number of spikeplates to deploy in line if earth beam
	UPROPERTY(EditAnywhere, category = "Setup")
	int SpikePlateCount{ 6 };

	// Delay between spikeplates (seconds)
	UPROPERTY(EditAnywhere, category = "Setup")
	float SpikeDelay{ 0.6666 };

	// Spell duration when earth modifier is active
	UPROPERTY(EditAnywhere, category = "Setup")
	float EarthSpellDuration{ 30 };

	UPROPERTY(EditAnywhere, category = "Setup")
	float EnergyBeamLength{ 6 };

	UPROPERTY(EditAnywhere, category = "Setup")
	float EnergyBeamSeparation{ 3 };

	// Variables
	class UCameraComponent* hmdCamera{};
	class UMotionControllerComponent* LHand{};
	class UMotionControllerComponent* RHand{};

	// Spell specific Functions
	virtual void UpdateCollision() override; // Only when launch animation is complete
	virtual void UpdatePosition() override;
	virtual void EndSpell() override;
	virtual void SetMeshMaterial(UMaterialInterface* NewMaterial) override;

	float SpikeTimer{ 0 };
	int NextSpike{ 0 };
	void SpawnNextSpikePlate(int PlateID);
	void SetupSpikePlates();

	void SetSpikeMaterials();
	UMaterialInterface* SpikeMaterial;
	UMaterialInterface* PlateMaterial;

	FTransform SpawnTF{};
	TArray<FVector> SpawnLocations{};
};
