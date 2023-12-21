// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ANS_GenericAnimStateNotifier.h"
#include "Components/ActorComponent.h"
#include "DodgeRollComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class REMNIT_API UDodgeRollComponent : public UActorComponent, public IAnimStateListener
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float RollSpeed = 10.0;

	UPROPERTY(BlueprintReadOnly)
	bool bIsRolling = false;

	UPROPERTY(BlueprintReadOnly)
	FVector RollDirection;
	FVector VelocityBeforeRoll;
	FLinearColor BodyColorBeforeRoll;

	// Sets default values for this component's properties
	UDodgeRollComponent();
	virtual bool OnNotifyBegin(const FName NotifyName) override;
	virtual bool OnNotifyEnd(const FName NotifyName) override;
	void TryRoll();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	ACharacter* Character;
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollForwardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollBackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollLeftMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollRightMontage;
};
