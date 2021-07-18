// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Spell.h"
#include "Spell_Wall.generated.h"

/*
* Wall spell - The Wall... Every spell wall, not much else to say
* Default behaviour:
*	Slows down enemy spells (not projectiles)
*	Damages them repeatedly reducing their effectiveness while they are within the wall
* Earthy Behaviour:
*	Blocks everything, destroying all energy spells and blocking all earth spells (earth ball should just bounce off)
*/

UCLASS()
class BATTLEMAGEATLANTIS01_API ASpell_Wall : public ASpell
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASpell_Wall();

	UPROPERTY(EditDefaultsOnly, category = "StaticMesh")
	class UStaticMeshComponent* LeftSideWall;
	UPROPERTY(EditDefaultsOnly, category = "StaticMesh")
	class UStaticMeshComponent* RightSideWall;

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

	void UpdateLaunchAnimation(float CompletionFactor);
	void UpdateWallPosition(float CompletionFactor);

	virtual void SetMeshMaterial(UMaterialInterface* NewMaterial) override;
};
