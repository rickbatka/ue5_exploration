// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyStates/ANS_GenericAnimStateNotifier.h"
#include "Components/StaticMeshComponent.h"
#include "WeaponSM.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class REMNIT_API UWeaponSM : public UStaticMeshComponent, public IAnimStateListener
{
	GENERATED_BODY()
	virtual bool OnNotifyBegin(const FName NotifyName, UAnimSequenceBase* Animation) override;
	virtual bool OnNotifyEnd(const FName NotifyName, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> AttackMontages;

protected:
	ACharacter* Character;
	int AttackIndex = 0;
	bool bIsAttacking = false;

	// This is true for the last few frames of the attack anim where we accept the next input
	bool bIsComboWindowOpen = false;
	UAnimMontage* CurrentPlayingMontage;

	void BeginAttack();
	void EndCurrentAttack();
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void TryAttack();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
