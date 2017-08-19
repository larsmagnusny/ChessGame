// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "ChessPlayerController.h"
#include "ChessPlayerPawn.h"
#include "Engine/World.h"

AChessGameGameModeBase::AChessGameGameModeBase()
{
	// Don't use default pawn
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = nullptr;
	PlayerControllerClass = AChessPlayerController::StaticClass();
	GameStateClass = AChessGameState::StaticClass();
}

void AChessGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld()->GetMapName().Contains("ChessGame"))
	{
		UGameplayStatics::OpenLevel(GetWorld(), "ChessGame");
	}

	GameState = Cast<AChessGameState>(GetWorld()->GetGameState());

	if(GetNetMode() == ENetMode::NM_Standalone)
	{
		FString WorldName = GetWorld()->GetMapName();

		UE_LOG(LogTemp, Warning, TEXT("Game is standalone"), *WorldName);
		UE_LOG(LogTemp, Warning, TEXT("Loaded map: %s"), *WorldName);

		UGameplayStatics::OpenLevel(GetWorld(), "127.0.0.1");
	}
	else
	{
		FString WorldName = GetWorld()->GetMapName();

		UE_LOG(LogTemp, Warning, TEXT("Loaded map: %s"), *WorldName);
	}

	
}

