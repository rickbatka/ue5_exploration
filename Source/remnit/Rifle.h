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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* ShootMontage;
	
protected:
	ARemnitCharacter* Character;
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
