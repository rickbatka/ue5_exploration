// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_GenericAnimStateNotifier.h"

#include "remnitCharacter.h"

void UANS_GenericAnimStateNotifier::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	const auto OwningActor = MeshComp->GetOwner();
	if (!OwningActor)
	{
		return;
	}
	for (const auto Component : OwningActor->GetComponents())
	{
		if (IAnimStateListener* AnimStateHandler = Cast<IAnimStateListener>(Component))
		{
			/**
			 * Give the component a chance to handle it, and if it does, stop propagating the event.
			 */
			if (AnimStateHandler->OnNotifyBegin(EventReference.GetNotify()->NotifyName))
			{
				return;
			}
		}
	}

	/**
	 * No components handled the event, give the owning actor a chance to handle it.
	 */
	if (IAnimStateListener* AnimOwner = Cast<IAnimStateListener>(OwningActor))
	{
		AnimOwner->OnNotifyBegin(EventReference.GetNotify()->NotifyName);
	}
}

void UANS_GenericAnimStateNotifier::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	const auto OwningActor = MeshComp->GetOwner();
	if (!OwningActor)
	{
		return;
	}

	for (const auto Component : OwningActor->GetComponents())
	{
		if (IAnimStateListener* AnimStateHandler = Cast<IAnimStateListener>(Component))
		{
			/**
			 * Give the component a chance to handle it, and if it does, stop propagating the event.
			 */
			if (AnimStateHandler->OnNotifyEnd(EventReference.GetNotify()->NotifyName))
			{
				return;
			}
		}
	}

	/**
	 * No components handled the event, give the owning actor a chance to handle it.
	 */
	if (IAnimStateListener* AnimOwner = Cast<IAnimStateListener>(OwningActor))
	{
		AnimOwner->OnNotifyEnd(EventReference.GetNotify()->NotifyName);
	}
}
