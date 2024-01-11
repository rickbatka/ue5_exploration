#include "Rifle.h"

#include <map>
#include <random>

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
	if(!Character || bIsFiring)
	{
		return;
	}
	Fire();
}

void URifle::Fire()
{
	bIsFiring = true;

	// Get a random kickback for this shot. Weight toward the top end, hence the max of `(KickbackPower * 1.33)` which is 30% above the actual max
	const float KickbackMin = KickbackPower - RecoilVariance;
	const float ThisBulletKickback = -1 * FMath::Clamp( (UKismetMathLibrary::RandomFloat() * (KickbackPower * 1.33)), KickbackMin, KickbackPower);
	
	// Kickback (recoil). The character will regain composure after some time according to the MarksmanSteadiness variable
	Character->RecoilTransform.SetLocation({0 , ThisBulletKickback, -ThisBulletKickback / 4});
	Character->RecoilTransform.SetRotation(UE::Math::TQuat<double>(UE::Math::TRotator<double>{0, 0 , ThisBulletKickback * 2}));

	// Initiate cooldown until next bullet can be fired
	GetWorld()->GetTimerManager().SetTimer(FireCooldownTimer, this, &URifle::FireCooldownExpired, FireRateCooldown, false);
	UE_LOG(LogActorComponent, Error, TEXT("Kickback! %f"), ThisBulletKickback);
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
	
	Character = GetOwner<ARemnitCharacter>();
	if (!Character)
	{
		UE_LOG(LogActorComponent, Error, TEXT("Rifle component can only be attached to ACharacters!"));
		UActorComponent::DestroyComponent();
	}

	/**
	 * TODO RICK
	 * - Implement camera zoom & reticle
	 * - Fix left hand IK (or not, whatever.)
	 */
	
}

void URifle::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bIsFiring)
	{
		if(Character->RecoilTransform.Rotator().IsNearlyZero(0.5) && Character->RecoilTransform.GetLocation().IsNearlyZero(0.5))
		{
			Character->RecoilTransform.SetLocation(FTransform::Identity.GetLocation());
			Character->RecoilTransform.SetRotation(FTransform::Identity.GetRotation());
		}
		else
		{
			const auto CurrentStep = UKismetMathLibrary::TInterpTo(Character->RecoilTransform, FTransform::Identity, DeltaTime, MarksmanSteadiness);
			Character->RecoilTransform.SetLocation(CurrentStep.GetLocation());
			Character->RecoilTransform.SetRotation(CurrentStep.GetRotation());
		}
	}
}
