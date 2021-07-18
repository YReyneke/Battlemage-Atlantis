// Copyright 2021 Yacob N. S. Reyneke All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */



class BATTLEMAGEATLANTIS01_API SpellHandler
{
public:
	SpellHandler();
	~SpellHandler();


public: // Access functions
	
	// Complete spell: 

	void CompleteSpell();
	void LaunchSpell();

private:

	void CheckCompatibility();
	
};
