// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellContainer.h"
#include "Spell.generated.h"


UCLASS(Blueprintable)
class BATTLEMAGEATLANTIS01_API ASpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpell();
	void SpellSetup(SpellID Element, SpellID Modifier, FVector InitialTargetPosition, bool isDualCast, float HandAnimationScaleFactor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetMeshMaterial(UMaterialInterface* NewMaterial);

	// Public functions
	void RemoveEnergy(float Energy);

public:
	// UProperties
	
	// Root Component
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootSceneComponent;

	// The total energy value of the spell
	UPROPERTY(EditAnywhere, category = "SpellDefaults")
	float SpellEnergy{ 2000.0f };

	// A limit to how long the spell can be alive for - Modifier spells affect this
	// Set to 9999 or greater to disable
	UPROPERTY(EditAnywhere, category = "SpellDefaults")
	float DefaultDuration{ 30.0f };

	// How much of total target energy is used up when damaging targets
	// If this spell does not have enough energy to reduce target energy, the remaining energy of this spell is used
	UPROPERTY(EditAnywhere, category = "SpellDefaults")
	float DefaultDamageFactor{ 1.f };

	// How far from player the spell can be launched to (targetting line-trace)
	UPROPERTY(EditAnywhere, category = "SpellDefaults")
	float SpellRange{ 3000.f };

	// Initial speed of the spell at moment of launch
	UPROPERTY(EditAnywhere, category = "SpellDefaults")
	float LaunchSpeed{ 5.f };

protected:
	// Variables

	SpellID ActiveElement{ SpellID::None };
	SpellID ActiveModifier{ SpellID::None };
	FVector TargetPosition{};
	bool isDualHandSpell{ false };
	float AnimationScaleFactor{ 0.1f }; // Holds the minimum scale of the hand animation object

	float CurrentEnergy{ 0.0f }; // Start energy -> Updated during spell lifespan, when this reaches 0, spell extinguishes
	FTransform StartTransform{};
	FVector MoveDirection{};

	bool isLaunchComplete{ false };

	// Spell specific Functions
	virtual void UpdateCollision(); // Only when launch animation is complete
	virtual void UpdatePosition();
	virtual void EndSpell();

	// Generic spell functions
	float GetDistanceFactor(FVector StartPos, FVector EndPos, FVector CurrentPos);
	float GetDistanceFactor(FVector StartPos, FVector CurrentPos, float Range);


protected:

	// The main mesh of the spell
	UPROPERTY(EditDefaultsOnly, category = "StaticMesh")
		UStaticMeshComponent* SpellMesh;

	// The mesh used for the launch animation
	UPROPERTY(EditDefaultsOnly, category = "StaticMesh")
		UStaticMeshComponent* SpellAnimationMesh;

	// Material List

	// The various materials that can be applied to each mesh
	// NOTE: Air is not visualised as a material
	// NOTE: Fire is a particle system that is activated/deactivated in ASpell::ASpell()
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterial* Energy_SpellMaterial;

	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EnergyHPow_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EnergyLPow_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EnergyHDur_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EnergyLDur_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EnergyExplode_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EnergyMagnet_SpellMaterial;
	
	UPROPERTY(EditDefaultsOnly, category="EffectMaterials")
	class UMaterial* Water_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category="EffectMaterials")
	class UMaterialInstance* WaterHPow_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category="EffectMaterials")
	class UMaterialInstance* WaterLPow_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category="EffectMaterials")
	class UMaterialInstance* WaterHDur_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category="EffectMaterials")
	class UMaterialInstance* WaterLDur_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category="EffectMaterials")
	class UMaterialInstance* WaterExplode_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category="EffectMaterials")
	class UMaterialInstance* WaterMagnet_SpellMaterial;
	
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterial* Earth_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthHPow_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthLPow_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthHDur_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthLDur_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthExplode_SpellMaterial;
	UPROPERTY(EditDefaultsOnly, category = "EffectMaterials")
		class UMaterialInstance* EarthMagnet_SpellMaterial;

	UMaterialInterface* GetMaterial();

};
