// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbstractionPlayerCharacter.generated.h"

class UDamageHandlerComponent;
class UHealthComponent;
class UParticleSystemComponent;

DECLARE_MULTICAST_DELEGATE(FInteractionStart);
DECLARE_MULTICAST_DELEGATE(FInteractionCancel);

UCLASS()
class ABSTRACTION_API AAbstractionPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAbstractionPlayerCharacter();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when the actor falls out of the world 'safety'
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	const bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	const float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Abstraction")
	void SetOnFire(float BaseDamage, float DamageTotalTime, float TakeDamageInterval);

	UFUNCTION(BlueprintCallable)
	void HandleItemCollected();

	UFUNCTION(BlueprintImplementableEvent)
	void ItemCollected();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ItemsCollected = 0;


	FInteractionStart OnInteractionStart;
	FInteractionCancel OnInteractionCancel;

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* ParticleSystemComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnDeath(bool IsFellOut);

	UFUNCTION()
	void OnDeathTimerFinished();

	//Input Bindings
	void StartInteraction();
	void StopInteraction();

	UPROPERTY(EditAnywhere)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere)
	UDamageHandlerComponent* DamageHandlerComponent;

	UPROPERTY(EditAnywhere)
	float TimeRestartAfterDeath = 2.0f;

	FTimerHandle RestartLevelTimerHandle;

	APlayerController* PC;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<UCameraShakeBase> CamShake;

	// Force Feedback values
	UPROPERTY(EditAnywhere, Category="ForceFeedback")
	float ForceFeedbackIntensity = 1.0f;
	UPROPERTY(EditAnywhere, Category = "ForceFeedback")
	float ForceFeedbackDuration = 1.0f;
};
