// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessPlayerController.h"
#include "ChessGameGameModeBase.h"
#include "ChessHighlightController.h"
#include "ChessPiece.h"


AChessPlayerController::AChessPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AChessPlayerController::BeginPlay()
{
	InputComponent->BindAction("LeftMouseButton", IE_Released, this, &AChessPlayerController::MouseLeftClick);
}

void AChessPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if(!GameMode)
		GameMode = Cast<AChessGameGameModeBase>(GetWorld()->GetAuthGameMode());

	FHitResult Hit;

	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, Hit);


	UpdateHighlight(Hit);
	UpdateBoardHighlight(Hit);
}

void AChessPlayerController::BeginPlayingState()
{
}

void AChessPlayerController::UpdateHighlight(FHitResult &Hit)
{
	
	if (Hit.Actor != nullptr)
	{
		if (GameMode->IsClickable(Hit.Actor.Get()))
		{
			AChessPiece* ChessPiece = Cast<AChessPiece>(Hit.Actor.Get());
			// Make the actor shine!

			if (Hit.Actor.Get() != LastHighlightedActor && LastHighlightedActor != nullptr)
			{
				AChessPiece* LastChessPiece = Cast<AChessPiece>(LastHighlightedActor);

				LastChessPiece->RemoveHighlight();
				LastHighlightedActor = nullptr;
			}
			else
			{
				ChessPiece->ToggleHighlight();
				LastHighlightedActor = Hit.Actor.Get();
			}
		}
		else
		{
			if (LastHighlightedActor)
			{
				AChessPiece* ChessPiece = Cast<AChessPiece>(LastHighlightedActor);

				ChessPiece->RemoveHighlight();
				LastHighlightedActor = nullptr;
			}
		}
	}
	else
	{
		if (LastHighlightedActor)
		{
			AChessPiece* ChessPiece = Cast<AChessPiece>(LastHighlightedActor);

			ChessPiece->RemoveHighlight();
			LastHighlightedActor = nullptr;
		}
	}
}

void AChessPlayerController::UpdateBoardHighlight(FHitResult & Hit)
{
	if (LastSelectedActor != nullptr)
	{
		AChessPiece* ChessPiece = Cast<AChessPiece>(LastSelectedActor);

		if (!ChessPiece->MovesAreHighlighted)
		{
			UChessHighlightController* HighlightController = Cast<UChessHighlightController>(GameMode->HighlightController);

			TArray<int> IndexesToHighlight;
			ChessPiece->GetPossibleMoveHighlight(IndexesToHighlight);

			for (int i = 0; i < IndexesToHighlight.Num(); i++)
			{
				HighlightController->SetOnOffByIndex(63 - IndexesToHighlight[i], 1.0f);
			}

			LastMovesHighlightedActor = LastSelectedActor;

			ChessPiece->MovesAreHighlighted = true;
		}

		GetSlotIndexFromWorld(Hit.ImpactPoint, SlotPointedAtI, SlotPointedAtJ);
	}
}

int AChessPlayerController::GetSlotIndexFromWorld(FVector HitPoint, int& I, int &J)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			// Care only about X / Y coordinates...
			if (HitPoint.X >= GameMode->BoardInstance.Slots[i][j].Position.X - GameMode->BoardInstance.Slots[i][j].Left && HitPoint.X <= GameMode->BoardInstance.Slots[i][j].Position.X + GameMode->BoardInstance.Slots[i][j].Right)
			{
				if (HitPoint.Y >= GameMode->BoardInstance.Slots[i][j].Position.Y - GameMode->BoardInstance.Slots[i][j].Top && HitPoint.Y <= GameMode->BoardInstance.Slots[i][j].Position.Y + GameMode->BoardInstance.Slots[i][j].Bottom)
				{
					UE_LOG(LogTemp, Warning, TEXT("Inside Square i: %s, j: %s"), *FString::FromInt(i), *FString::FromInt(j));
					I = i;
					J = j;
					HoveringOverSlot = true;
					return i * 8 + (7-j);
				}
			}
		}
	}

	HoveringOverSlot = false;

	return -1;
}



void AChessPlayerController::MouseLeftClick()
{
	FHitResult Hit;

	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Hit);

	// We are clicking on an actor
	if (Hit.Actor != nullptr)
	{
		AChessPiece* ChessPiece = Cast<AChessPiece>(Hit.Actor.Get());

		if (GameMode->IsClickable(Hit.Actor.Get()))
		{
			if (LastSelectedActor != Hit.Actor.Get() && LastSelectedActor != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Removing Toggle!"));
				AChessPiece* LastChessPiece = Cast<AChessPiece>(LastSelectedActor);

				LastChessPiece->RemoveSelected();
				LastSelectedActor = nullptr;
				GameMode->CurrentlySelectedActor = nullptr;
				Cast<UChessHighlightController>(GameMode->HighlightController)->SetAllOff();
				Cast<AChessPiece>(LastMovesHighlightedActor)->MovesAreHighlighted = false;
			}

			if (LastSelectedActor == Hit.Actor.Get() && LastSelectedActor != nullptr)
			{
				ChessPiece->RemoveSelected();
				LastSelectedActor = nullptr;
				GameMode->CurrentlySelectedActor = nullptr;
				Cast<UChessHighlightController>(GameMode->HighlightController)->SetAllOff();
				Cast<AChessPiece>(LastMovesHighlightedActor)->MovesAreHighlighted = false;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Highlighting new object!"));
				ChessPiece->ToggleSelected();
				GameMode->CurrentlySelectedActor = ChessPiece;
				LastSelectedActor = ChessPiece;
			}
		}
		else
		{
			if (LastSelectedActor != nullptr)
			{
				
				AChessPiece* LastChessPiece = Cast<AChessPiece>(LastSelectedActor);

				// Check if the selected piece can move to the slot you are pointing at
				if (HoveringOverSlot)
				{
					UE_LOG(LogTemp, Warning, TEXT("Clicked a slot while a piece was selected"));
					// Check if we can actually move to the slot we clicked...

					if (LastChessPiece->isValidMove(SlotPointedAtI, SlotPointedAtJ))
					{
						GameMode->BoardInstance.Slots[LastChessPiece->CurrentSlotI][LastChessPiece->CurrentSlotJ].Occupant = nullptr;
						LastChessPiece->CurrentSlotI = SlotPointedAtI;
						LastChessPiece->CurrentSlotJ = SlotPointedAtJ;
						GameMode->BoardInstance.Slots[LastChessPiece->CurrentSlotI][LastChessPiece->CurrentSlotJ].Occupant = LastChessPiece;
					}
				}

				UE_LOG(LogTemp, Warning, TEXT("Removing Toggle!"));
				// Remove Selection and Board Highlighting
				LastChessPiece->RemoveSelected();
				LastSelectedActor = nullptr;
				GameMode->CurrentlySelectedActor = nullptr;
				Cast<UChessHighlightController>(GameMode->HighlightController)->SetAllOff();
				Cast<AChessPiece>(LastMovesHighlightedActor)->MovesAreHighlighted = false;
			}
		}
	}
	else
	{
		if (LastSelectedActor != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Removing Toggle!"));
			AChessPiece* LastChessPiece = Cast<AChessPiece>(LastSelectedActor);

			LastChessPiece->RemoveSelected();
			LastSelectedActor = nullptr;
			GameMode->CurrentlySelectedActor = nullptr;
			Cast<UChessHighlightController>(GameMode->HighlightController)->SetAllOff();
			Cast<AChessPiece>(LastMovesHighlightedActor)->MovesAreHighlighted = false;
		}
	}
}
