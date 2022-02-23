// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageHandlerComponent.h"
#include "AbstractionPlayerCharacter.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystemComponent.h"
#include "Misc/ScopeLock.h"


UDamageHandlerComponent::UDamageHandlerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AAbstractionPlayerCharacter>(GetOwner());
}


// Called every frame
void UDamageHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PlayerCharacter)
	{
		FScopeLock Lock(&CriticalSection);
		if (ActiveDamageInfo.IsSet())
		{
			if (ActiveDamageInfo.GetValue().AccumulatedTime > ActiveDamageInfo.GetValue().Lifetime)
			{
				if (PlayerCharacter->ParticleSystemComponent)
				{
					PlayerCharacter->ParticleSystemComponent->Deactivate();
					PlayerCharacter->ParticleSystemComponent->SetTemplate(nullptr);
				}
				ActiveDamageInfo.Reset();
			}
			else
			{
				ActiveDamageInfo.GetValue().AccumulatedTime += DeltaTime;
				ActiveDamageInfo.GetValue().CurrentIntervalTime += DeltaTime;
				if (ActiveDamageInfo.GetValue().CurrentIntervalTime > ActiveDamageInfo.GetValue().IntervalTime)
				{
					float ModifiedDamage = ActiveDamageInfo.GetValue().BaseDamage / (ActiveDamageInfo.GetValue().Lifetime / ActiveDamageInfo.GetValue().IntervalTime);
					TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
					FDamageEvent DamageEvent(ValidDamageTypeClass);
					PlayerCharacter->TakeDamage(ModifiedDamage, DamageEvent, nullptr, GetOwner());
					ActiveDamageInfo.GetValue().CurrentIntervalTime = 0.0f;
				}
			}
		}
	}
}

void UDamageHandlerComponent::TakeFireDamage(float BaseDamage, float DamageTotalTime, float TakeDamageInterval)
{
	FScopeLock Lock(&CriticalSection);
	if (ActiveDamageInfo.IsSet())
	{
		ActiveDamageInfo.GetValue().BaseDamage = FMath::Max(ActiveDamageInfo.GetValue().BaseDamage, BaseDamage);

		//extend the lifetime
		if (ActiveDamageInfo.GetValue().Lifetime < DamageTotalTime)
		{
			ActiveDamageInfo.GetValue().Lifetime = DamageTotalTime;
			ActiveDamageInfo.GetValue().IntervalTime = ActiveDamageInfo.GetValue().IntervalTime;
		}
	}
	else
	{
		ActiveDamageInfo.Emplace();
		ActiveDamageInfo.GetValue().BaseDamage = BaseDamage;
		ActiveDamageInfo.GetValue().IntervalTime = TakeDamageInterval;
		ActiveDamageInfo.GetValue().Lifetime = DamageTotalTime;

		if (FireTemplate && PlayerCharacter->ParticleSystemComponent)
		{
			PlayerCharacter->ParticleSystemComponent->SetTemplate(FireTemplate);
			PlayerCharacter->ParticleSystemComponent->Activate(true);
		}
	}
}