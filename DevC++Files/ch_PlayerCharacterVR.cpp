// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


#include "ch_PlayerCharacterVR.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "SpellCasting/SpellComponent.h"
#include "MotionControllerComponent.h"

// Sets default values
Ach_PlayerCharacterVR::Ach_PlayerCharacterVR(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Default Vars
	MovementSensitivity = 1;
	MovementDeadZone = 0.2;
	RotationSensitivity = 3;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("VR Root Component"));
	VRRootComponent->SetupAttachment(GetRootComponent());

	hmdCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera Coponent"));
	hmdCamera->SetupAttachment(VRRootComponent);

	RightMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right Motion Controller"));
	LeftMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left Motion Controller"));
	RightMotionController->SetupAttachment(VRRootComponent);
	LeftMotionController->SetupAttachment(VRRootComponent);
	RightMotionController->SetTrackingSource(EControllerHand::Right);
	LeftMotionController->SetTrackingSource(EControllerHand::Left);

	SpellComponent = CreateDefaultSubobject<USpellComponent>(TEXT("SpellControllingComponent"));
	if (SpellComponent) {
		SpellComponent->SetupAttachment(VRRootComponent);
		if (!SpellComponent->SetupHands(LeftMotionController, RightMotionController, hmdCamera)) {
			UE_LOG(LogTemp, Error, TEXT("Spell Component initialised with invalid setup components. Ensure you have all Fields in SpellComponent->Setup filled correctly"));
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("FAILED TO CREATE SPELLCOMPONENT!"));
	}

	//SendSpellsToFile(); // DEV ONLY
	/*// *** DEV ONLY ***
	CastLogFile.Append(TEXT("CastLog.txt"));
	if (FileManager.FileExists(*CastLogFile)) {
		FFileHelper::LoadFileToString(CastFileContent, *CastLogFile);
	}
	else {
		CastFileContent = TEXT("This is a logfile including all Spellasting motions performed - ever:\n\n");
		FFileHelper::SaveStringToFile(CastFileContent, *CastLogFile);
	}*/
}

// Called when the game starts or when spawned
void Ach_PlayerCharacterVR::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void Ach_PlayerCharacterVR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update real world playspace center to relative hmd position in game.
	FVector NewCameraOffset{ hmdCamera->GetComponentLocation() - GetActorLocation() }; // How much player moved in real world since last frame
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	VRRootComponent->AddWorldOffset(-NewCameraOffset);
}

// Called to bind functionality to input
void Ach_PlayerCharacterVR::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("OculusThumbFWBW"), this, &Ach_PlayerCharacterVR::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("OculusThumbRL"), this, &Ach_PlayerCharacterVR::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("OculusThumbRotate"), this, &Ach_PlayerCharacterVR::RotatePlayspace);

	PlayerInputComponent->BindAction(TEXT("RHCast"), EInputEvent::IE_Pressed, this, &Ach_PlayerCharacterVR::RightHandCast);
	PlayerInputComponent->BindAction(TEXT("LHCast"), EInputEvent::IE_Pressed, this, &Ach_PlayerCharacterVR::LeftHandCast);
	PlayerInputComponent->BindAction(TEXT("RHCast"), EInputEvent::IE_Released, this, &Ach_PlayerCharacterVR::RightHandStopCast);
	PlayerInputComponent->BindAction(TEXT("LHCast"), EInputEvent::IE_Released, this, &Ach_PlayerCharacterVR::LeftHandStopCast);

	PlayerInputComponent->BindAction(TEXT("RHLaunch"), EInputEvent::IE_Pressed, this, &Ach_PlayerCharacterVR::RightHandLaunch);
	PlayerInputComponent->BindAction(TEXT("LHLaunch"), EInputEvent::IE_Pressed, this, &Ach_PlayerCharacterVR::LeftHandLaunch);
	PlayerInputComponent->BindAction(TEXT("RHLaunch"), EInputEvent::IE_Released, this, &Ach_PlayerCharacterVR::RightHandStopLaunch);
	PlayerInputComponent->BindAction(TEXT("LHLaunch"), EInputEvent::IE_Released, this, &Ach_PlayerCharacterVR::LeftHandStopLaunch);
}

void Ach_PlayerCharacterVR::MoveForward(float movement) {
	if (FMath::Abs(movement) > MovementDeadZone) {
		AddMovementInput(hmdCamera->GetForwardVector(), movement * MovementSensitivity, false);
	}
}

void Ach_PlayerCharacterVR::MoveRight(float movement) {
	if (FMath::Abs(movement) > MovementDeadZone) {
		AddMovementInput(hmdCamera->GetRightVector(), movement * MovementSensitivity, false);
	}
}

void Ach_PlayerCharacterVR::RotatePlayspace(float rotation)
{
	if (FMath::Abs(rotation) > MovementDeadZone) {
		FRotator DeltaRot{ 0, rotation * RotationSensitivity, 0 };
		FVector CameraDeltaPos{ hmdCamera->GetComponentLocation() };
		VRRootComponent->AddWorldRotation(DeltaRot);
		CameraDeltaPos -= hmdCamera->GetComponentLocation();
		CameraDeltaPos.Z = 0;
		AddActorWorldOffset(CameraDeltaPos);
	}
}

void Ach_PlayerCharacterVR::RightHandCast()
{
	if (SpellComponent) {
		SpellComponent->RightHandCast();
		//DebugRelativeHandPositions();
	}
}

void Ach_PlayerCharacterVR::LeftHandCast()
{
	SpellComponent->LeftHandCast();
	//DebugRelativeHandPositions();
}

void Ach_PlayerCharacterVR::RightHandStopCast()
{
	SpellComponent->RightHandStopCast();
}

void Ach_PlayerCharacterVR::LeftHandStopCast()
{
	SpellComponent->LeftHandStopCast();
}

void Ach_PlayerCharacterVR::RightHandLaunch()
{
	SpellComponent->RightHandLaunch();
}

void Ach_PlayerCharacterVR::LeftHandLaunch()
{
	SpellComponent->LeftHandLaunch();
}

void Ach_PlayerCharacterVR::RightHandStopLaunch()
{
	SpellComponent->RightHandStopLaunch();
}

void Ach_PlayerCharacterVR::LeftHandStopLaunch()
{
	SpellComponent->LeftHandStopLaunch();
}

// *** DEV ONLY remove before release ***
void Ach_PlayerCharacterVR::DebugRelativeHandPositions()
{
	FVector LHPosToSpellGrid{ SpellComponent->ToSpellcastingGrid(LeftMotionController->GetComponentLocation()) };
	FVector RHPosToSpellGrid{ SpellComponent->ToSpellcastingGrid(RightMotionController->GetComponentLocation()) };
	FVector RelativeHPos{ RHPosToSpellGrid - LHPosToSpellGrid };
	FRotator LHRotToSpellGrid{ SpellComponent->ToSpellcastingGrid(LeftMotionController->GetComponentRotation()) };
	FRotator RHRotToSpellGrid{ SpellComponent->ToSpellcastingGrid(RightMotionController->GetComponentRotation()) };
	
	UE_LOG(LogTemp, Warning, TEXT("LH SCG POS: %s; RH SCG POS: %s; R->L SCG POS: %s"), *LHPosToSpellGrid.ToString(), *RHPosToSpellGrid.ToString(), *RelativeHPos.ToString());
	UE_LOG(LogTemp, Warning, TEXT("LH SCG ROT: %s; RH SCG ROT: %s"), *LHRotToSpellGrid.ToString(), *RHRotToSpellGrid.ToString());
}

/*
void Ach_PlayerCharacterVR::StartLHMotionRecord()
{
	LHPos = LeftMotionController->GetRelativeLocation(); // Relative to VRRootComponent
	LHRot = LeftMotionController->GetRelativeRotation(); // Relative to VRRootComponent
	castNumber++;

	CastFileContent.Append(TEXT("Start Cast: ")); CastFileContent.Append(FString::FromInt(castNumber));
	CastFileContent.Append(TEXT("\n     Hand: Left\n     Relative Pos: ")); CastFileContent.Append(LHPos.ToString());
	CastFileContent.Append(TEXT("\n     Relative Rot: ")); CastFileContent.Append(LHRot.ToString()); CastFileContent.Append(TEXT("\n\n"));

	FFileHelper::SaveStringToFile(CastFileContent, *CastLogFile);
}


void Ach_PlayerCharacterVR::StartRHMotionRecord()
{
	RHPos = RightMotionController->GetRelativeLocation(); // Relative to VRRootComponent
	RHRot = RightMotionController->GetRelativeRotation(); // Relative to VRRootComponent
	castNumber++;

	CastFileContent.Append(TEXT("Start Cast: ")); CastFileContent.Append(FString::FromInt(castNumber));
	CastFileContent.Append(TEXT("\n     Hand: Right\n     Relative Pos: ")); CastFileContent.Append(RHPos.ToString());
	CastFileContent.Append(TEXT("\n     Relative Rot: ")); CastFileContent.Append(RHRot.ToString()); CastFileContent.Append(TEXT("\n\n"));

	FFileHelper::SaveStringToFile(CastFileContent, *CastLogFile);
}

void Ach_PlayerCharacterVR::EndLHMotionRecord()
{
	FVector EndPos{ LeftMotionController->GetRelativeLocation() }; // Relative to VRRootComponent
	FRotator EndRot{ LeftMotionController->GetRelativeRotation() }; // Relative to VRRootComponent
	FVector DeltaPos{ LHPos - EndPos };
	FRotator DeltaRot{ LHRot - EndRot };

	CastFileContent.Append(TEXT("End Cast: ")); CastFileContent.Append(FString::FromInt(castNumber));
	CastFileContent.Append(TEXT("\n     Hand: Left\n     Relative Pos: ")); CastFileContent.Append(EndPos.ToString());
	CastFileContent.Append(TEXT("\n     Relative Rot: ")); CastFileContent.Append(EndRot.ToString());
	CastFileContent.Append(TEXT("\n     Delta Pos: ")); CastFileContent.Append(DeltaPos.ToString());
	CastFileContent.Append(TEXT("\n     Delta Rot: ")); CastFileContent.Append(DeltaRot.ToString()); CastFileContent.Append(TEXT("\n\n"));

	FFileHelper::SaveStringToFile(CastFileContent, *CastLogFile);
}

void Ach_PlayerCharacterVR::EndRHMotionRecord()
{
	FVector EndPos{ RightMotionController->GetRelativeLocation() }; // Relative to VRRootComponent
	FRotator EndRot{ RightMotionController->GetRelativeRotation() }; // Relative to VRRootComponent
	FVector DeltaPos{ RHPos - EndPos };
	FRotator DeltaRot{ RHRot - EndRot };

	CastFileContent.Append(TEXT("End Cast: ")); CastFileContent.Append(FString::FromInt(castNumber));
	CastFileContent.Append(TEXT("\n     Hand: Right\n     Relative Pos: ")); CastFileContent.Append(EndPos.ToString());
	CastFileContent.Append(TEXT("\n     Relative Rot: ")); CastFileContent.Append(EndRot.ToString());
	CastFileContent.Append(TEXT("\n     Delta Pos: ")); CastFileContent.Append(DeltaPos.ToString());
	CastFileContent.Append(TEXT("\n     Delta Rot: ")); CastFileContent.Append(DeltaRot.ToString()); CastFileContent.Append(TEXT("\n\n"));

	FFileHelper::SaveStringToFile(CastFileContent, *CastLogFile);
}*/
