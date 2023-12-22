// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


class ARemnitCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class REMNIT_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULockOnComponent();

	bool bIsLockedOn;
	AActor* Target;

	UPROPERTY(EditAnywhere, Category = Input)
	float ToggleCooldownSec = 0.5;
	
	/**
	 * 
	 */
	bool bIsToggleOnCooldown;

protected:
	ARemnitCharacter* Character;
	// Called when the game starts
	virtual void BeginPlay() override;
	void PreventToggleForCooldownDuration();

public:
	void ToggleLockOn();
	
	void RemoveLock();
	void SetLock(AActor* NewTarget);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
