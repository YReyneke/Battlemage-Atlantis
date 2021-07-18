// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CastingNode.generated.h"

UCLASS(DefaultToInstanced)
class BATTLEMAGEATLANTIS01_API ACastingNode : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACastingNode();

	// Static Mesh
	UPROPERTY(EditDefaultsOnly, category = "MeshComponent")
	UStaticMeshComponent* StaticMesh;

	// Various castingpoint materials
	UPROPERTY(EditAnywhere, category = "Materials")
	class UMaterial* OrangeMaterial;
	UPROPERTY(EditAnywhere, category = "Materials")
	class UMaterialInstance* RedMaterial;
	UPROPERTY(EditAnywhere, category = "Materials")
	class UMaterialInstance* GreenMaterial;
	UPROPERTY(EditAnywhere, category = "Materials")
	class UMaterialInstance* BlueMaterial;
	UPROPERTY(EditAnywhere, category = "Materials")
	class UMaterialInstance* PurpleMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};