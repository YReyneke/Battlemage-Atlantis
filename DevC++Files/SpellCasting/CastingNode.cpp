// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "CastingNode.h"

// Sets default values
ACastingNode::ACastingNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CastingNodeMesh"));
	SetRootComponent(StaticMesh);
}

// Called when the game starts or when spawned
void ACastingNode::BeginPlay()
{
	Super::BeginPlay();
}

