// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"
#include "ObjectiveWorldSubsystem.h"
#include "DrawDebugHelpers.h"
#include "Components/AudioComponent.h"
#include "Components/TextRenderComponent.h"
#include "AbstractionPlayerCharacter.h"

constexpr float FLT_METERS(float meters) { return meters * 100.0f; }

// Create a console variable
static TAutoConsoleVariable<bool> CVarToggleDebugDoor(
	TEXT("Abstraction.DoorInteractionComponent.Debug"),
	false,
	TEXT("Toggle DoorInteractionComponent debug display"),
	ECVF_Default);

// Sets default values for this component's properties
UDoorInteractionComponent::UDoorInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DoorState = EDoorState::DS_Closed;

	CVarToggleDebugDoor.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&UDoorInteractionComponent::OnDebugToggled)); 
}

// Called every frame
void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*if (DoorState == EDoorState::DS_Closed)
	{
		if (TriggerBox && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
		{
			APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
			if (PlayerPawn)
			{
				if (TriggerBox->IsOverlappingActor(PlayerPawn))
				{
					DoorState = EDoorState::DS_Opening;
					CurrentRotationTime = 0;
				}
			}
		}
	}
	else 
	*/	
	if (DoorState == EDoorState::DS_Opening)
	{
		CurrentRotationTime += DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
		GetOwner()->SetActorRotation(CurrentRotation);
		if (TimeRatio >= 1.0f)
		{
			OnDoorOpen();
		}
	}

	//if (TriggerBox && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	//{
	//	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	//	if (PlayerPawn)
	//	{
	//		if (TriggerBox->IsOverlappingActor(PlayerPawn))
	//		{
	//			// If we are going from closing the door to opening the door
	//			// we need to set a new rotation
	//			if (DidDoorClose == true)
	//			{
	//				DidDoorClose = false;
	//				IsRotationSet = false;
	//			}

	//			DidDoorOpen = true;
	//			
	//			CurrentRotationTime += DeltaTime;
	//			if (CurrentRotationTime > TimeToRotate)
	//			{
	//				CurrentRotationTime = TimeToRotate;
	//			}
	//			else
	//			{
	//				if (IsRotationSet == false)
	//				{
	//					IsRotationSet = true;
	//					FVector PlayerLocation = PlayerPawn->GetActorLocation();
	//					FVector DoorLocation = GetOwner()->GetActorLocation();

	//					FVector Difference = PlayerLocation - DoorLocation;
	//					Difference.Normalize();

	//					float DotProduct = FVector::DotProduct(Difference, OwnerStartingForwardVector);
	//					if (DotProduct < 0)
	//					{	// Player is in front
	//						FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation.GetInverse();
	//					}
	//					else
	//					{	// player is in the back
	//						FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation;
	//					}
	//				}

	//				const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
	//				const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
	//				const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
	//				GetOwner()->SetActorRotation(CurrentRotation);
	//			}
	//		}
	//		else
	//		{
	//			DidDoorClose = true;

	//			CurrentRotationTime -= DeltaTime;
	//			if (CurrentRotationTime < 0)
	//			{
	//				CurrentRotationTime = 0;
	//			}
	//			else
	//			{
	//				const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
	//				const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
	//				const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
	//				GetOwner()->SetActorRotation(CurrentRotation);
	//			}
	//		}
	//	}
	//}

	DebugDraw();
}


void UDoorInteractionComponent::OpenDoor()
{
	if (IsOpen() || DoorState == EDoorState::DS_Opening)
	{
		return;
	}

	if (AudioComponent)
	{
		AudioComponent->Play();
	}


	DoorState = EDoorState::DS_Opening;
	CurrentRotationTime = 0.0f;
}


// Called when the game starts
void UDoorInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	StartRotation = GetOwner()->GetActorRotation();
	FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation;

	CurrentRotationTime = 0;

	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	//check(AudioComponent);
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDoorInteractionComponent::BeginPlay() Missing Audio Component"));
	}

	TextRenderComponent = GetOwner()->FindComponentByClass<UTextRenderComponent>();
}


void UDoorInteractionComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("UDoorInteractionComponent::OnOverlapBegin"));
	//we already have somebody interacting, currently we don't support multiple interactions
	if (InteractingActor)
	{
		return;
	}

	//for now we will get that component and set visible
	if (OtherActor->ActorHasTag("Player"))
	{
		InteractingActor = OtherActor;
		if (TextRenderComponent)
		{
			TextRenderComponent->SetText(InteractionPrompt);
			TextRenderComponent->SetVisibility(true);
		}
	}
}

void UDoorInteractionComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("UDoorInteractionComponent::OnOverlapEnd"));
	if (OtherActor == InteractingActor)
	{
		InteractingActor = nullptr;
		if (TextRenderComponent)
		{
			TextRenderComponent->SetVisibility(false);
		}
	}
}



void UDoorInteractionComponent::InteractionRequested()
{
	//ideally we would make sure this is allowed
	if (InteractingActor)
	{
		SetActive(false);
		if (TextRenderComponent)
		{
			TextRenderComponent->SetText(InteractionPrompt);
			TextRenderComponent->SetVisibility(false);
		}

		AAbstractionPlayerCharacter* APC = Cast<AAbstractionPlayerCharacter>(InteractingActor);
		if (APC)
		{
			APC->DoorOpenInteractionStarted(GetOwner());
		}

		//this will be called from the owner to be in sync with animation
		//OpenDoor();
	}
}


void UDoorInteractionComponent::OnDoorOpen()
{
	DoorState = EDoorState::DS_Open;

	UObjectiveComponent* ObjectiveComponent = GetOwner()->FindComponentByClass<UObjectiveComponent>();
	if (ObjectiveComponent)
	{
		ObjectiveComponent->SetState(EObjectiveState::OS_Completed);
	}

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("DoorOpened"));
	InteractionSuccess.Broadcast();
}

void UDoorInteractionComponent::OnDebugToggled(IConsoleVariable* Var)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDebugToggle"));
}

void UDoorInteractionComponent::DebugDraw()
{
	if (CVarToggleDebugDoor->GetBool())
	{
		FVector Offset(FLT_METERS(-0.75f), 0.0f, FLT_METERS(2.5f));
		FVector StartLocation = GetOwner()->GetActorLocation() + Offset;
		FString EnumAsString = TEXT("Door State: ") + UEnum::GetDisplayValueAsText(DoorState).ToString();
		DrawDebugString(GetWorld(), Offset, EnumAsString, GetOwner(), FColor::Blue, 0.0f);
	}
}