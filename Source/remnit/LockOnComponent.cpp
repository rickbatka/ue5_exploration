// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnComponent.h"

#include "AITestsCommon.h"
#include "AITestsCommon.h"
#include "ComponentReregisterContext.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "remnitCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
ULockOnComponent::ULockOnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = GetOwner<ARemnitCharacter>();
	if (!Character)
	{
		UE_LOG(LogActorComponent, Error, TEXT("Lock On component can only be attached to ACharacters!"));
		UActorComponent::DestroyComponent();
	}
	bIsLockedOn = false;
}

void ULockOnComponent::PreventToggleForCooldownDuration()
{
	bIsToggleOnCooldown = true;
	FTimerHandle InOutTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(InOutTimerHandle, [&]()
	{
		bIsToggleOnCooldown = false;
	}, 1, false, ToggleCooldownSec);
}

void ULockOnComponent::ToggleLockOn()
{
	if(bIsToggleOnCooldown)
	{
		return;
	}
	
	if (bIsLockedOn)
	{
		RemoveLock();
		PreventToggleForCooldownDuration();
		return;
	}

	TArray<AActor*> OutFoundLockOnTargets;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "LOCK", OutFoundLockOnTargets);
	if (OutFoundLockOnTargets.Num() == 0)
	{
		return;
	}
	SetLock(OutFoundLockOnTargets[0]);
	PreventToggleForCooldownDuration();
}

void ULockOnComponent::RemoveLock()
{
	bIsLockedOn = false;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ULockOnComponent::SetLock(AActor* NewTarget)
{
	if (!NewTarget)
	{
		return;
	}
	this->Target = NewTarget;
	bIsLockedOn = true;
}


// Called every frame
void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// In case our target was destroyed
	if (bIsLockedOn && !Target)
	{
		RemoveLock();
	}

	if (bIsLockedOn && Target && !Character->GetIsRolling())
	{
		GetOwner()->SetActorRotation(FRotator(0, UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), Target->GetActorLocation()).Yaw, 0));
	}
}
