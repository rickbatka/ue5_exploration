// Copyright Epic Games, Inc. All Rights Reserved.

#include "remnitCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

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
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

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

//void AremnitCharacter::StartIFrames()
//{
//	GEngine->AddOnScreenDebugMessage(2, 1, FColor::Red, TEXT("WHAT"));
//}

void ARemnitCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


void ARemnitCharacter::SwingSwordMedium()
{
	bShouldSwingSwordMedium = true;
	if (!GetCharacterMovement()->IsFalling())
	{
		PlayAnimMontage(SwordAttackAMontage);
	}
}

void ARemnitCharacter::TryRoll()
{
	if (bIsRolling || GetCharacterMovement()->IsFalling())
	{
		return;
	}

	// Get current movement vector relative to player
	// Set movement speed in that vector 
	// Play appropriate roll animation (direction)
	// during roll: update movement speed to remain constant every Tick
	// set IFrames
	// after roll: stop setting movement vector
	// 

	if (const auto Duration = PlayAnimMontage(RollForwardMontage, 1.25); Duration == 0.)
	{
		UE_LOG(LogActor, Error, TEXT("Failed to execute roll action; bailing"));
		bIsRolling = false;
		return;
	}

	VelocityBeforeRoll = GetCharacterMovement()->GetLastUpdateVelocity();
	RollDirection = GetCharacterMovement()->GetLastUpdateVelocity();
	// Roll forward if character was standing still
	if (RollDirection.IsZero())
	{
		RollDirection = GetOwner()->GetActorForwardVector();
	}
	RollDirection.Normalize();
	bIsRolling = true;
}

void ARemnitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Attacking
		EnhancedInputComponent->BindAction(SwordMediumAction, ETriggerEvent::Triggered, this, &ARemnitCharacter::SwingSwordMedium);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARemnitCharacter::Move);

		// Dodging
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &ARemnitCharacter::TryRoll);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARemnitCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void ARemnitCharacter::OnNotifyBegin(const FName NotifyName)
{
	if (NotifyName == "ANS_AnimRunning")
	{
		
	}
	else if (NotifyName == "ANS_IFramesAnimNotifyState")
	{
		
	}
}

void ARemnitCharacter::OnNotifyEnd(const FName NotifyName)
{
	if (NotifyName == "ANS_AnimRunning" && bIsRolling)
	{
		bIsRolling = false;
		
		if(GetMovementComponent()->GetLastInputVector().IsZero())
		{
			// Stop as soon as possible if no movement input - so the character doesn't "glide" to a stop after a roll.
			GetMovementComponent()->ConsumeInputVector();
		}else
		{
			// Hack, exceed the speed limit to ensure they are running after roll, it feels nicer
			GetMovementComponent()->Velocity = VelocityBeforeRoll + 20; 
			GetMovementComponent()->UpdateComponentVelocity();
		}
		
	}
	else if (NotifyName == "ANS_IFramesAnimNotifyState")
	{
		UE_LOG(LogActor, Log, TEXT("IFrames ended"));
	}
}

void ARemnitCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
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
	if (bIsRolling)
	{
		SetActorLocation(GetActorLocation() + RollDirection * RollSpeed, true);
		lookat
	}
}

// void ARemnitCharacter::AnimNotify_StartIFrames()
// {
// 	// Do nothing, for now using AnimNotifyState instead
// }
