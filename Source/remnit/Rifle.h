// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyStates/ANS_GenericAnimStateNotifier.h"
#include "Components/StaticMeshComponent.h"
#include "Rifle.generated.h"

class ARemnitCharacter;
/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class REMNIT_API URifle : public UStaticMeshComponent, public IAnimStateListener
{
	GENERATED_BODY()

	virtual bool OnNotifyBegin(const FName NotifyName, UAnimSequenceBase* Animation) override;
	virtual bool OnNotifyEnd(const FName NotifyName, UAnimSequenceBase* Animation) override;

public:
	URifle();
	
	void TryFire();

	/**
	 * How hard does this gun kick recoil
	 * Recommend between 2 and 10
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KickbackPower = 6;

	/**
	 * Variance in recoil between shots.
	 * Recommend between 1 and 3
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecoilVariance = 3;

	/**
	 * How quickly does the shooter regain composure after a recoil kickback
	 * Recommend between 10 and 30
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MarksmanSteadiness = 10;

	/**
	 * Amount of time (in seconds) that must be waited in between shots
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRateCooldown = 0.25;
	
protected:
	ARemnitCharacter* Character;
	void Fire();
	bool bIsFiring = false;
	FTimerHandle FireCooldownTimer{};

	UFUNCTION()
	void FireCooldownExpired();
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
