// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "remnitCharacter.generated.h"

struct FEnhancedInputActionValueBinding;
class UWeaponSM;
class URifle;
class ULockOnComponent;
class UDodgeRollComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ARemnitCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;


	/** SwordMedium Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackPrimaryAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LockOnAction;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

	
	
	USkeletalMeshSocket* WeaponSocketRStock;
	USkeletalMeshSocket* WeaponSocketRMuzzle;
public:

	ARemnitCharacter();

	bool GetCanTakeAnyAction() const;
	bool GetIsRolling() const;
	bool GetIsLockedAttacking() const;
	void TryAttack();

	//UFUNCTION()
	//void StartIFrames();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float CameraBoomLength_Melee = 400;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FVector CameraBoomOffset_Melee = {0, 0, 100};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float CameraBoomLength_Rifle = 235;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FVector CameraBoomOffset_Rifle = {0, -50, 100};


	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Interp)
	FTransform GunAimTransform;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Interp)
	FRotator GunAimRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Interp)
	FTransform RecoilTransform;
	

	UFUNCTION(BlueprintGetter)
	FTransform GetWeaponMuzzleTransform() const;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


	UDodgeRollComponent* DodgeRollComponent;
	ULockOnComponent* LockOnComponent;
	UWeaponSM* SwordComponent;
	URifle* RifleComponent;

	void EquipSword() const;
	void EquipRifle() const;

	FEnhancedInputActionValueBinding* CurrentIsAimingInputValue;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void TryStartAiming();
	void TryStopAiming();
	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	/**
	 * Important! the "AnimNotify_" prefix is a magic string. The call to
	 * AnimInstance->AddExternalNotifyHandler will not work without that
	 * magic prefix, even though it looks like it all works!
	 */
	// UFUNCTION(BlueprintCallable)
	// virtual void AnimNotify_StartIFrames();
};
