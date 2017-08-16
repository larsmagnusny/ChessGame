// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessHighlightController.h"
#include "ChessGameGameModeBase.h"

// Sets default values for this component's properties
UChessHighlightController::UChessHighlightController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ColorNames = new FString[64];

	ColorNames[0] = "HighlightColor";

	for (int i = 1; i < 64; i++)
	{
		ColorNames[i] = ColorNames[0] + "_" + FString::FromInt(i);
	}

	ColorValues = new FLinearColor[64];
	for (int i = 0; i < 64; i++)
		ColorValues[i] = FLinearColor(1.0f, 1.0f, 1.0f);

	OnOffNames = new FString[64];

	OnOffNames[0] = "OnOff";

	for (int i = 1; i < 64; i++)
	{
		OnOffNames[i] = OnOffNames[0] + "_" + FString::FromInt(i);
	}

	OnOffValues = new float[64];
	for (int i = 0; i < 64; i++)
		OnOffValues[i] = 0.f;

	// Load Material Instance
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MaterialLoader(TEXT("MaterialInstanceConstant'/Game/ChessBoard_Inst.ChessBoard_Inst'"));

	if (MaterialLoader.Succeeded())
	{
		MaterialConstantInstance = MaterialLoader.Object;
	}
}


// Called when the game starts
void UChessHighlightController::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AChessGameGameModeBase>(GetWorld()->GetAuthGameMode());

	GameMode->HighlightController = this;

	// Change Material Of object to a dynamic instance
	UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if(MaterialConstantInstance != nullptr)
		MaterialToUse = UMaterialInstanceDynamic::Create(MaterialConstantInstance, this);

	if (MaterialToUse != nullptr)
		MeshComponent->SetMaterial(0, MaterialToUse);
}


// Called every frame
void UChessHighlightController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update Material Paramaters
	UpdateMaterialParameters();
}

void UChessHighlightController::UpdateMaterialParameters()
{
	// Update OnOff
	for (int i = 0; i < 64; i++)
	{
		MaterialToUse->SetScalarParameterValue(FName(*OnOffNames[i]), OnOffValues[i]);
	}


	// Update Colors
	for (int i = 0; i < 64; i++)
	{
		MaterialToUse->SetVectorParameterValue(FName(*ColorNames[i]), ColorValues[i]);
	}
}

void UChessHighlightController::SetOnOffByIndex(int i, float value)
{
	if (i < 64)
	{
		OnOffValues[i] = value;
	}
}

void UChessHighlightController::SetColorByIndex(int i, FLinearColor Color)
{
	if (i < 64)
	{
		ColorValues[i] = Color;
	}
}

void UChessHighlightController::SetAllOff()
{
	for (int i = 0; i < 64; i++)
	{
		OnOffValues[i] = 0.f;
	}
}

void UChessHighlightController::ClearAllColor()
{
	for (int i = 0; i < 64; i++)
	{
		ColorValues[i] = FLinearColor(0.f, 0.f, 0.f);
	}
}

