// Fill out your copyright notice in the Description page of Project Settings.


#include "VRMotionController.h"
#include "PickupActorInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AVRMotionController::AVRMotionController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Scene
    Scene = CreateDefaultSubobject<USceneComponent>("Scene");
    Scene->SetupAttachment(GetRootComponent());

    // MotionController
    MotionController = CreateDefaultSubobject<UMotionControllerComponent>("MotionController");
    MotionController->SetupAttachment(Scene);

    // HandSkeletalMesh
    HandSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("HandSkeletalMesh");
    HandSkeletalMesh->SetupAttachment(MotionController);

    // Triggers
    GrabSphereTrigger = CreateDefaultSubobject<USphereComponent>("GrabSphereTrigger");
    GrabSphereTrigger->SetupAttachment(HandSkeletalMesh);

    ThumbTrigger3 = CreateDefaultSubobject<UCapsuleComponent>("ThumbTrigger3");
    ThumbTrigger3->SetupAttachment(HandSkeletalMesh);

    IndexTrigger3 = CreateDefaultSubobject<UCapsuleComponent>("IndexTrigger3");
    IndexTrigger3->SetupAttachment(HandSkeletalMesh);

    MiddleTrigger3 = CreateDefaultSubobject<UCapsuleComponent>("MiddleTrigger3");
    MiddleTrigger3->SetupAttachment(HandSkeletalMesh);

    RingTrigger3 = CreateDefaultSubobject<UCapsuleComponent>("RingTrigger3");
    RingTrigger3->SetupAttachment(HandSkeletalMesh);

    PinkyTrigger3 = CreateDefaultSubobject<UCapsuleComponent>("PinkyTrigger3");
    PinkyTrigger3->SetupAttachment(HandSkeletalMesh);

    ThumbTrigger2 = CreateDefaultSubobject<UCapsuleComponent>("ThumbTrigger2");
    ThumbTrigger2->SetupAttachment(HandSkeletalMesh);

    IndexTrigger2 = CreateDefaultSubobject<UCapsuleComponent>("IndexTrigger2");
    IndexTrigger2->SetupAttachment(HandSkeletalMesh);

    MiddleTrigger2 = CreateDefaultSubobject<UCapsuleComponent>("MiddleTrigger2");
    MiddleTrigger2->SetupAttachment(HandSkeletalMesh);

    RingTrigger2 = CreateDefaultSubobject<UCapsuleComponent>("RingTrigger2");
    RingTrigger2->SetupAttachment(HandSkeletalMesh);

    PinkyTrigger2 = CreateDefaultSubobject<UCapsuleComponent>("PinkyTrigger2");
    PinkyTrigger2->SetupAttachment(HandSkeletalMesh);

    InitFingerFlex();
    InitFingerTriggersBlocked();
    InitFingerTriggersOverlap();
}

// Called when the game starts or when spawned
void AVRMotionController::BeginPlay()
{
	Super::BeginPlay();

    VRCharacter = Cast<AVRCharacterBase>(GetOwner());
}

// Called every frame
void AVRMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


    if (bTracingForTeleportation)
    {
        TraceForTeleportLocation();
    }
}

//========================== Grasping Logic ==========================

// Returns the nearest mesh that's overlapping the hand
AActor* AVRMotionController::ObjectSelection()
{
    TArray< AActor* > overlappingActors;
    GrabSphereTrigger->GetOverlappingActors(overlappingActors);

    AActor* nearestOverlappingActor = NULL;
    float nearestOverlap = 10000.0f;

    for (AActor* actor : overlappingActors)
    {
        if (actor->Implements<UPickupActorInterface>())
        {
            FVector distance = GrabSphereTrigger->GetComponentLocation() - actor->GetActorLocation();

            if (distance.Size() < nearestOverlap)
            {
                nearestOverlap = distance.Size();
                nearestOverlappingActor = actor;
            }
        }
    }

    return nearestOverlappingActor; // Nearest Mesh
}

// Manages the capsule triggers attached to the fingertips
void AVRMotionController::InteractionManager(float GrabControllerInput)
{
    for (TPair<EHandTriggers, bool>& triggerBlocked : FingerTriggersBlocked)
    {
        EHandTriggers capsule = triggerBlocked.Key;

        if (triggerBlocked.Value)
        {
            if (GrabControllerInput < FingerFlex[capsule] || !FingerTriggersOverlap[capsule])
            {
                triggerBlocked.Value = false;
            }
        }
        else
        {
            FingerMovement(capsule, GrabControllerInput);
        }
    }
}

/*
* Update the deflection value of a finger based on the player's gripping action 
* through the triggers and the speed of finger deflection.
*/
void AVRMotionController::FingerMovement(EHandTriggers Finger, float GrabFlex, float speed)
{
    if (GrabFlex > 0.5)
    {
        const float deltaTimeSeconds = GetWorld()->GetDeltaSeconds();

        float delta = GrabFlex - FingerFlex[Finger];

        if (abs(delta) > 0.05 && delta > 0)
        {
            FingerFlex[Finger] += deltaTimeSeconds * speed / GrabFlex;
        }
    }
    else
    {
        FingerFlex[Finger] = GrabFlex;
    }
}

bool AVRMotionController::GraspingLogic(float HandFlex)
{
    /*(HandFlex > 0.1);

    FingerTriggersBlocked[Thumb_3] || FingerTriggersBlocked[Thumb_2];
    FingerTriggersBlocked[Index_3] || FingerTriggersBlocked[Index_2];
    FingerTriggersBlocked[Middle_3] || FingerTriggersBlocked[Middle_2];*/

    return true;
}

void AVRMotionController::EvaluateGrabbing()
{
    FVector startPoint, endPoint;
    const TArray<AActor*> actorsToIgnore = {GetOwner()};
    const TArray< TEnumAsByte<EObjectTypeQuery> > objectTypes = {
        UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
        UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
        UEngineTypes::ConvertToObjectType(ECC_PhysicsBody),
        UEngineTypes::ConvertToObjectType(ECC_Vehicle)
    };
    FHitResult hitResult;
    FLinearColor traceColor = FLinearColor(1.0f, 0.0f, 0.0f);
    FLinearColor hitColor = FLinearColor(0.0f, 1.0f, 0.0f);

    TMap<EHandTriggers, FVector> GrabEvalImpactPoints;
    TMap<EHandTriggers, float> GrabEvalErrors;

    for (TPair<EHandTriggers, UCapsuleComponent*>& fingerTrigger : FingerTriggers)
    {
        EHandTriggers trigger = fingerTrigger.Key;
        UCapsuleComponent*& capsule = fingerTrigger.Value;

        startPoint = capsule->GetComponentLocation() + -7.0f * capsule->GetRightVector();
        endPoint = capsule->GetComponentLocation() + 5.0f * capsule->GetRightVector();

        // More info on : https://www.youtube.com/watch?v=6n25OTWHxSE
        const bool collide = UKismetSystemLibrary::SphereTraceSingleForObjects(
            GetWorld(), 
            startPoint, 
            endPoint, 
            1.2f, // Radius
            objectTypes,
            true, // test complex collision
            actorsToIgnore,
            EDrawDebugTrace::ForDuration,
            hitResult,
            true, // Ignore self
            traceColor,
            hitColor,
            5.0f // Draw time
        );

        GrabEvalImpactPoints.Add(trigger, hitResult.ImpactPoint);

        if (collide)
        {

            if (AttachedActor == hitResult.GetActor())
            {
                FVector impactPoint = hitResult.ImpactPoint;

                FVector Dip = impactPoint - startPoint;
                FVector Dctc = capsule->GetComponentLocation() + 1.0 * capsule->GetRightVector();

                double nearestDistance = (FVector::DotProduct(Dip, Dctc) / Dctc.Length()) - Dctc.Length();

                GrabEvalErrors[trigger] = (float) nearestDistance;
            }
            else
            {
                GrabEvalErrors[trigger] = 0.0f;
            }
        }
    }
}

void AVRMotionController::GrabActor()
{
    bWantToGrip = true;

    //AActor* nearestActor = ObjectSelection();

    //if (nearestActor != NULL)
    //{
    //    AttachedActor = nearestActor;

    //    IPickupActorInterface* pickableActor = Cast<IPickupActorInterface>(nearestActor);

    //    if (pickableActor != NULL)
    //    {
    //        pickableActor->Pickup(MotionController);
    //        // Rumble controller : {Intensity: 0.7}
    //    }
    //}

}

void AVRMotionController::ReleaseActor()
{
    bWantToGrip = false;

    //if (AttachedActor != NULL && AttachedActor->Implements<UPickupActorInterface>())
    //{
    //    auto attachedParent = AttachedActor->GetRootComponent()->GetAttachParent();

    //    // Make sure that the hand still holding the object
    //    if (MotionController == attachedParent)
    //    {
    //        IPickupActorInterface* attachedActor = Cast<IPickupActorInterface>(AttachedActor);

    //        attachedActor->Drop();
    //        // Rumble controller : {Intensity: 0.2}
    //    }

    //    AttachedActor = NULL;
    //}
}

// Put all capsule fingers into a Map
void AVRMotionController::SetFingerTriggers()
{
    FingerTriggers.Add(EHandTriggers::Thumb_3, ThumbTrigger3);
    FingerTriggers.Add(EHandTriggers::Index_3, IndexTrigger3);
    FingerTriggers.Add(EHandTriggers::Middle_3, MiddleTrigger3);
    FingerTriggers.Add(EHandTriggers::Ring_3, RingTrigger3);
    FingerTriggers.Add(EHandTriggers::Pinky_3, PinkyTrigger3);
    FingerTriggers.Add(EHandTriggers::Thumb_2, ThumbTrigger2);
    FingerTriggers.Add(EHandTriggers::Index_2, IndexTrigger2);
    FingerTriggers.Add(EHandTriggers::Middle_2, MiddleTrigger2);
    FingerTriggers.Add(EHandTriggers::Ring_2, RingTrigger2);
    FingerTriggers.Add(EHandTriggers::Pinky_2, PinkyTrigger2);
}

// Sets the finger flexing value to zero
void AVRMotionController::InitFingerFlex()
{
    FingerFlex.Add(EHandTriggers::Thumb_3, 0.0f);
    FingerFlex.Add(EHandTriggers::Index_3, 0.0f);
    FingerFlex.Add(EHandTriggers::Middle_3, 0.0f);
    FingerFlex.Add(EHandTriggers::Ring_3, 0.0f);
    FingerFlex.Add(EHandTriggers::Pinky_3, 0.0f);
    FingerFlex.Add(EHandTriggers::Thumb_2, 0.0f);
    FingerFlex.Add(EHandTriggers::Index_2, 0.0f);
    FingerFlex.Add(EHandTriggers::Middle_2, 0.0f);
    FingerFlex.Add(EHandTriggers::Ring_2, 0.0f);
    FingerFlex.Add(EHandTriggers::Pinky_2, 0.0f);
}

//
void AVRMotionController::InitFingerTriggersBlocked()
{
    FingerTriggersBlocked.Add(EHandTriggers::Thumb_3, false);
    FingerTriggersBlocked.Add(EHandTriggers::Index_3, false);
    FingerTriggersBlocked.Add(EHandTriggers::Middle_3, false);
    FingerTriggersBlocked.Add(EHandTriggers::Ring_3, false);
    FingerTriggersBlocked.Add(EHandTriggers::Pinky_3, false);
    FingerTriggersBlocked.Add(EHandTriggers::Thumb_2, false);
    FingerTriggersBlocked.Add(EHandTriggers::Index_2, false);
    FingerTriggersBlocked.Add(EHandTriggers::Middle_2, false);
    FingerTriggersBlocked.Add(EHandTriggers::Ring_2, false);
    FingerTriggersBlocked.Add(EHandTriggers::Pinky_2, false);
}

void AVRMotionController::InitFingerTriggersOverlap()
{
    FingerTriggersOverlap.Add(EHandTriggers::Thumb_3, false);
    FingerTriggersOverlap.Add(EHandTriggers::Index_3, false);
    FingerTriggersOverlap.Add(EHandTriggers::Middle_3, false);
    FingerTriggersOverlap.Add(EHandTriggers::Ring_3, false);
    FingerTriggersOverlap.Add(EHandTriggers::Pinky_3, false);
    FingerTriggersOverlap.Add(EHandTriggers::Thumb_2, false);
    FingerTriggersOverlap.Add(EHandTriggers::Index_2, false);
    FingerTriggersOverlap.Add(EHandTriggers::Middle_2, false);
    FingerTriggersOverlap.Add(EHandTriggers::Ring_2, false);
    FingerTriggersOverlap.Add(EHandTriggers::Pinky_2, false);
}

//========================== Teleportation Logic ==========================

void AVRMotionController::BeginTeleport()
{
    // Start tracing for the teleport location
    bTracingForTeleportation = true;
}

void AVRMotionController::EndTeleport()
{
    // Start tracing for the teleport location
    bTracingForTeleportation = false;
    VRCharacter->TeleportLocationIndicator->SetHiddenInGame(true);
}


void AVRMotionController::TraceForTeleportLocation()
{
    // Active trace visualization (Niagara System)
    FPredictProjectilePathParams predictParams;

    predictParams.bTraceComplex = false;
    predictParams.bTraceWithChannel = true;
    predictParams.bTraceWithCollision = true;
    predictParams.TraceChannel = ECC_Visibility;
    predictParams.StartLocation = MotionController->GetComponentLocation();
    predictParams.LaunchVelocity = TeleportTraceProjectileVelocity * MotionController->GetForwardVector();

    FPredictProjectilePathResult predictResult;


    if (UGameplayStatics::PredictProjectilePath(this, predictParams, predictResult))
    {
        VRCharacter->TeleportLocationIndicator->SetWorldLocation(predictResult.HitResult.Location);
        VRCharacter->TeleportLocationIndicator->SetHiddenInGame(false);
    }
    else
    {
        VRCharacter->TeleportLocationIndicator->SetHiddenInGame(true);
    }
}
