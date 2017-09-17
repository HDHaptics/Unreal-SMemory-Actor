// Fill out your copyright notice in the Description page of Project Settings.
// Created by David Seo at Postech
// 20170720

#pragma once

#include <Windows.h>
#include "CoreMinimal.h"
#include <EngineGlobals.h>
#include "EngineUtils.h"
#include "Components/SphereComponent.h"
#include "ConstructorHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/EditorFramework/AssetImportData.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "GameFramework/Actor.h"
#include "SMemory.generated.h"


#define SCREENMSG(param1) (GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (param1)))

UENUM(BlueprintType)
enum ESMBraches
{
	Connected,
	Failed
};

USTRUCT(BlueprintType)
struct FSharedMemoryInfoData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int systemGranularity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float posX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float posY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float posZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int numberOfView;


	FSharedMemoryInfoData() {
		systemGranularity = 0;

		posX = 0;
		posY = 0;
		posZ = 0;

		numberOfView = 0;
	}
};

USTRUCT(BlueprintType)
struct FSceneObjectData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int objtag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString filename;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AStaticMeshActor* actorPtr;


	FSceneObjectData() {
		objtag = 0;
		path = FString("");
		filename = FString("");
		actorPtr = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FObjectConfiguration
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int objTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float objposX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float objposY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float objposZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float objrotX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float objrotY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float objrotZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float objScaleX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float objScaleY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float objScaleZ;

	FObjectConfiguration() {
		objTag = 0;
		objposX = 0;
		objposY = 0;
		objposZ = 0;
		objrotX = 0;
		objrotY = 0;
		objrotZ = 0;
		objScaleX = 0;
		objScaleY = 0;
		objScaleZ = 0;
	}

};

USTRUCT(BlueprintType)
struct FObjectEdit
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool command;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int objectNumTag;

		char filename[200];

	FObjectEdit() {
		command = 0;
		objectNumTag = 1;
	}

};

USTRUCT(BlueprintType)
struct FObjectNumInView
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int objectConfNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int verticesNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int objectEditNum;

	FObjectNumInView() {
		objectConfNum = 0;
		verticesNum = 0;
		objectEditNum = 0;
	}
};


UCLASS()
class HDHAPTICSUNREAL_API ASMemory : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ASMemory();

	UFUNCTION(BlueprintCallable, Category = "SharedMemory", Meta = (ExpandEnumAsExecs = "Branches"))
		bool CreateSharedMemory(FString address, TEnumAsByte<ESMBraches>& Branches);
	UFUNCTION(BlueprintCallable, Category = "SharedMemory", Meta = (ExpandEnumAsExecs = "Branches"))
		bool ConnectSharedMemory(FString address, TEnumAsByte<ESMBraches>& Branches);
	UFUNCTION(BlueprintCallable, Category = "SharedMemory")
		FVector getHIPConfiguration();
	UFUNCTION(BlueprintCallable, Category = "HDHaptics")
		bool getMeshPath(FString& fileOut);
	UFUNCTION(BlueprintCallable, Category = "HDHaptics")
		bool getMeshFiles(TArray<FString>& ArrayFileName);
	UFUNCTION(BlueprintCallable, Category = "HDHaptics")
		bool getObjectListInWorld(TArray<FSceneObjectData>& ArraySceneObjData);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//Status
	static int const SMUNCONNECTED = 0;
	static int const SMCONNECTED = 1;

	//CHANGE!!
	static const int CREATION = 1;
	static const int DELETION = 0;

	//SystemGranularity
	int systemGranularity;

	//current status of Shared Memory Communication
	int currentStatus;

	//The number of Objects Created or Deleted;
	int objNumCreated = 0;
	int objNumDeleted = 0;
	int lastObjIndex = 0;

	//Shared Memory
	HANDLE handleFile;
	LPBYTE infoView;
	LPBYTE memoryView[10] = {};

	FSharedMemoryInfoData *infoData;

	USphereComponent* SphereHIP;
	UStaticMeshComponent* VisualHIP;

	//String Arrays of Objects In Map
	TArray<FSceneObjectData> ArraySceneObjectData;
	TArray<FObjectEdit> EditListQueue;
	//String Arrays of Objects In GameEngine
	TArray<FString> ArrayFileList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWorld* mWorld;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};


