// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// Forward declaration
class AActor;

/**
 * 
 */
class CHESSGAME_API ChessBoardSlot
{
public:
	ChessBoardSlot();
	~ChessBoardSlot();

	FVector Position;
	float Left;
	float Right;
	float Top;
	float Bottom;
	AActor* Occupant = nullptr;
};
