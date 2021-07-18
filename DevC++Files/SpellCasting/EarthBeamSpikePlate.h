// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EarthBeamSpikePlate.generated.h"

UCLASS()
class BATTLEMAGEATLANTIS01_API AEarthBeamSpikePlate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEarthBeamSpikePlate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupPlate(UMaterialInterface* BasePlateMaterial, UMaterialInterface* SpikeMaterial);

private:

	UPROPERTY(EditAnywhere, category = "Setup")
	UStaticMeshComponent* BasePlate;
	UPROPERTY(EditAnywhere, category = "Setup")
	UStaticMeshComponent* Spikes;

	// Distance used when retracting spikes
	UPROPERTY(EditAnywhere, category = "Spell Setup")
	float ExtensionDistance{ 50.f };

	// Distance used when retracting spikes
	UPROPERTY(EditAnywhere, category = "Spell Setup")
	float SpikeSpeed{ 3.f };

	void UpdateSpikeExtension();
};
