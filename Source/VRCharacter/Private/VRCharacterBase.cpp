// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacterBase.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AVRCharacterBase::AVRCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// VROffset
	VROffset = CreateDefaultSubobject<USceneComponent>("VROffset");
	VROffset->SetupAttachment(GetCapsuleComponent());
	VROffset->SetRelativeLocation(FVector(0, 0, -1 * GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	// VRCamera
	VRCamera = CreateDefaultSubobject<UCineCameraComponent>("VRCamera");
	VRCamera->SetupAttachment(VROffset);

	// HandControllerBase
	HandControllerBase = CreateDefaultSubobject<USceneComponent>("HandControllerBase");
	HandControllerBase->SetupAttachment(VROffset);

	// LeftHandController
	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>("LeftHandController");
	LeftHandController->SetupAttachment(HandControllerBase);
	LeftHandController->SetTrackingMotionSource(FName("Left"));

	// RightHandController
	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>("RightHandController");
	RightHandController->SetupAttachment(HandControllerBase);
	RightHandController->SetTrackingMotionSource(FName("Right"));

}

// Called when the game starts or when spawned
void AVRCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

/**
* Allows the VRCharacter to move in a specific direction using the 
* player's VR camera. The method takes a "ratio" parameter that 
* determines the speed of movement.
*/
void AVRCharacterBase::CharacterMoveForward(float ratio)
{
	const FVector cameraDirection = FVector(VRCamera->GetForwardVector().X, VRCamera->GetForwardVector().Y, 0);
	const FVector cameraProjectedForward = cameraDirection.GetSafeNormal();

	AddActorWorldOffset(cameraProjectedForward * ratio * MaximumSpeed);
}

/**
* Is used to determine the focus distance for the player's VR camera.
* The method uses a beam to detect an object in the direction the 
* camera is facing and adjusts the focus distance accordingly.
*/
void AVRCharacterBase::FindFocusDistance()
{
	FVector start = VRCamera->GetComponentLocation();
	FVector end = start + VRCamera->GetForwardVector() * MaximumFocusTrackingDistance;
	FCameraFocusSettings focusSettings;
	FHitResult hitResult;

	focusSettings.ManualFocusDistance = MaximumFocusTrackingDistance;

	if (!GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility))
		return;

	focusSettings.ManualFocusDistance = hitResult.Distance;
	VRCamera->SetFocusSettings(focusSettings);
}

// Called every frame
void AVRCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

