// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "VRRacer.h"
#include "VRRacerGameMode.h"
#include "VRRacerPawn.h"

AVRRacerGameMode::AVRRacerGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AVRRacerPawn::StaticClass();
}
