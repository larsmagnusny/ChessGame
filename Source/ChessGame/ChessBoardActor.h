// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "Engine/StaticMeshActor.h"
#include "ChessBoardActor.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInstanceConstant;
class AChessGameState;

UCLASS()
class CHESSGAME_API AChessBoardActor : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	AChessBoardActor();
	
	virtual void BeginPlay() override;

	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction) override;

	void UpdateMaterialParameters();

	void SetOnOffByIndex(int i, float value);
	void SetColorByIndex(int i, FLinearColor Color);
	void SetAllOff();
	void ClearAllColor();


	UMaterialInstanceDynamic* MaterialToUse = nullptr;
	UMaterialInstanceConstant* MaterialConstantInstance = nullptr;

	UStaticMesh* Mesh = nullptr;

	FString *ColorNames = nullptr;

	TArray<FLinearColor> ColorValues;
	FString *OnOffNames = nullptr;

	TArray<float> OnOffValues;

	float Timer = 0.f;
	float Step = 0.2f;

	int CurrentIndex = 0;

	AChessGameState* GameState = nullptr;
};
