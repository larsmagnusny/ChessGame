// Fill out your copyright notice in the Description page of Project Settings.
#include "ChessPlayerController.h"
#include "ChessGameState.h"
#include "ChessBoardActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "ChessPiece.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Pawn.h"
//#include "ChessPlayerController.h"

AChessPlayerController::AChessPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	PrimaryActorTick.bCanEverTick = true;
	bAutoManageActiveCameraTarget = false;
}

void AChessPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!GameState)
		GameState = Cast<AChessGameState>(GetWorld()->GetGameState());

	if (GameState->CurrentPlayers == 0)
	{
		PlayerType = 0;
		GameState->CurrentPlayers++;
	}
	else
	{
		PlayerType = 1;
		GameState->CurrentPlayers++;
	}

	if (GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		InputComponent->BindAction("LeftMouseButton", IE_Released, this, &AChessPlayerController::MouseLeftClick);
	}

	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		GameState->SpawnPieces();
	}
}

void AChessPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	FHitResult Hit;

	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, Hit);

	UpdateHighlight(Hit);
	//UpdateBoardHighlight(Hit);

	if (GetNetMode() == ENetMode::NM_Client)
	{
		UE_LOG(LogTemp, Warning, TEXT("Camera set on client: %s"), *PlayerCameraManager->GetViewTarget()->GetName());
	}

	if (!CameraViewSet || (GetViewTarget() != Camera && Camera != nullptr))
	{
		if (GetNetMode() == ENetMode::NM_Client)
		{
			UE_LOG(LogTemp, Warning, TEXT("Camera set on client: %s"), *PlayerCameraManager->GetViewTarget()->GetName());
		}
		if (GetNetMode() == ENetMode::NM_DedicatedServer)
		{
			UE_LOG(LogTemp, Warning, TEXT("Camera set on Dedicated Server"), *PlayerCameraManager->GetViewTarget()->GetName());
		}
		if (GetNetMode() == ENetMode::NM_Standalone)
		{
			UE_LOG(LogTemp, Warning, TEXT("Camera set on StandAlone"), *PlayerCameraManager->GetViewTarget()->GetName());
		}
		
		FVector Location = FVector(0, 17.f, 36.f);
		FRotator Rotation = FRotator(-70.f, -90.f, 0.f);

		if (PlayerType == 1)
		{
			Location = FVector(0.f, -17.f, 36.f);
			Rotation = FRotator(-70.f, 90.f, 0.f);
		}

		FActorSpawnParameters SpawnInfo;

		Camera = GetWorld()->SpawnActor<ACameraActor>(Location, Rotation, SpawnInfo);

		SetViewTarget(Camera);

		CameraViewSet = true;
	}
}

void AChessPlayerController::BeginPlayingState()
{
}

void AChessPlayerController::UpdateHighlight(FHitResult &Hit)
{
	if (GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		if (Hit.Actor != nullptr)
		{
			if (GameState->IsClickable(Hit.Actor.Get()))
			{
				AChessPiece* ChessPiece = Cast<AChessPiece>(Hit.Actor.Get());
				// Make the actor shine!

				if (Hit.Actor.Get() != LastHighlightedActor && LastHighlightedActor != nullptr)
				{
					AChessPiece* LastChessPiece = Cast<AChessPiece>(LastHighlightedActor);

					LastChessPiece->MultiplayerSafeRemoveHighlight();
					LastHighlightedActor = nullptr;
				}
				else if (PlayerType == ChessPiece->type)
				{
					UE_LOG(LogTemp, Warning, TEXT("ToggleHighlight Called"));
					ChessPiece->MultiplayerSafeToggleHighlight();
					LastHighlightedActor = Hit.Actor.Get();
				}
			}
			else
			{
				if (LastHighlightedActor)
				{
					AChessPiece* ChessPiece = Cast<AChessPiece>(LastHighlightedActor);

					ChessPiece->MultiplayerSafeRemoveHighlight();
					LastHighlightedActor = nullptr;
				}
			}
		}
		else
		{
			if (LastHighlightedActor)
			{
				AChessPiece* ChessPiece = Cast<AChessPiece>(LastHighlightedActor);

				ChessPiece->MultiplayerSafeRemoveHighlight();
				LastHighlightedActor = nullptr;
			}
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
			AChessBoardActor* HighlightController = Cast<AChessBoardActor>(GameState->HighlightController);

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
			if (HitPoint.X >= GameState->BoardInstance.Slots[i][j].Position.X - GameState->BoardInstance.Slots[i][j].Left && HitPoint.X <= GameState->BoardInstance.Slots[i][j].Position.X + GameState->BoardInstance.Slots[i][j].Right)
			{
				if (HitPoint.Y >= GameState->BoardInstance.Slots[i][j].Position.Y - GameState->BoardInstance.Slots[i][j].Top && HitPoint.Y <= GameState->BoardInstance.Slots[i][j].Position.Y + GameState->BoardInstance.Slots[i][j].Bottom)
				{
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
		AChessPiece* LastChessPiece = nullptr;

		if (LastSelectedActor != nullptr)
			LastChessPiece = Cast<AChessPiece>(LastSelectedActor);

		if (GameState->IsClickable(Hit.Actor.Get()))
		{
			if (LastSelectedActor != Hit.Actor.Get() && LastSelectedActor != nullptr)
			{
				if (LastChessPiece->type == ChessPiece->type)
				{
					// Remove Selection and Board Highlighting
					RemoveHighlighting(LastChessPiece);
				}
				else
				{
					if (LastChessPiece->isValidMove(ChessPiece->CurrentSlotI, ChessPiece->CurrentSlotJ))
					{
						int SlotToMoveToI = ChessPiece->CurrentSlotI;
						int SlotToMoveToJ = ChessPiece->CurrentSlotJ;
						if (GameState->BoardInstance.Slots[SlotToMoveToI][SlotToMoveToJ].Occupant != nullptr)
						{
							GameState->BoardInstance.Slots[SlotToMoveToI][SlotToMoveToJ].Occupant->Destroy();
							GameState->BoardInstance.Slots[SlotToMoveToI][SlotToMoveToJ].Occupant = nullptr;
						}

						GameState->BoardInstance.Slots[LastChessPiece->CurrentSlotI][LastChessPiece->CurrentSlotJ].Occupant = nullptr;
						LastChessPiece->CurrentSlotI = SlotToMoveToI;
						LastChessPiece->CurrentSlotJ = SlotToMoveToJ;
						LastChessPiece->isOnStartingSquare = false;
						GameState->BoardInstance.Slots[LastChessPiece->CurrentSlotI][LastChessPiece->CurrentSlotJ].Occupant = LastChessPiece;

						// Remove Selection and Board Highlighting
						RemoveHighlighting(LastChessPiece);

						GameState->RunCameraAnimation = true;
						return;
					}
					else
					{
						// Remove Selection and Board Highlighting
						RemoveHighlighting(LastChessPiece);
						return;
					}
				}
			}

			if (LastSelectedActor == Hit.Actor.Get() && LastSelectedActor != nullptr)
			{
				// Remove Selection and Board Highlighting
				RemoveHighlighting(ChessPiece);
			}
			else if(PlayerType == ChessPiece->type)
			{
				ChessPiece->ToggleSelected();
				GameState->CurrentlySelectedActor = ChessPiece;
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
						// Check if there is a chesspiece in the spot you want to move, so you can take it.
						if (GameState->BoardInstance.Slots[SlotPointedAtI][SlotPointedAtJ].Occupant != nullptr)
						{
							GameState->BoardInstance.Slots[SlotPointedAtI][SlotPointedAtJ].Occupant->Destroy();
							GameState->BoardInstance.Slots[SlotPointedAtI][SlotPointedAtJ].Occupant = nullptr;
						}

						GameState->BoardInstance.Slots[LastChessPiece->CurrentSlotI][LastChessPiece->CurrentSlotJ].Occupant = nullptr;
						LastChessPiece->CurrentSlotI = SlotPointedAtI;
						LastChessPiece->CurrentSlotJ = SlotPointedAtJ;
						LastChessPiece->isOnStartingSquare = false;
						GameState->BoardInstance.Slots[LastChessPiece->CurrentSlotI][LastChessPiece->CurrentSlotJ].Occupant = LastChessPiece;

						GameState->RunCameraAnimation = true;
					}
				}

				// Remove Selection and Board Highlighting
				RemoveHighlighting(LastChessPiece);
			}
		}
	}
	else
	{
		if (LastSelectedActor != nullptr)
		{
			AChessPiece* LastChessPiece = Cast<AChessPiece>(LastSelectedActor);

			// Remove Selection and Board Highlighting
			RemoveHighlighting(LastChessPiece);
		}
	}
}

void AChessPlayerController::RemoveHighlighting(AChessPiece * LastChessPiece)
{
	LastChessPiece->MultiplayerSafeRemoveSelected();
	LastSelectedActor = nullptr;
	GameState->CurrentlySelectedActor = nullptr;
	Cast<AChessBoardActor>(GameState->HighlightController)->SetAllOff();
	Cast<AChessPiece>(LastMovesHighlightedActor)->MovesAreHighlighted = false;
}