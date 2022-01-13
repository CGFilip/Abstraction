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
					const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
					const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
					const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
					GetOwner()->SetActorRotation(CurrentRotation);
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
					const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
					const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
					const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
					GetOwner()->SetActorRotation(CurrentRotation);
				}
			}
			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, FString::Printf(TEXT("CurrentRotationTime: %f"), CurrentRotationTime));

		}
	}
}

