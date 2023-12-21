// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeRollComponent.h"

#include "ComponentReregisterContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


bool UDodgeRollComponent::OnNotifyBegin(const FName NotifyName)
{
	if (!Character) { return false; }
	/**
	 * Start Rolling
	 */
	if (NotifyName == "ANS_DodgeRoll")
	{
		VelocityBeforeRoll = Character->GetCharacterMovement()->GetLastUpdateVelocity();
		RollDirection = Character->GetCharacterMovement()->GetLastUpdateVelocity();
		// Roll forward if character was standing still
		if (RollDirection.IsZero())
		{
			RollDirection = Character->GetTransform().GetRotation().GetForwardVector();
		}
		RollDirection.Normalize();
		bIsRolling = true;
		Character->GetCharacterMovement()->DisableMovement();
		return true;
	}
	/**
	 * Start IFrames
	 */
	if (NotifyName == "ANS_IFramesAnimNotifyState")
	{
		// TODO Hack: Assuming body material is first material on mannequin.
		if (const auto Material = Character->GetMesh()->GetMaterials()[0])
		{
			TArray<FMaterialParameterInfo> OutParameterInfo;
			TArray<FGuid> OutParameterIds;
			Material->GetAllVectorParameterInfo(OutParameterInfo, OutParameterIds);
			for (int i = 0; i < OutParameterInfo.Num(); ++i)
			{
				if (OutParameterInfo[i].Name == "BodyColor")
				{
					Material->GetVectorParameterValue(OutParameterInfo[i], BodyColorBeforeRoll);
					Character->GetMesh()->CreateDynamicMaterialInstance(0, Material)->SetVectorParameterValue("BodyColor", FLinearColor::Green);
				}
			}
		}
		return true;
	}
	return false;
}

bool UDodgeRollComponent::OnNotifyEnd(const FName NotifyName)
{
	if (!Character) { return false; }
	/**
	 * End Dodge Roll
	 */
	if (NotifyName == "ANS_DodgeRoll" && bIsRolling)
	{
		bIsRolling = false;

		if (Character->GetMovementComponent()->GetLastInputVector().IsZero())
		{
			// Stop as soon as possible if no movement input - so the character doesn't "glide" to a stop after a roll.
			Character->GetMovementComponent()->ConsumeInputVector();
		}
		else
		{
			// Hack, exceed the speed limit to ensure they are running after roll, it feels nicer
			Character->GetMovementComponent()->Velocity = VelocityBeforeRoll + 20;
			Character->GetMovementComponent()->UpdateComponentVelocity();
		}

		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		return true;
	}

	/**
	 * End IFrames
	 */
	if (NotifyName == "ANS_IFramesAnimNotifyState")
	{
		// TODO Hack: Assuming body material is first material on mannequin.
		if (const auto Material = Character->GetMesh()->GetMaterials()[0])
		{
			Character->GetMesh()->CreateDynamicMaterialInstance(0, Material)->SetVectorParameterValue("BodyColor", BodyColorBeforeRoll);
		}
		return true;
	}
	return false;
}

void UDodgeRollComponent::TryRoll()
{
	if (!Character || bIsRolling || Character->GetCharacterMovement()->IsFalling())
	{
		return;
	}

	if (const auto Duration = Character->PlayAnimMontage(RollForwardMontage, 1.25); Duration == 0.)
	{
		UE_LOG(LogActorComponent, Error, TEXT("Failed to play RollForwardMontage; bailing"));
		bIsRolling = false;
	}

	// If the animation plays, it will start when the ANS_AnimRunning state triggers
}

// Sets default values for this component's properties
UDodgeRollComponent::UDodgeRollComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UDodgeRollComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = GetOwner<ACharacter>();
	if (!Character)
	{
		UE_LOG(LogActorComponent, Error, TEXT("Dodge Roll component can only be attached to ACharacters!"));
		UActorComponent::DestroyComponent();
	}
}


// Called every frame
void UDodgeRollComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Character) { return; }

	if (bIsRolling)
	{
		Character->ConsumeMovementInputVector();
		Character->SetActorLocation(Character->GetActorLocation() + RollDirection * RollSpeed, true);
	}
}
