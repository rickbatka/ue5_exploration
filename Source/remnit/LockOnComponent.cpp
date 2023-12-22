// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnComponent.h"

#include "ComponentReregisterContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


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
	Character = GetOwner<ACharacter>();
	if (!Character)
	{
		UE_LOG(LogActorComponent, Error, TEXT("Lock On component can only be attached to ACharacters!"));
		UActorComponent::DestroyComponent();
	}
	bIsLockedOn = false;
	//Character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
}

void ULockOnComponent::ToggleLockOn()
{
	if(bIsLockedOn)
	{
		RemoveLock();
		return;
	}
	
	TArray<AActor*> OutFoundLockOnTargets;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "LOCK", OutFoundLockOnTargets);
	if(OutFoundLockOnTargets.Num() == 0)
	{
		return;
	}
	SetLock(OutFoundLockOnTargets[0]);
}

void ULockOnComponent::RemoveLock()
{
	bIsLockedOn = false;
	Character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ULockOnComponent::SetLock(AActor* NewTarget)
{
	if(!NewTarget)
	{
		return;
	}
	this->Target = NewTarget;
	bIsLockedOn = true;
	
	// TODO RICK editing ABP_Remnit1,
	// // Trying to get directiopn / rotation into blendspace
}


// Called every frame
void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// In case our target was destroyed
	if(bIsLockedOn && !Target)
	{
		RemoveLock();
	}
	
	if(bIsLockedOn && Target)
	{
		GetOwner()->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), Target->GetActorLocation()));
	}
}

