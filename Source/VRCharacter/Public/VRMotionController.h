// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotionControllerComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "VRMotionController.generated.h"

UENUM(BlueprintType)
enum class EHandTriggers : uint8
{
    Thumb_3         UMETA(DisplayName = "Thumb_3"),
    Index_3         UMETA(DisplayName = "Index_3"),
    Middle_3        UMETA(DisplayName = "Middle_3"),
    Ring_3          UMETA(DisplayName = "Ring_3"),
    Pinky_3         UMETA(DisplayName = "Pinky_3"),

    Thumb_2         UMETA(DisplayName = "Thumb_2"),
    Index_2         UMETA(DisplayName = "Index_2"),
    Middle_2        UMETA(DisplayName = "Middle_2"),
    Ring_2          UMETA(DisplayName = "Ring_2"),
    Pinky_2         UMETA(DisplayName = "Pinky_2")
};

UENUM(BlueprintType)
enum class EGripState : uint8
{
    Open            UMETA(DisplayName = "Open"),
    CanGrab         UMETA(DisplayName = "CanGrab"),
    Grab            UMETA(DisplayName = "Grab")
};

UCLASS()
class VRCHARACTER_API AVRMotionController : public AActor
{
	GENERATED_BODY()
	
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        USceneComponent* Scene; // Root component

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        UMotionControllerComponent* MotionController;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        USkeletalMeshComponent* HandSkeletalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        USphereComponent* GrabSphereTrigger;

    //========================== Finger Triggers ==========================

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Triggers")
        UCapsuleComponent* ThumbTrigger3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Triggers")
        UCapsuleComponent* IndexTrigger3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Triggers")
        UCapsuleComponent* MiddleTrigger3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Triggers")
        UCapsuleComponent* RingTrigger3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Triggers")
        UCapsuleComponent* PinkyTrigger3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Triggers")
        UCapsuleComponent* ThumbTrigger2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Triggers")
        UCapsuleComponent* IndexTrigger2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Triggers")
        UCapsuleComponent* MiddleTrigger2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Triggers")
        UCapsuleComponent* RingTrigger2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Triggers")
        UCapsuleComponent* PinkyTrigger2;

    //========================== Grab Management Variables ==========================

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables|Grabbing")
        bool bWantToGrip;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Grabbing")
        bool bObjectCanBeGrabbed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables|Grabbing")
        bool bObjectGrabbed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Grabbing")
        EGripState GripState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Grabbing")
        AActor* AttachedActor = NULL;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables|Grabbing")
        TMap<EHandTriggers, float> FingerFlex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Grabbing")
        TMap<EHandTriggers, bool> FingerTriggersBlocked;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Grabbing")
        TMap<EHandTriggers, bool> FingerTriggersOverlap;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variables|Grabbing")
        TMap<EHandTriggers, UCapsuleComponent*> FingerTriggers;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Gabbing")
        AActor* ObjectSelection();
    
    UFUNCTION(BlueprintCallable, Category = "Grabbing")
        void InteractionManager(float GrabControllerInput);

    UFUNCTION(BlueprintCallable, Category = "Grabbing")
        void FingerMovement(EHandTriggers Finger, float GrabFlex, float speed = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Grabbing")
        bool GraspingLogic(float HandFlex);

    UFUNCTION(BlueprintCallable, Category = "Grabbing|Utils")
        void EvaluateGrabbing();

    UFUNCTION(BlueprintCallable, Category = "Grabbing|Utils")
        void SetFingerTriggers();

    UFUNCTION(BlueprintCallable, Category = "Grabbing")
        void GrabActor();

    UFUNCTION(BlueprintCallable, Category = "Grabbing")
        void ReleaseActor();

private:
    void InitFingerFlex();

    void InitFingerTriggersBlocked();

    void InitFingerTriggersOverlap();

public:	
    // Sets default values for this actor's properties
    AVRMotionController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
