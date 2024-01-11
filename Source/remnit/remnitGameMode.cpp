// Copyright Epic Games, Inc. All Rights Reserved.

#include "remnitGameMode.h"
#include "remnitCharacter.h"
#include "RemnitHud.h"
#include "UObject/ConstructorHelpers.h"

AremnitGameMode::AremnitGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<ARemnitHud> RemnitHUDBPClass(TEXT("Script/Engine.Blueprint'/Game/Remnit/BP_RemnitHud.BP_RemnitHud_C'"));
	if (RemnitHUDBPClass.Class != NULL)
	{
		HUDClass = RemnitHUDBPClass.Class;
	}
}
