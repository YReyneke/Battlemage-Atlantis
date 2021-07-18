// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellContainer.h"
#include "CastingDemo.generated.h"

UCLASS()
class BATTLEMAGEATLANTIS01_API ACastingDemo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACastingDemo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void initDemo(FSpellData SpellToDemo);

	// Used to decide which spell to display
	// Multiple is not supported... And will default to
	// None? Well... there's nothing to be seen
	UPROPERTY(EditAnywhere, category = "Spell")
		TEnumAsByte<SpellID> SpellToDisplay;

private:

	UPROPERTY(EditAnywhere, category = "Setup")
		UStaticMeshComponent* DisplayPillar;
	UPROPERTY(EditAnywhere, category = "Setup")
		class UTextRenderComponent* DisplayText;
	UPROPERTY(EditAnywhere, category = "Setup")
		USceneComponent* DemoOrigin;
	UPROPERTY(EditAnywhere, category = "Setup")
		UStaticMeshComponent* LHand;
	UPROPERTY(EditAnywhere, category = "Setup")
		UStaticMeshComponent* RHand;

	UPROPERTY(EditAnywhere, category = "Default Variables")
		float DemoSize{ 20 };
	UPROPERTY(EditAnywhere, category = "Default Variables")
		float HandSeparation{ 0 };
	UPROPERTY(EditAnywhere, category = "Default Variables")
		float SecondsPerMove{ 3 };

private:

	// Variables
	FSpellData DemoSpell;
	int CurrentMove{ 1 };
	int TotalMoves{ 0 };
	float CurrentTime{ 0 };

	FVector LHandStartPos{};
	FVector RHandStartPos{};

	// Functions
	void UpdateDemoHands(float MoveCompletionFactor);
	void SetFactoredLocation(float MoveCompletionFactor);
	void SetFactoredRotation(float MoveCompletionFactor);
};
