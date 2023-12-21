// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_GenericAnimStateNotifier.generated.h"

UINTERFACE(MinimalAPI)
class UAnimStateListener : public UInterface
{
	GENERATED_BODY()
};

class IAnimStateListener
{
	GENERATED_BODY()

public:
	virtual bool OnNotifyBegin(const FName NotifyName) { return false; }
	virtual bool OnNotifyEnd(const FName NotifyName) { return false; }
};

/**
 * 
 */
UCLASS()
class REMNIT_API UANS_GenericAnimStateNotifier : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
