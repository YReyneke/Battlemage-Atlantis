// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "CastingDemo.h"
#include "Components/TextRenderComponent.h"

// Sets default values
ACastingDemo::ACastingDemo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DisplayPillar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Display Pillar"));
	DisplayPillar->SetupAttachment(GetRootComponent());

	DisplayText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Pillar Text"));
	DisplayText->SetupAttachment(DisplayPillar);

	DemoOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("Origin Reference For Demo"));
	DemoOrigin->SetupAttachment(DisplayPillar);

	LHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Hand Reference"));
	LHand->SetupAttachment(DemoOrigin);
	RHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Hand Reference"));
	RHand->SetupAttachment(DemoOrigin);
}

// Called when the game starts or when spawned
void ACastingDemo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACastingDemo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (&DemoSpell && SecondsPerMove != 0) { // Once spell demo has been set
		UpdateDemoHands(CurrentTime / SecondsPerMove);

		if (CurrentTime < SecondsPerMove) {
			CurrentTime += DeltaTime;
		}
	}
}

void ACastingDemo::initDemo(FSpellData SpellToDemo)
{
	DemoSpell = SpellToDemo;
	TotalMoves = DemoSpell.KeyPoints.Num();

	LHandStartPos = -0.5 * DemoSpell.LtoRRelativeStartPos * (DemoSize + HandSeparation); // +5 used to offset hands, helps with colliding into each other
	RHandStartPos = 0.5 * DemoSpell.LtoRRelativeStartPos * (DemoSize + HandSeparation);
	LHand->SetRelativeLocation(LHandStartPos);
	LHand->SetRelativeRotation(DemoSpell.KeyPoints[0].LHRotation);
	RHand->SetRelativeLocation(RHandStartPos);
	RHand->SetRelativeRotation(DemoSpell.KeyPoints[0].RHRotation);
}

void ACastingDemo::UpdateDemoHands(float MoveCompletionFactor)
{
	if (CurrentMove > TotalMoves) CurrentMove = 1;
	if (MoveCompletionFactor > 1) MoveCompletionFactor = 1;

	if (!(CurrentMove == TotalMoves)) {
		SetFactoredLocation(MoveCompletionFactor);
		SetFactoredRotation(MoveCompletionFactor);
	}

	if (MoveCompletionFactor == 1) {
		CurrentTime = 0;
		CurrentMove++;
	}
}

void ACastingDemo::SetFactoredLocation(float MoveCompletionFactor)
{
	LHand->SetRelativeLocation(
		LHandStartPos + 
		(DemoSpell.KeyPoints[CurrentMove - 1].LHPosition * DemoSize) +
		(DemoSpell.KeyPoints[CurrentMove].LHPosition - DemoSpell.KeyPoints[CurrentMove - 1].LHPosition) * MoveCompletionFactor * DemoSize
	);
	RHand->SetRelativeLocation(
		RHandStartPos +
		(DemoSpell.KeyPoints[CurrentMove - 1].RHPosition * DemoSize) +
		(DemoSpell.KeyPoints[CurrentMove].RHPosition - DemoSpell.KeyPoints[CurrentMove - 1].RHPosition) * MoveCompletionFactor * DemoSize
	);
}

void ACastingDemo::SetFactoredRotation(float MoveCompletionFactor)
{
	LHand->SetRelativeRotation(
		DemoSpell.KeyPoints[CurrentMove - 1].LHRotation +
		((DemoSpell.KeyPoints[CurrentMove].LHRotation - DemoSpell.KeyPoints[CurrentMove - 1].LHRotation) * MoveCompletionFactor)
	);
	RHand->SetRelativeRotation(
		DemoSpell.KeyPoints[CurrentMove - 1].RHRotation +
		((DemoSpell.KeyPoints[CurrentMove].RHRotation - DemoSpell.KeyPoints[CurrentMove - 1].RHRotation) * MoveCompletionFactor)
	);
}
