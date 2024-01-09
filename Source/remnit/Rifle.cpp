// Fill out your copyright notice in the Description page of Project Settings.


#include "Rifle.h"

#include "ComponentReregisterContext.h"
#include "remnitCharacter.h"
#include "Kismet/KismetMathLibrary.h"

bool URifle::OnNotifyBegin(const FName NotifyName, UAnimSequenceBase* Animation)
{
	return false;
}

bool URifle::OnNotifyEnd(const FName NotifyName, UAnimSequenceBase* Animation)
{
	return false;
}

URifle::URifle()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URifle::TryFire()
{
	if(!Character || !ShootMontage || bIsFiring)
	{
		return;
	}
	Fire();
}

void URifle::Fire()
{
	//GetWorld()->GetTimerManager().SetTimer(FireCooldownTimer, this, &URifle::FireCooldownExpired,1.,  false);
	bIsFiring = true;

	// TODO Rick Tuesday: IK the left hand onto the gun, IK the right forearm to the right hand during recoil.
	Character->RecoilTransform.SetLocation({0 , -6, 2.5});
	Character->RecoilTransform.SetRotation(UE::Math::TQuat<double>(UE::Math::TRotator<double>{0, 0 , -8}));
}

void URifle::FireCooldownExpired()
{
	bIsFiring = false;
	Character->RecoilTransform.SetLocation(FVector::ZeroVector);
	Character->RecoilTransform.SetRotation(UE::Math::TQuat<double>::Identity);
}

void URifle::BeginPlay()
{
	Super::BeginPlay();
	//PrimaryComponentTick.bCanEverTick = true;
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

	if(bIsFiring)
	{
		if(Character->RecoilTransform.Rotator().IsNearlyZero(0.5) && Character->RecoilTransform.GetLocation().IsNearlyZero(0.5))
		{
			FireCooldownExpired();
		}
		else
		{
			const auto CurrentStep = UKismetMathLibrary::TInterpTo(Character->RecoilTransform, FTransform::Identity, DeltaTime, 10);
			Character->RecoilTransform.SetLocation(CurrentStep.GetLocation());
			Character->RecoilTransform.SetRotation(CurrentStep.GetRotation());
		}
	}
}
