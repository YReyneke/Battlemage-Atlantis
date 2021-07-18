// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "EarthBeamSpikePlate.h"

// Sets default values
AEarthBeamSpikePlate::AEarthBeamSpikePlate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BasePlate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spikeplate Base"));
	BasePlate->SetupAttachment(GetRootComponent());
	Spikes = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spikey Things"));
	Spikes->SetupAttachment(BasePlate);
}

// Called when the game starts or when spawned
void AEarthBeamSpikePlate::BeginPlay()
{
	Super::BeginPlay();
	
	Spikes->SetRelativeLocation(FVector{ 0,0,-ExtensionDistance });
}

// Called every frame
void AEarthBeamSpikePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpikeExtension();
}

void AEarthBeamSpikePlate::SetupPlate(UMaterialInterface* BasePlateMaterial, UMaterialInterface* SpikeMaterial)
{
	BasePlate->SetMaterial(0, BasePlateMaterial);
	Spikes->SetMaterial(0, SpikeMaterial);
}

// *** TBI - Retracts when friendly player is close
// Extends Spikes
void AEarthBeamSpikePlate::UpdateSpikeExtension()
{
	FVector RelativeSpikePos = Spikes->GetRelativeLocation();
	if (RelativeSpikePos.Z < 0) {
		RelativeSpikePos.Z += SpikeSpeed;
		if (RelativeSpikePos.Z > 0) RelativeSpikePos.Z = 0;
		Spikes->SetRelativeLocation(RelativeSpikePos);
	}
}

