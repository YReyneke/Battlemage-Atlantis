// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "Spell_Ball.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEMAGEATLANTIS01_API ASpell_Ball : public ASpell
{
	GENERATED_BODY()
	

public:
	// Sets default values for this actor's properties
	ASpell_Ball();

private:

	UPROPERTY(EditDefaultsOnly, category = "StaticMesh")
		class UStaticMeshComponent* AnimationMesh2;
	UPROPERTY(EditDefaultsOnly, category = "StaticMesh")
		class UStaticMeshComponent* AnimationMesh3;

	// Sets the distance from launch by which the launch animation should be completed
	UPROPERTY(EditAnywhere, category = "SpellDefaults")
		float LaunchRange{ 25.f };
	UPROPERTY(EditAnywhere, category = "SpellDefaults")
		float LaunchRadius{ 10.f };
	UPROPERTY(EditAnywhere, category = "SpellDefaults")
		float SpellSpeed{ 10.f };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	virtual void UpdateCollision() override; // Spell Specific
	virtual void UpdatePosition() override; // Spell Specific
	virtual void EndSpell() override; // Spell Specific

	void UpdateLaunch();
	void UpdateLaunchAnimation(float CompletionFactor);
	void UpdateBallPosition(float CompletionFactor);

	virtual void SetMeshMaterial(UMaterialInterface* NewMaterial) override;
};
