// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "Spell.h"

// Sets default values
ASpell::ASpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootSceneComponent->SetupAttachment(GetRootComponent());

	SpellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpellMesh"));
	SpellMesh->SetupAttachment(RootSceneComponent);

	SpellAnimationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpellAnimationMesh"));
	SpellAnimationMesh->SetupAttachment(RootSceneComponent);
}

void ASpell::SpellSetup(SpellID Element, SpellID Modifier, FVector InitialTargetPosition, bool isDualCast, float HandAnimationScaleFactor)
{
	ActiveElement = Element;
	ActiveModifier = Modifier;
	TargetPosition = InitialTargetPosition;
	isDualHandSpell = isDualCast;
	AnimationScaleFactor = HandAnimationScaleFactor;

	CurrentEnergy = SpellEnergy;
	StartTransform = GetActorTransform();
	MoveDirection = StartTransform.GetRotation().GetForwardVector();

	// Update mesh materials if required
	SetMeshMaterial(GetMaterial());
}

// Called when the game starts or when spawned
void ASpell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!(DefaultDuration >= 9999.f) && isLaunchComplete){ // Only count down once launch is complete
		if (DefaultDuration > 0) {
			DefaultDuration -= DeltaTime;
		}
		else {
			EndSpell();
		}
	}
}

void ASpell::SetMeshMaterial(UMaterialInterface* NewMaterial)
{
	if (NewMaterial != nullptr) {
		if (SpellAnimationMesh) {
			SpellAnimationMesh->SetMaterial(0, NewMaterial);
		}
		if (SpellMesh) {
			SpellMesh->SetMaterial(0, NewMaterial);
		}
	}
}


UMaterialInterface* ASpell::GetMaterial()
{
	switch (ActiveElement) {
	case SpellID::None: // Basic energy materials
		switch (ActiveModifier) {
		case SpellID::IncPwr:
			return EnergyHPow_SpellMaterial;
			break;
		case SpellID::DecPwr:
			return EnergyLPow_SpellMaterial;
			break;
		case SpellID::IncDur:
			return EnergyHDur_SpellMaterial;
			break;
		case SpellID::DecDur:
			return EnergyLDur_SpellMaterial;
			break;
		case SpellID::Explode:
			return EnergyExplode_SpellMaterial;
			break;
		case SpellID::Magnet:
			return EnergyMagnet_SpellMaterial;
			break;
		}
		break;
	case SpellID::Water: // 'Invisible' materials
		switch (ActiveModifier) {
		case SpellID::IncPwr:
			return WaterHPow_SpellMaterial;
			break;
		case SpellID::DecPwr:
			return WaterLPow_SpellMaterial;
			break;
		case SpellID::IncDur:
			return WaterHDur_SpellMaterial;
			break;
		case SpellID::DecDur:
			return WaterLDur_SpellMaterial;
			break;
		case SpellID::Explode:
			return WaterExplode_SpellMaterial;
			break;
		case SpellID::Magnet:
			return WaterMagnet_SpellMaterial;
			break;
		default:
			return Water_SpellMaterial;
			break;
		}
		break;
	case SpellID::Earth: // 'Solid' materials
		switch (ActiveModifier) {
		case SpellID::IncPwr:
			return EarthHPow_SpellMaterial;
			break;
		case SpellID::DecPwr:
			return EarthLPow_SpellMaterial;
			break;
		case SpellID::IncDur:
			return EarthHDur_SpellMaterial;
			break;
		case SpellID::DecDur:
			return EarthLDur_SpellMaterial;
			break;
		case SpellID::Explode:
			return EarthExplode_SpellMaterial;
			break;
		case SpellID::Magnet:
			return EarthMagnet_SpellMaterial;
			break;
		default:
			return Earth_SpellMaterial;
			break;
		}
		break;
	}

	return nullptr;
}


// Spell specific functions
void ASpell::UpdateCollision()
{
	// Must be overridden in child classes to work
}

void ASpell::UpdatePosition()
{
	// Must be overridden in child classes to work
}

void ASpell::EndSpell()
{
	Destroy();
}

// Generic spell functions
void ASpell::RemoveEnergy(float Energy)
{
	CurrentEnergy -= Energy;
}

// Returns factor of how complete the distance of current position is relative to endpos-startpos
float ASpell::GetDistanceFactor(FVector StartPos, FVector EndPos, FVector CurrentPos)
{
	float TotalLength{ (EndPos - StartPos).Size() };
	float CurrentLength{ (CurrentPos - StartPos).Size() };

	if (TotalLength != 0) { // Div 0 check
		return CurrentLength / TotalLength;
	}
	return -1.f; // Negative distance is not exactly normal
}

float ASpell::GetDistanceFactor(FVector StartPos, FVector CurrentPos, float Range)
{
	float CurrentLength{ (CurrentPos - StartPos).Size() };

	if (Range != 0) { // Div 0 check
		return CurrentLength / Range;
	}
	return -1.f; // Negative distance is not exactly normal
}
