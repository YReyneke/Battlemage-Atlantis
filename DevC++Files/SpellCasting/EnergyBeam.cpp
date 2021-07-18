// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "EnergyBeam.h"

// Sets default values
AEnergyBeam::AEnergyBeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeamSegmentMesh"));
	BeamMesh->SetupAttachment(GetRootComponent());
}

void AEnergyBeam::SetupDefaults(float& speed)
{
	ParticleSpeed = speed;
}

// Called when the game starts or when spawned
void AEnergyBeam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnergyBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalOffset(FVector{ ParticleSpeed, 0, 0 }, true);
}

