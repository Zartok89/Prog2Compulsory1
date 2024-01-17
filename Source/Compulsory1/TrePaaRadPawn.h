// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "UObject/Class.h"
#include "Sound/SoundCue.h"
#include "TrePaaRadPawn.generated.h"

//--ForwardClasses--//
class UStaticMeshComponent;
class UMaterial;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UAudioComponent;
class USoundCue;

//--GameStates--//
UENUM(BlueprintType)
enum EGameState
{
	Playing,
	Won
};

UCLASS()
class COMPULSORY1_API ATrePaaRadPawn : public APawn
{
	GENERATED_BODY()

public:
	ATrePaaRadPawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//--Generators--//
	void GenerateCameraAndArm();
	void GenerateSphereMeshes();
	void GenerateSphereMaterials();
	void SetSpherePositions();
	void GenerateAudioComponent();
	void PlayAudioComponent();

	//--GameCheckers--//
	EGameState GameState = EGameState::Playing;
	void TurnControllChecker(int32 SphereIndex);
	int TurnCounter;
	TArray<bool> bIsSphereColorChanged;
	TArray<bool> bIsRed;
	TArray<bool> bIsBlue;
	void SetCurrentGameState(EGameState NewState);

	//--KeyPresses--//
	void KeyOnePressed();
	void KeyTwoPressed();
	void KeyThreePressed();
	void KeyFourPressed();
	void KeyFivePressed();
	void KeySixPressed();
	void KeySevenPressed();
	void KeyEightPressed();
	void KeyNinePressed();

	//--Audio--//
	USoundCue* AudioCue;


	UPROPERTY(EditAnywhere)
	TArray<UStaticMeshComponent*> SphereMeshArray;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* CameraArm;

	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		UAudioComponent* AudioComponent;

	//--KeyActionAndControllers--//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* ButtonOneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* ButtonTwoAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* ButtonThreeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* ButtonFourAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* ButtonFiveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* ButtonSixAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* ButtonSevenAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* ButtonEightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		UInputAction* ButtonNineAction;

private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterial* DefaultMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInstance* Red;

	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInstance* Blue;

	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInstance* ColorWon;

	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInstance* ColorDraw;
	
};

