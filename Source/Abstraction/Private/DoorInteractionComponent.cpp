// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UDoorInteractionComponent::UDoorInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDoorInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	StartRotation = GetOwner()->GetActorRotation();
	//FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation;

	CurrentRotationTime = 0;
	OwnerStartingForwardVector = GetOwner()->GetActorRightVector(); // Should be Forward, but the mesh is turned
}


// Called every frame
void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TriggerBox && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (PlayerPawn)
		{
			if (TriggerBox->IsOverlappingActor(PlayerPawn))
			{
				// If we are going from closing the door to opening the door
				// we need to set a new rotation
				if (DidDoorClose == true)
				{
					DidDoorClose = false;
					IsRotationSet = false;
				}

				DidDoorOpen = true;
				
				CurrentRotationTime += DeltaTime;
				if (CurrentRotationTime > TimeToRotate)
				{
					CurrentRotationTime = TimeToRotate;
				}
				else
				{
					if (IsRotationSet == false)
					{
						IsRotationSet = true;
						FVector ActorBLocation = PlayerPawn->GetActorLocation();
						FVector ActorALocation = GetOwner()->GetActorLocation();

						FVector Difference = ActorBLocation - ActorALocation;
						Difference.Normalize();

						float DotProduct = FVector::DotProduct(Difference, OwnerStartingForwardVector);
						if (DotProduct < 0)
						{
							FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation.GetInverse();
						}
						else
						{
							FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation;
						}

					}

					const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
					const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
					const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
					GetOwner()->SetActorRotation(CurrentRotation);
				}
			}
			else
			{
				// If we are going from opening the door to closing the door
				// we need to set a new rotation
				if (DidDoorOpen == true)
				{
					DidDoorOpen = false;
					IsRotationSet = false;
				}

				DidDoorClose = true;

				CurrentRotationTime -= DeltaTime;
				if (CurrentRotationTime < 0)
				{
					CurrentRotationTime = 0;
				}
				else
				{
					if (IsRotationSet == false)
					{
						IsRotationSet = true;
						FVector PlayerLocation = PlayerPawn->GetActorLocation();
						FVector DoorLocation = GetOwner()->GetActorLocation();

						FVector Difference = PlayerLocation - DoorLocation;
						Difference.Normalize();

						float DotProduct = FVector::DotProduct(Difference, OwnerStartingForwardVector);
						FRotator Rotation = FinalRotation;
						if (DotProduct < 0)
						{
							Rotation = FinalRotation.GetInverse();
						}
					}

					const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
					const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
					const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
					GetOwner()->SetActorRotation(CurrentRotation);
				}
			}
		}
	}
}

