// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_GenericAnimStateNotifier.h"

#include "remnitCharacter.h"

void UANS_GenericAnimStateNotifier::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (IAnimStateListener* AnimOwner = MeshComp->GetOwner<IAnimStateListener>()) {
		AnimOwner->OnNotifyBegin(EventReference.GetNotify()->NotifyName);
	}
}

void UANS_GenericAnimStateNotifier::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (IAnimStateListener* AnimOwner = MeshComp->GetOwner<IAnimStateListener>()) {
		AnimOwner->OnNotifyEnd(EventReference.GetNotify()->NotifyName);
	}
}
