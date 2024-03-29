// Copyright Epic Games, Inc. All Rights Reserved.

#include "remnitCharacter.h"

#include "ComponentReregisterContext.h"
#include "DodgeRollComponent.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "LockOnComponent.h"
#include "RemnitHud.h"
#include "Rifle.h"
#include "WeaponSM.h"
#include "Engine/Canvas.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/StaticMeshSocket.h"
#include "GameFramework/HUD.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ARemnitCharacter

ARemnitCharacter::ARemnitCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 50.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 5;
	

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	
}

bool ARemnitCharacter::GetCanTakeAnyAction() const
{
	return !GetMovementComponent()->IsFalling() && !GetIsRolling() && !GetIsLockedAttacking();
}

bool ARemnitCharacter::GetIsRolling() const
{
	return DodgeRollComponent && DodgeRollComponent->bIsRolling;
}

bool ARemnitCharacter::GetIsLockedAttacking() const
{
	if(const auto WeaponComponent = GetComponentByClass<UWeaponSM>(); WeaponComponent && WeaponComponent->IsLockedAttacking())
	{
		return true;
	}
	return false;
}

void ARemnitCharacter::EquipSword() 
{
	if(RifleComponent)
	{
		RifleComponent->Deactivate();
		RifleComponent->SetVisibility(false);
	}

	if(SwordComponent)
	{
		SwordComponent->Activate();
		SwordComponent->SetVisibility(true);
	}
	CurrentCameraBoomLength = CameraBoomLength_Melee;
	CurrentCameraBoomOffset = CameraBoomOffset_Melee;
}

void ARemnitCharacter::EquipRifle() 
{
	if(SwordComponent)
	{
		SwordComponent->Deactivate();
		SwordComponent->SetVisibility(false);
	}

	if(RifleComponent)
	{
		RifleComponent->Activate();
		RifleComponent->SetVisibility(true);
	}
	// Move camera boom in
	CurrentCameraBoomLength = CameraBoomLength_Rifle;
	CurrentCameraBoomOffset = CameraBoomOffset_Rifle;
}

//void AremnitCharacter::StartIFrames()
//{
//	GEngine->AddOnScreenDebugMessage(2, 1, FColor::Red, TEXT("WHAT"));
//}

void ARemnitCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		//Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// Initalize HUD overlay so this actor can draw on the hud from `PostRenderFor()`
		if (ARemnitHud* Hud = Cast<ARemnitHud>(PlayerController->GetHUD()); Hud)
		{
			Hud->AddPostRenderedActor(this);	
		}
		
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;
	CameraBoom->bUsePawnControlRotation = true; 
	FollowCamera->bUsePawnControlRotation = false;

	EquipSword();
}

void ARemnitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	DodgeRollComponent = GetComponentByClass<UDodgeRollComponent>();
	LockOnComponent = GetComponentByClass<ULockOnComponent>();
	SwordComponent = GetComponentByClass<UWeaponSM>();
	RifleComponent = GetComponentByClass<URifle>();
	RifleMuzzleSocket = RifleComponent->GetStaticMesh()->FindSocket("muzzle");
	if(!RifleMuzzleSocket)
	{
		UE_LOG(LogActor, Error, TEXT("Failed to find Muzzle Socket!"));
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Attacking
		// EnhancedInputComponent->BindAction(SwordMediumAction, ETriggerEvent::Triggered, this, &ARemnitCharacter::SwingSwordMedium);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARemnitCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARemnitCharacter::Look);
		
		// Dodging
		if(DodgeRollComponent)
		{
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, DodgeRollComponent, &UDodgeRollComponent::TryRoll);
		}

		// Lock On
		if(LockOnComponent)
		{
			EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, LockOnComponent, &ULockOnComponent::ToggleLockOn);
		}

		EnhancedInputComponent->BindAction(AttackPrimaryAction, ETriggerEvent::Triggered, this, &ARemnitCharacter::TryAttack);

		CurrentIsAimingInputValue = &EnhancedInputComponent->BindActionValue(AimAction);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

FTransform ARemnitCharacter::GetWeaponMuzzleTransform() const
{
	if(!RifleComponent || !RifleMuzzleSocket)
	{
		return FTransform::Identity;
	}

	return FTransform{RifleComponent->GetSocketLocation("muzzle"), };
	// FTransform Result{};
	// if (const bool bSucceeded = RifleMuzzleSocket->GetSocketTransform(Result, RifleComponent); bSucceeded)
	// {
	// 	const auto FacingDirection = FRotator{0, Controller->GetControlRotation().Yaw , 0};
	// 	Result.SetRotation(FacingDirection.Quaternion());
	// 	return Result;
	// }
	// UE_LOG(LogActor, Error, TEXT("Failed to locate muzzle socket in world space"));
	// return FTransform::Identity;
}

void ARemnitCharacter::Move(const FInputActionValue& Value)
{
	if(!Controller || GetIsRolling())
	{
		return;
	}
	
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// Rotation: Exactly where the camera is pointed. Ignores where the actor is looking.
	const FRotator Rotation = Controller->GetControlRotation();
	//UKismetSystemLibrary::DrawDebugBox(this, GetActorLocation() + FVector{0, 100, 0}, FVector{100}, FLinearColor::Red, Rotation);
	
	// Remove pitch and roll from rotation.
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	// add movement
	AddMovementInput(ForwardDirection * 20, MovementVector.Y);
	AddMovementInput(RightDirection * 20, MovementVector.X);
}

void ARemnitCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const auto LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}



void ARemnitCharacter::Tick(float DeltaSeconds)
{
	if(!CurrentIsAimingInputValue)
	{
		return;
	}

	/**
	 * Handle toggling between aiming gun and using melee
	 */
	if(CurrentIsAimingInputValue->GetValue().Get<bool>() && !bIsAiming)
	{
		TryStartAiming();
	}

	if(!CurrentIsAimingInputValue->GetValue().Get<bool>() && bIsAiming)
	{
		TryStopAiming();
	}

	/**
	 * Gun aiming mode: Pull in camera and lock pawn rotation to camera rotation 
	 */
	if(bIsAiming && RifleComponent)
	{
		// Turn character toward target (Yaw only)
		const auto FacingDirection = FRotator{0, Controller->GetControlRotation().Yaw , 0};
		SetActorRotation(FacingDirection);

		// Angle rifle toward target
		auto const CameraLoc = FollowCamera->GetComponentTransform().GetLocation();
		auto const CameraForward = FollowCamera->GetComponentTransform().GetRotation().GetForwardVector().GetSafeNormal();
		auto const CameraGoal = CameraLoc + CameraForward * 2500;

		// Ignore any possibly applied recoil when finding gun aim rotator; this is where we want to point before recoil is applied.
		const auto CameraPitch = UKismetMathLibrary::FindLookAtRotation(CameraGoal, GetMesh()->GetBoneLocation("weapon_r"));
		GunAimRotator = FRotator{CameraPitch.Pitch, 15, 0};
	}
	// Update reticle size, which will lag behind recoil kickback smoothly.
	CurrentReticleSize = UKismetMathLibrary::FInterpTo(CurrentReticleSize, ReticleSize + ReticleGrowFactor * RecoilTransform.GetLocation().Size(), DeltaSeconds, ReticleGrowSpeed);
	//
	CameraBoom->TargetArmLength = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, CurrentCameraBoomLength, DeltaSeconds, CameraBoomTransitionSpeed);
	CameraBoom->SocketOffset = UKismetMathLibrary::VInterpTo(CameraBoom->SocketOffset, CurrentCameraBoomOffset, DeltaSeconds, CameraBoomTransitionSpeed);
}

void ARemnitCharacter::TryAttack()
{
	if(!bIsAiming && SwordComponent)
	{
		SwordComponent->TryAttack();
	}
	
	if(bIsAiming && RifleComponent)
	{
		RifleComponent->TryFire();
	}
}

void ARemnitCharacter::TryStartAiming()
{
	if(GetIsRolling() || GetIsLockedAttacking())
	{
		return;
	}

	if(RifleComponent)
	{
		EquipRifle();
		bIsAiming = true;
	}	
}

void ARemnitCharacter::TryStopAiming()
{
	EquipSword();
	bIsAiming = false;

}

void ARemnitCharacter::PostRenderFor(APlayerController* PC, UCanvas* Canvas, FVector CameraPosition, FVector CameraDir)
{
	Super::PostRenderFor(PC, Canvas, CameraPosition, CameraDir);

	if(bIsAiming)
	{
		Canvas->K2_DrawLine({Canvas->SizeX / 2. - CurrentReticleSize, Canvas->SizeY / 2.}, {Canvas->SizeX / 2. + CurrentReticleSize, Canvas->SizeY / 2.}, ReticleThickness);
		Canvas->K2_DrawLine({Canvas->SizeX / 2., Canvas->SizeY / 2. - CurrentReticleSize}, {Canvas->SizeX / 2. , Canvas->SizeY / 2. + CurrentReticleSize}, ReticleThickness);
	}
}
