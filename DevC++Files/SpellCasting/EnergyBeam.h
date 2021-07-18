// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnergyBeam.generated.h"

UCLASS()
class BATTLEMAGEATLANTIS01_API AEnergyBeam : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnergyBeam();

	void SetupDefaults(float& speed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditDefaultsOnly, category = "Setup")
		UStaticMeshComponent* BeamMesh{};

	float ParticleSpeed{};

};
