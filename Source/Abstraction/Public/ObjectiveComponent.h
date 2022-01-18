// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectiveComponent.generated.h"


UENUM()
enum class EObjectiveState
{
	OS_Inactive = 0		UMETA(DisplayName = "Inactive"),
	OS_Active = 1		UMETA(DisplayName = "Active"),
	OS_Completed = 2	UMETA(DisplayName = "Completed"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABSTRACTION_API UObjectiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UObjectiveComponent();

	UFUNCTION(BlueprintCallable)
	const FString& GetDescription() const { return Description; }

	DECLARE_EVENT_TwoParams(FObjectiveComponent, FStateChanged, UObjectiveComponent*, EObjectiveState)
	FStateChanged& OnStateChanged() { return StateChangedEvent; }

	UFUNCTION(BlueprintCallable)
	EObjectiveState GetState() const { return State; }

	void SetState(EObjectiveState NewState);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitializeComponent() override;

	UPROPERTY(EditAnywhere)
	FString Description;

	UPROPERTY(EditAnywhere)
	EObjectiveState State;

	FStateChanged StateChangedEvent;
		
};
