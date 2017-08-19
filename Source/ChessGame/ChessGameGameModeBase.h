// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Camera/CameraComponent.h"
#include "ChessGameState.h"
#include "ChessGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CHESSGAME_API AChessGameGameModeBase : public AGameMode
{
	GENERATED_BODY()
public:
	AChessGameGameModeBase();
	virtual void BeginPlay() override;

	AChessGameState* GameState = nullptr;

	int MaxPlayers = 2;
	int CurrentPlayers = 0;
};
