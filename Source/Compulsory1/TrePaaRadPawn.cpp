// Fill out your copyright notice in the Description page of Project Settings.

#include "TrePaaRadPawn.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/StaticMeshComponent.h"
#include <UObject/ConstructorHelpers.h>
#include "Components/SphereComponent.h"
#include "Containers/Array.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"


ATrePaaRadPawn::ATrePaaRadPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultMaterial = CreateDefaultSubobject<UMaterial>(TEXT("MM_DefaultMaterial"));
	Red = CreateDefaultSubobject<UMaterialInstance>(TEXT("Red"));
	Blue = CreateDefaultSubobject<UMaterialInstance>(TEXT("Blue"));
	ColorWon = CreateDefaultSubobject<UMaterialInstance>(TEXT("Won"));
	ColorDraw = CreateDefaultSubobject<UMaterialInstance>(TEXT("Draw"));

	GenerateSphereMeshes();

	SetSpherePositions();

	GenerateCameraAndArm();

	GenerateAudioComponent();

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ATrePaaRadPawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}

	GenerateSphereMaterials();

	bIsSphereColorChanged.Init(NULL, 9);
	bIsRed.Init(NULL, 9);
	bIsBlue.Init(NULL, 9);
	TurnCounter = 0;
	GameState = EGameState::Playing;

	AudioComponent->Play();
}


void ATrePaaRadPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TurnCounter == 9)
	{
		for (int32 i{}; i < SphereMeshArray.Num(); i++)
		{
			SphereMeshArray[i]->SetMaterial(0, ColorDraw);
		}
	}

	if(GameState == EGameState::Won)
	{
		for (int32 i{}; i < SphereMeshArray.Num(); i++)
		{
			SphereMeshArray[i]->SetMaterial(0, ColorWon);
		}
	}
}


//--Generators--//
void ATrePaaRadPawn::GenerateSphereMeshes()
{
	SphereMeshArray.Init(NULL, 9);
	for (int32 i{}; i < SphereMeshArray.Num(); i++)
	{
		FString Number = FString::FromInt(i);
		SphereMeshArray[i] = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("MeshNr") + FString::FromInt(i)), false);
	}
	

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Props/MaterialSphere.MaterialSphere'"));

	SetRootComponent(SphereMeshArray[4]);

	for (int32 i{}; i < SphereMeshArray.Num(); i++)
	{
		SphereMeshArray[i]->SetStaticMesh(MeshComponentAsset.Object);
	}

	for (int32 i{}; i < 9; i++)
	{
		if (i != 4)
		{
			SphereMeshArray[i]->SetupAttachment(GetRootComponent());
		}
	}
}

void ATrePaaRadPawn::GenerateSphereMaterials()
{
	for (int32 i{}; i < SphereMeshArray.Num(); i++)
	{
		SphereMeshArray[i]->SetMaterial(0, DefaultMaterial);
	}
}

void ATrePaaRadPawn::SetSpherePositions()
{
	float OffsetY = -200.f;
	float OffsetX = -200.f;
	for (int32 i = 0; i < SphereMeshArray.Num(); i++)
	{
		SphereMeshArray[i]->SetRelativeLocation(FVector(OffsetX, OffsetY, 0.f));
		for (int32 q = 0; q < 2; q++)
		{
			OffsetX += 200.f;
			i++;
			SphereMeshArray[i]->SetRelativeLocation(FVector(OffsetX, OffsetY, 0.f));
		}
		OffsetY += 200;
		OffsetX = -200.f;
	}
}

void ATrePaaRadPawn::GenerateAudioComponent()
{
	static ConstructorHelpers::FObjectFinder<USoundCue> PlayCue(TEXT("'/Game/StarterContent/Audio/Starter_Music01.Starter_Music01'"));
	AudioCue = PlayCue.Object;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(GetRootComponent());
	AudioComponent->SetSound(AudioCue);
}

void ATrePaaRadPawn::GenerateCameraAndArm()
{
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(GetRootComponent());
	CameraArm->SetRelativeRotation(FRotator(-90.f, 0.f, -90.f));
	CameraArm->TargetArmLength = 800.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = 1000;
}


//--GameCheckers--//
void ATrePaaRadPawn::TurnControllChecker(int32 SphereIndex)
{
	if(bIsSphereColorChanged[SphereIndex] == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Index %d is allready taken"), SphereIndex);
		return;
	}

	if (TurnCounter % 2 == 0)
	{
		SphereMeshArray[SphereIndex]->SetMaterial(0, Blue);
		bIsBlue[SphereIndex] = true;
		
	} else if (TurnCounter % 2 == 1)
	{
		SphereMeshArray[SphereIndex]->SetMaterial(0, Red);
		bIsRed[SphereIndex] = true;
	}
	bIsSphereColorChanged[SphereIndex] = true;
	TurnCounter++;
}

//--ActionKeys--//
void ATrePaaRadPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ButtonOneAction, ETriggerEvent::Triggered, this, &ATrePaaRadPawn::KeyOnePressed);
		EnhancedInputComponent->BindAction(ButtonTwoAction, ETriggerEvent::Triggered, this, &ATrePaaRadPawn::KeyTwoPressed);
		EnhancedInputComponent->BindAction(ButtonThreeAction, ETriggerEvent::Triggered, this, &ATrePaaRadPawn::KeyThreePressed);
		EnhancedInputComponent->BindAction(ButtonFourAction, ETriggerEvent::Triggered, this, &ATrePaaRadPawn::KeyFourPressed);
		EnhancedInputComponent->BindAction(ButtonFiveAction, ETriggerEvent::Triggered, this, &ATrePaaRadPawn::KeyFivePressed);
		EnhancedInputComponent->BindAction(ButtonSixAction, ETriggerEvent::Triggered, this, &ATrePaaRadPawn::KeySixPressed);
		EnhancedInputComponent->BindAction(ButtonSevenAction, ETriggerEvent::Triggered, this, &ATrePaaRadPawn::KeySevenPressed);
		EnhancedInputComponent->BindAction(ButtonEightAction, ETriggerEvent::Triggered, this, &ATrePaaRadPawn::KeyEightPressed);
		EnhancedInputComponent->BindAction(ButtonNineAction, ETriggerEvent::Triggered, this, &ATrePaaRadPawn::KeyNinePressed);
	}
}


//--KeyActionFunctions--//
void ATrePaaRadPawn::KeyOnePressed()
{
	if (GameState == EGameState::Playing)
	{
		TurnControllChecker(0);
		if (((bIsBlue[0] && bIsBlue[1] && bIsBlue[2]) || (bIsBlue[0] && bIsBlue[3] && bIsBlue[6]) || (bIsBlue[0] && bIsBlue[4] && bIsBlue[8]))
			|| ((bIsRed[0] && bIsRed[1] && bIsRed[2]) || (bIsRed[0] && bIsRed[3] && bIsRed[6]) || (bIsRed[0] && bIsRed[4] && bIsRed[8])))
		{
			GameState = Won;
			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
		}
	}
}

void ATrePaaRadPawn::KeyTwoPressed()
{
	if (GameState == EGameState::Playing)
	{
		TurnControllChecker(1);
		if (((bIsBlue[1] && bIsBlue[0] && bIsBlue[2]) || (bIsBlue[1] && bIsBlue[4] && bIsBlue[7]))
			|| ((bIsRed[1] && bIsRed[0] && bIsRed[2]) || (bIsRed[1] && bIsRed[4] && bIsRed[7])))
		{
			GameState = Won;
			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
		}

	}
}

void ATrePaaRadPawn::KeyThreePressed()
{
	if (GameState == EGameState::Playing)
	{
		TurnControllChecker(2);
		if (((bIsBlue[2] && bIsBlue[1] && bIsBlue[0]) || (bIsBlue[2] && bIsBlue[4] && bIsBlue[6]) || (bIsBlue[2] && bIsBlue[5] && bIsBlue[8]))
			|| ((bIsRed[2] && bIsRed[1] && bIsRed[0]) || (bIsRed[2] && bIsRed[4] && bIsRed[6]) || (bIsRed[2] && bIsRed[5] && bIsRed[8])))
		{
			GameState = Won;
			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
		}
	}
}

void ATrePaaRadPawn::KeyFourPressed()
{
	if (GameState == EGameState::Playing)
	{
		TurnControllChecker(3);
		if (((bIsBlue[3] && bIsBlue[0] && bIsBlue[6]) || (bIsBlue[3] && bIsBlue[4] && bIsBlue[5]))
			|| ((bIsRed[3] && bIsRed[0] && bIsRed[6]) || (bIsRed[3] && bIsRed[4] && bIsRed[5])))
		{
			GameState = Won;
			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
		}
	}
}

void ATrePaaRadPawn::KeyFivePressed()
{
	if (GameState == EGameState::Playing)
	{
		TurnControllChecker(4);
		if (((bIsBlue[4] && bIsBlue[1] && bIsBlue[7]) || (bIsBlue[4] && bIsBlue[3] && bIsBlue[5]) || (bIsBlue[4] && bIsBlue[0] && bIsBlue[8]) || (bIsBlue[4] && bIsBlue[2] && bIsBlue[6]))
			|| ((bIsRed[4] && bIsRed[1] && bIsRed[7]) || (bIsRed[4] && bIsRed[3] && bIsRed[5]) || (bIsRed[4] && bIsRed[0] && bIsRed[8]) || (bIsRed[4] && bIsRed[2] && bIsRed[6])))
		{
			GameState = Won;
			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
		}
	}
}

void ATrePaaRadPawn::KeySixPressed()
{
	if (GameState == EGameState::Playing)
	{
		TurnControllChecker(5);
		if (((bIsBlue[5] && bIsBlue[2] && bIsBlue[8]) || (bIsBlue[5] && bIsBlue[3] && bIsBlue[4]))
			|| ((bIsRed[5] && bIsRed[2] && bIsRed[8]) || (bIsRed[5] && bIsRed[3] && bIsRed[4])))
		{
			GameState = Won;
			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
		}
	}
}

void ATrePaaRadPawn::KeySevenPressed()
{
	if (GameState == EGameState::Playing)
	{
		TurnControllChecker(6);
		if (((bIsBlue[6] && bIsBlue[3] && bIsBlue[0]) || (bIsBlue[6] && bIsBlue[7] && bIsBlue[8]) || (bIsBlue[6] && bIsBlue[4] && bIsBlue[2]))
			|| ((bIsRed[6] && bIsRed[3] && bIsRed[0]) || (bIsRed[6] && bIsRed[7] && bIsRed[8]) || (bIsRed[6] && bIsRed[4] && bIsRed[2])))
		{
			GameState = Won;
			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
		}
	}
}

void ATrePaaRadPawn::KeyEightPressed()
{
	if (GameState == EGameState::Playing)
	{
		TurnControllChecker(7);
		if (((bIsBlue[7] && bIsBlue[1] && bIsBlue[4]) || (bIsBlue[7] && bIsBlue[8] && bIsBlue[6]))
			|| ((bIsRed[7] && bIsRed[1] && bIsRed[4]) || (bIsRed[7] && bIsRed[8] && bIsRed[6])))
		{
			GameState = Won;
			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
		}
	}
}

void ATrePaaRadPawn::KeyNinePressed()
{
	if (GameState == EGameState::Playing)
	{
		TurnControllChecker(8);
		if (((bIsBlue[8] && bIsBlue[5] && bIsBlue[2]) || (bIsBlue[8] && bIsBlue[6] && bIsBlue[7]) || (bIsBlue[8] && bIsBlue[4] && bIsBlue[0]))
			|| ((bIsRed[8] && bIsRed[5] && bIsRed[2]) || (bIsRed[8] && bIsRed[6] && bIsRed[7]) || (bIsRed[8] && bIsRed[4] && bIsRed[0])))
		{
			GameState = Won;
			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
		}
	}
}

//void ATrePaaRadPawn::KeyOnePressed()
//{
//	if (GameState == EGameState::Playing)
//	{
//		TurnControllChecker(0);
//		if ((bIsBlue[0] && bIsBlue[1] && bIsBlue[2]) || (bIsBlue[0] && bIsBlue[3] && bIsBlue[6]) || (bIsBlue[0] && bIsBlue[4] && bIsBlue[8]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//		if ((bIsRed[0] && bIsRed[1] && bIsRed[2]) || (bIsRed[0] && bIsRed[3] && bIsRed[6]) || (bIsRed[0] && bIsRed[4] && bIsRed[8]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//	}
//}
//
//void ATrePaaRadPawn::KeyTwoPressed()
//{
//	if (GameState == EGameState::Playing)
//	{
//		TurnControllChecker(1);
//		if ((bIsBlue[1] && bIsBlue[0] && bIsBlue[2]) || (bIsBlue[1] && bIsBlue[4] && bIsBlue[7]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//		if ((bIsRed[1] && bIsRed[0] && bIsRed[2]) || (bIsRed[1] && bIsRed[4] && bIsRed[7]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//	}
//}
//
//void ATrePaaRadPawn::KeyThreePressed()
//{
//	if (GameState == EGameState::Playing)
//	{
//		TurnControllChecker(2);
//		if ((bIsBlue[2] && bIsBlue[1] && bIsBlue[0]) || (bIsBlue[2] && bIsBlue[4] && bIsBlue[6]) || (bIsBlue[2] && bIsBlue[5] && bIsBlue[8]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//		if ((bIsRed[2] && bIsRed[1] && bIsRed[0]) || (bIsRed[2] && bIsRed[4] && bIsRed[6]) || (bIsRed[2] && bIsRed[5] && bIsRed[8]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//	}
//}
//
//void ATrePaaRadPawn::KeyFourPressed()
//{
//	if (GameState == EGameState::Playing)
//	{
//		TurnControllChecker(3);
//		if ((bIsBlue[3] && bIsBlue[0] && bIsBlue[6]) || (bIsBlue[3] && bIsBlue[4] && bIsBlue[5]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//		if ((bIsRed[3] && bIsRed[0] && bIsRed[6]) || (bIsRed[3] && bIsRed[4] && bIsRed[5]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//	}
//}
//
//void ATrePaaRadPawn::KeyFivePressed()
//{
//	if (GameState == EGameState::Playing)
//	{
//		TurnControllChecker(4);
//		if ((bIsBlue[4] && bIsBlue[1] && bIsBlue[7]) || (bIsBlue[4] && bIsBlue[3] && bIsBlue[5]) || (bIsBlue[4] && bIsBlue[0] && bIsBlue[8]) || (bIsBlue[4] && bIsBlue[2] && bIsBlue[6]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//		if ((bIsRed[4] && bIsRed[1] && bIsRed[7]) || (bIsRed[4] && bIsRed[3] && bIsRed[5]) || (bIsRed[4] && bIsRed[0] && bIsRed[8]) || (bIsRed[4] && bIsRed[2] && bIsRed[6]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//	}
//}
//
//void ATrePaaRadPawn::KeySixPressed()
//{
//	if (GameState == EGameState::Playing)
//	{
//		TurnControllChecker(5);
//		if ((bIsBlue[5] && bIsBlue[2] && bIsBlue[8]) || (bIsBlue[5] && bIsBlue[3] && bIsBlue[4]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//		if ((bIsRed[5] && bIsRed[2] && bIsRed[8]) || (bIsRed[5] && bIsRed[3] && bIsRed[4]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//	}
//}
//
//void ATrePaaRadPawn::KeySevenPressed()
//{
//	if (GameState == EGameState::Playing)
//	{
//		TurnControllChecker(6);
//		if ((bIsBlue[6] && bIsBlue[3] && bIsBlue[0]) || (bIsBlue[6] && bIsBlue[7] && bIsBlue[8]) || (bIsBlue[6] && bIsBlue[4] && bIsBlue[2]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//		if ((bIsRed[6] && bIsRed[3] && bIsRed[0]) || (bIsRed[6] && bIsRed[7] && bIsRed[8]) || (bIsRed[6] && bIsRed[4] && bIsRed[2]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//	}
//}
//
//void ATrePaaRadPawn::KeyEightPressed()
//{
//	if (GameState == EGameState::Playing)
//	{
//		TurnControllChecker(7);
//		if ((bIsBlue[7] && bIsBlue[1] && bIsBlue[4]) || (bIsBlue[7] && bIsBlue[8] && bIsBlue[6]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//		if ((bIsRed[7] && bIsRed[1] && bIsRed[4]) || (bIsRed[7] && bIsRed[8] && bIsRed[6]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//	}
//}
//
//void ATrePaaRadPawn::KeyNinePressed()
//{
//	if (GameState == EGameState::Playing)
//	{
//		TurnControllChecker(8);
//		if ((bIsBlue[8] && bIsBlue[5] && bIsBlue[2]) || (bIsBlue[8] && bIsBlue[6] && bIsBlue[7]) || (bIsBlue[8] && bIsBlue[4] && bIsBlue[0]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//		if ((bIsRed[8] && bIsRed[5] && bIsRed[2]) || (bIsRed[8] && bIsRed[6] && bIsRed[7]) || (bIsRed[8] && bIsRed[4] && bIsRed[0]))
//		{
//			GameState = Won;
//			UE_LOG(LogTemp, Warning, TEXT("WON!!!"));
//		}
//	}
//}