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
	FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation;

	CurrentRotationTime = 0;
	OwnerStartingForwardVector = GetOwner()->GetActorForwardVector();
}


// Called every frame
void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, FString::Printf(TEXT("Name: %f"), CurrentRotationTime));

	if (TriggerBox && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (PlayerPawn)
		{
			if (TriggerBox->IsOverlappingActor(PlayerPawn))
			{
				CurrentRotationTime += DeltaTime;
				if (CurrentRotationTime > TimeToRotate)
				{
					CurrentRotationTime = TimeToRotate;
				}
				else
				{
					FVector ActorBLocation = PlayerPawn->GetActorLocation();
					FVector ActorALocation = GetOwner()->GetActorLocation();

					FVector Difference = ActorALocation - ActorBLocation;
					Difference.Normalize();

					float DotProduct = FVector::DotProduct(Difference, PlayerPawn->GetActorForwardVector());
					GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, FString::Printf(TEXT("DotProduct: %f"), DotProduct));
					FRotator Rotation = FinalRotation;
					if (DotProduct < 0)
					{
						Rotation = FinalRotation.GetInverse();
					}


					const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
					const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
					const FRotator CurrentRotation = FMath::Lerp(StartRotation, Rotation, RotationAlpha);
					GetOwner()->SetActorRotation(CurrentRotation);
					//GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, FString::Printf(TEXT("Open Front: %f"), CurrentRotationTime));
				}
			}
			else
			{
				CurrentRotationTime -= DeltaTime;
				if (CurrentRotationTime < 0)
				{
					CurrentRotationTime = 0;
				}
				else
				{

					FVector ActorBLocation = PlayerPawn->GetActorLocation();
					FVector ActorALocation = GetOwner()->GetActorLocation();

					FVector Difference = ActorALocation - ActorBLocation;
					Difference.Normalize();

					float DotProduct = FVector::DotProduct(Difference, PlayerPawn->GetActorForwardVector());
					GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, FString::Printf(TEXT("DotProduct: %f"), DotProduct));
					FRotator Rotation = FinalRotation;
					if (DotProduct < 0)
					{
						Rotation = FinalRotation.GetInverse();
					}

					const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
					const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
					const FRotator CurrentRotation = FMath::Lerp(StartRotation, Rotation, RotationAlpha);
					GetOwner()->SetActorRotation(CurrentRotation);
					//GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, FString::Printf(TEXT("Close Front: %f"), CurrentRotationTime));
				}
			}

		}
	}
}

