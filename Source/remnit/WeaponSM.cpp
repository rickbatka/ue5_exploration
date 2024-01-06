﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSM.h"

#include "ComponentReregisterContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

bool UWeaponSM::OnNotifyBegin(const FName NotifyName, UAnimSequenceBase* Animation)
{
	if (!Character)
	{
		return false;
	}

	const auto Montage = Cast<UAnimMontage>(Animation);
	if(!Montage)
	{
		return false;
	}

	if(Montage != CurrentPlayingMontage)
	{
		// Important! Ignore events for abandoned montage sequences.
		return false;
	}

	if (NotifyName == "ANS_ComboWindowOpen")
	{
		bIsComboWindowOpen = true;
		UE_LOG(LogActor, Error, TEXT("Combo Window %d"), bIsComboWindowOpen);
		return true;
	}
	return false;
}

bool UWeaponSM::OnNotifyEnd(const FName NotifyName, UAnimSequenceBase* Animation)
{
	if (!Character) { return false; }
	const auto Montage = Cast<UAnimMontage>(Animation);
	if(!Montage)
	{
		return false;
	}

	if(Montage != CurrentPlayingMontage)
	{
		// Important! Ignore events for abandoned montage sequences.
		return false;
	}
	
	if (NotifyName == "ANS_ComboWindowOpen")
	{
		bIsComboWindowOpen = false;
		UE_LOG(LogActor, Error, TEXT("Combo Window %d"), bIsComboWindowOpen);
		return true;
	}

	/**
	 * End Current Attack
	 */
	if (NotifyName == "ANS_AttackAnim")
	{
		UE_LOG(LogActor, Error, TEXT("ANS_AttackAnim END"));

		EndCurrentAttack();
		// An attack fully finished without being preempted by a combo; reset to the beginning attack again.
		AttackIndex = 0;
		return true;
	}
	
	return false;
}

void UWeaponSM::BeginAttack()
{
	//Character->StopAnimMontage(nullptr);
	UE_LOG(LogActor, Error, TEXT("Entering attack %d"), AttackIndex);
	CurrentPlayingMontage = AttackMontages[AttackIndex];
	Character->PlayAnimMontage(AttackMontages[AttackIndex]);
	bIsAttacking = true;
	bIsComboWindowOpen = false;
}

void UWeaponSM::EndCurrentAttack()
{
	UE_LOG(LogActor, Error, TEXT("EndCurrentAttack"));
	bIsAttacking = false;
	bIsComboWindowOpen = false;
	CurrentPlayingMontage = nullptr;
	AttackIndex++;
	if (AttackIndex > AttackMontages.Num() - 1)
	{
		AttackIndex = 0;
	}
}

void UWeaponSM::BeginPlay()
{
	Super::BeginPlay();
	Character = GetOwner<ACharacter>();
	if (!Character)
	{
		UE_LOG(LogActorComponent, Error, TEXT("Weapon component can only be attached to ACharacters!"));
		UActorComponent::DestroyComponent();
	}
}

void UWeaponSM::TryAttack()
{
	if ((bIsAttacking && !bIsComboWindowOpen) || Character->GetCharacterMovement()->IsFalling())
	{
		return;
	}

	if (bIsAttacking && bIsComboWindowOpen)
	{
		EndCurrentAttack();
	}

	if (!bIsAttacking)
	{
		BeginAttack();
	}
}

void UWeaponSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
