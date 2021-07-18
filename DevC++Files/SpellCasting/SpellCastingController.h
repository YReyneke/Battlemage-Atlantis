// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.


/*	This class controls whether the post-cast spell is applied
*	If so, does that spell override any already active spells
*	When player launches spell, play relevant launch animation and clear the active spell
*/
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Spell.h"
#include "SpellCastingController.generated.h"


UCLASS(Blueprintable)
class BATTLEMAGEATLANTIS01_API USpellCastingController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpellCastingController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// The ApplySpell Functions: - These apply the relevant materials and spawn the relevant pre-launch actors
	void ApplyRHSpell(SpellID spell);
	void ApplyLHSpell(SpellID spell);
	void ApplyDualHSpell(SpellID spell);

	// NOTE: Launched spells include transition animation from pre-launch (around hand) state to actual spell
	void LaunchRHSpell();
	void LaunchLHSpell();
	void LaunchDualHSpell();
	
private:
	// Variables
	
	// The various applied spell types structure
	SpellID ActiveRHSpell{ SpellID::None };
	SpellID ActiveLHSpell{ SpellID::None };
	SpellID ActiveElement{ SpellID::None };
	SpellID ActiveModifier{ SpellID::None };

	// Background Functions
	void UpdateAimpoints(); // Will include a switch aim to dual/1h mode button one day, for now we display both simultaneously
	void UpdateHandAnimations(); // Moves the current pre-launch animation along with relevant controller
	
	// Above is the foundation, anything beyond this point is purely infrastructure and implementation used by the above functions...

	void ResetSecondarySpells();
private:

	class UCameraComponent* hmdCamera{};
	class UMotionControllerComponent* LHand{};
	class UMotionControllerComponent* RHand{};

	// Spell count limiters
	// NOTE: it is possible to cast while a spell is active, however too many of the same type of spell running simultaneously is not allowed
	// Only one Atune can be applied at any given time
	// Infinite balls per hand
	// Two walls per hand
	TArray<class ASpell_Wall*> ActiveLHWalls{ nullptr, nullptr };
	TArray<class ASpell_Wall*> ActiveRHWalls{ nullptr, nullptr };
	
	// One beam per hand
	class ASpell_Beam* ActiveLHBeam{};
	class ASpell_Beam* ActiveRHBeam{};

private: // All properties that must be filled in to SpellCastingController_BP

	// The hand animation actors for each spell (TSubclassOf<AHandSpell>*)

	// The various spell actor blueprints that will be instanced every time a spell is successfully cast (TSubclassOf<ASpell>)
	UPROPERTY(EditAnywhere, category = "SpellBlueprints")
	TSubclassOf<class ASpell_Wall> WallSpell_BP;
	UPROPERTY(EditAnywhere, category = "SpellBlueprints")
	TSubclassOf<class ASpell_Ball> BallSpell_BP;
	UPROPERTY(EditAnywhere, category = "SpellBlueprints")
	TSubclassOf<class ASpell_Beam> BeamSpell_BP;

	// Default value properties

	// Maximum number of times elements and modifiers can be chained
	UPROPERTY(EditAnywhere, category = "DefaultVariables")
	int MaxChainCount{ 5 };

	int CurrentChainCount{ 0 };

public:
	
	// MUST be called by USpellComponent::SetupHands();
	void ConnectMotionControllers(UMotionControllerComponent* LeftController, UMotionControllerComponent* RightController, UCameraComponent* HMD);

};