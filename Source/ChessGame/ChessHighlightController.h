// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Components/ActorComponent.h"
#include "ChessHighlightController.generated.h"

class AChessGameGameModeBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHESSGAME_API UChessHighlightController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChessHighlightController();

	UMaterialInstanceDynamic* MaterialToUse = nullptr;
	UMaterialInstanceConstant* MaterialConstantInstance = nullptr;
	FString *ColorNames = nullptr;
	FLinearColor* ColorValues = nullptr;
	FString *OnOffNames = nullptr;
	float* OnOffValues = nullptr;

	float Timer = 0.f;
	float Step = 0.2f;

	int CurrentIndex = 0;

	AChessGameGameModeBase* GameMode = nullptr;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateMaterialParameters();
	void SetOnOffByIndex(int i, float value);
	void SetColorByIndex(int i, FLinearColor Color);
	void SetAllOff();
	void ClearAllColor();
};
