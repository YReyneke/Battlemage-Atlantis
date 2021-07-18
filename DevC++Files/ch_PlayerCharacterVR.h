// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ch_PlayerCharacterVR.generated.h"

UCLASS(Blueprintable)
class BATTLEMAGEATLANTIS01_API Ach_PlayerCharacterVR : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	Ach_PlayerCharacterVR(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MovementSensitivity{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MovementDeadZone{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationSensitivity{};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private: // Components
	UPROPERTY(EditAnywhere)
	class USceneComponent* VRRootComponent;
	
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* hmdCamera;

	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* RightMotionController;

	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* LeftMotionController;
	
	UPROPERTY(VisibleAnywhere, category="Setup")
	class USpellComponent* SpellComponent{};

private: // Custom Functions
	void MoveForward(float movement);
	void MoveRight(float movement);
	void RotatePlayspace(float rotation);

	void RightHandCast();
	void LeftHandCast();
	void RightHandStopCast();
	void LeftHandStopCast();

	void RightHandLaunch();
	void LeftHandLaunch();
	void RightHandStopLaunch();
	void LeftHandStopLaunch();

private: // ***Debug/test section***

	void DebugRelativeHandPositions();

	// void SendSpellsToFile();
	// Spell casting motion test script
	// int castNumber{0};

	// FString CastLogFile = FPaths::ProjectLogDir();
	// FString CastFileContent{};
	// IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	// void StartLHMotionRecord();
	// void EndLHMotionRecord();
	// void StartRHMotionRecord();
	// void EndRHMotionRecord();
};
