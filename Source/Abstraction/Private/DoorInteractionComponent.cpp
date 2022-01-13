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
}


// Called every frame
void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, FString::Printf(TEXT("CurrentRotationTime: %f"), CurrentRotationTime));

	if (FrontTriggerBox && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (PlayerPawn)
		{
			if (FrontTriggerBox->IsOverlappingActor(PlayerPawn))
			{
				DidOpenFromFront = true;
				DidOpenFromBack = false;
				CurrentRotationTime += DeltaTime;
				if (CurrentRotationTime > TimeToRotate)
				{
					CurrentRotationTime = TimeToRotate;
				}
				else
				{
					const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
					const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
					const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
					GetOwner()->SetActorRotation(CurrentRotation);
					GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, FString::Printf(TEXT("Open Front: %f"), CurrentRotationTime));
				}
			}
			else if(DidOpenFromFront)
			{
				CurrentRotationTime -= DeltaTime;
				if (CurrentRotationTime < 0)
				{
					CurrentRotationTime = 0;
				}
				else
				{
					const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
					const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
					const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
					GetOwner()->SetActorRotation(CurrentRotation);
					GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, FString::Printf(TEXT("Close Front: %f"), CurrentRotationTime));
				}
			}

		}
	}


	if (BackTriggerBox && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (PlayerPawn)
		{
			if (BackTriggerBox->IsOverlappingActor(PlayerPawn))
			{
				DidOpenFromBack = true;
				DidOpenFromFront = false;
				CurrentRotationTime += DeltaTime;
				if (CurrentRotationTime > TimeToRotate)
				{
					CurrentRotationTime = TimeToRotate;
				}
				else
				{
					const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
					const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
					const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation.GetInverse(), RotationAlpha);
					GetOwner()->SetActorRotation(CurrentRotation);
					GEngine->AddOnScreenDebugMessage(3, 1, FColor::Green, FString::Printf(TEXT("Open Back: %f"), CurrentRotationTime));
				}
			}
			else if(DidOpenFromBack)
			{
				CurrentRotationTime -= DeltaTime;
				if (CurrentRotationTime < 0)
				{
					CurrentRotationTime = 0;
				}
				else
				{
					const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
					const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
					const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation.GetInverse(), RotationAlpha);
					GetOwner()->SetActorRotation(CurrentRotation);
					GEngine->AddOnScreenDebugMessage(4, 1, FColor::Green, FString::Printf(TEXT("Close Back: %f"), CurrentRotationTime));
				}
			}

		}
	}
}

