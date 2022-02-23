// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveHud.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class ABSTRACTION_API UObjectiveHud : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateObjectiveText(uint32 ObjectivesCompleted, uint32 TotalObjectives);
protected:

	// Allows us to get access to the same named TextBlock in the widget
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ObjectivesText; 
};
