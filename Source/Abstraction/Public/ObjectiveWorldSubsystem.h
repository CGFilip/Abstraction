// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveComponent.h"
#include "ObjectiveWorldSubsystem.generated.h"

class UObjectiveComponent;
/**
 * 
 */
UCLASS()
class ABSTRACTION_API UObjectiveWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void CreateObjectiveWidget(TSubclassOf<UUserWidget> ObjectiveWidgetClass);
	void DisplayObjectiveWidget();

	void OnObjectiveCompleted();

	UFUNCTION(BlueprintCallable)
	FString GetCurrentObjectiveDescription();

	UFUNCTION(BlueprintCallable)
	void AddObjective(UObjectiveComponent* ObjectiveComponent);

	UFUNCTION(BlueprintCallable)
	void RemoveObjective(UObjectiveComponent* ObjectiveComponent);

	void OnObjectiveStateChanged(UObjectiveComponent* ObjectiveComponent, EObjectiveState ObjectiveState);

private:
	UUserWidget* ObjectiveWidget = nullptr;

	TArray<const UObjectiveComponent*> Objectives;

};
