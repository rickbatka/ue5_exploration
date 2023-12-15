// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_IFramesAnimNotifyState.h"

void UANS_IFramesAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	
	AremnitCharacter* playerCharacter = MeshComp->GetOwner<AremnitCharacter>();
	if (playerCharacter) {
		playerCharacter->RollIFramesStarted();
	}
}

void UANS_IFramesAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) {
}

void UANS_IFramesAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	
	AremnitCharacter* playerCharacter = MeshComp->GetOwner<AremnitCharacter>();
	if (playerCharacter) {
		playerCharacter->RollIFramesEnded();
	}
}