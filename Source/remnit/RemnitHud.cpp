// Fill out your copyright notice in the Description page of Project Settings.


#include "RemnitHud.h"

void ARemnitHud::BeginPlay()
{
	Super::BeginPlay();
	bShowOverlays = true;
}

void ARemnitHud::DrawHUD()
{
	Super::DrawHUD();
}
