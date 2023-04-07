// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CineCameraComponent.h"
#include "VRCharacterBase.generated.h"

UCLASS()
class VRCHARACTER_API AVRCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* VROffset;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        //UCineCameraComponent* VRCamera;
        UCameraComponent* VRCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* HandControllerBase;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* TeleportLocationIndicator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float MaximumSpeed = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float MaximumFocusTrackingDistance = 10000.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void CharacterMoveForward(float ratio);

	UFUNCTION(BlueprintCallable)
		void FindFocusDistance();

public:
	// Sets default values for this character's properties
	AVRCharacterBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
