// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABSTRACTION_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	void TakeDamage(const float Damage) { CurrentHealth -= Damage; }
	bool IsDead() const { return CurrentHealth <= FLT_EPSILON; }
	float GetCurrentHealth() const { return CurrentHealth; }
	void SetCurrentHealth(float Health) { CurrentHealth = Health; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.0f;
	
	float CurrentHealth = 0.0f;
};
