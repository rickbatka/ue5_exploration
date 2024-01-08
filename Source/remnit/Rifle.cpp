// Fill out your copyright notice in the Description page of Project Settings.


#include "Rifle.h"

#include "ComponentReregisterContext.h"
#include "remnitCharacter.h"

bool URifle::OnNotifyBegin(const FName NotifyName, UAnimSequenceBase* Animation)
{
	return false;
}

bool URifle::OnNotifyEnd(const FName NotifyName, UAnimSequenceBase* Animation)
{
	return false;
}

void URifle::BeginPlay()
{
	Super::BeginPlay();
	Character = GetOwner<ARemnitCharacter>();
	if (!Character)
	{
		UE_LOG(LogActorComponent, Error, TEXT("Rifle component can only be attached to ACharacters!"));
		UActorComponent::DestroyComponent();
	}

	/**
	 * TODO RICK
	 * - Switch to gun when aiming, back to sword when not (actually create / destroy actor components? make sure all pointer accesses are safe)
	 *   - Have to re-regsiter input action listeners to properly find components at action time, in case they've been switched out
	 * - Play equipping montage when starting aiming
	 * - Add shooting action & animation (doesn't need to do anything)
	 * - Implement camera zoom & reticle
	 * - Fix aiming left hand attach to muzzle (IK?)
	 */
	
}

void URifle::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
