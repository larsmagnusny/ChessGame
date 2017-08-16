// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChessBoard.h"
#include "Camera/CameraComponent.h"
#include "ChessGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CHESSGAME_API AChessGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AChessGameGameModeBase();
	virtual void BeginPlay() override;

	bool IsClickable(AActor* Actor);


	TArray<AActor*> ClickableActors;
	AActor* CurrentlySelectedActor = nullptr;

	ChessBoard BoardInstance;
	UActorComponent* HighlightController = nullptr;

	UFUNCTION(BlueprintCallable, Category="Set the view target")
	void SetViewTarget(AActor* Target);

	int CurrentPlayer = 0;
	bool NextPlayer = false;
private:
	FVector StartPosition = FVector(-18.f, 18.f, 1.f);
	float Delta = 5.13f;
};
