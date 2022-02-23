// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveComponent.h"
#include "ObjectiveWorldSubsystem.generated.h"

class UObjectiveComponent;
class UObjectiveHud;
class UUserWidget;
/**
 * 
 */
UCLASS()
class ABSTRACTION_API UObjectiveWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	FString GetCurrentObjectiveDescription();

	UFUNCTION(BlueprintCallable)
	void AddObjective(UObjectiveComponent* ObjectiveComponent);

	UFUNCTION(BlueprintCallable)
	void RemoveObjective(UObjectiveComponent* ObjectiveComponent);

	//we call this from maps that we want to display objectives(ie main menu will not call this function)
	UFUNCTION(BlueprintCallable)
	void OnMapStart();

protected:

	virtual void Deinitialize() override;

	void CreateObjectiveWidgets();

	void DisplayObjectiveWidget();
	void RemoveObjectiveWidget();

	void DisplayObjectivesCompleteWidget();
	void RemoveObjectivesCompleteWidget();

	uint32 GetCompletedObjectiveCount();

	void OnObjectiveStateChanged(UObjectiveComponent* ObjectiveComponent, EObjectiveState ObjectiveState);
private:
	UPROPERTY()
	UObjectiveHud* ObjectiveWidget = nullptr;
	UPROPERTY()
	UUserWidget* ObjectivesCompleteWidget = nullptr;

	TArray<const UObjectiveComponent*> Objectives;
};
