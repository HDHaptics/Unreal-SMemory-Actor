// Fill out your copyright notice in the Description page of Project Settings.

#include "SMemory.h"

// Sets default values
ASMemory::ASMemory()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	currentStatus = 0;

	//Find system Allocation granularity
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	systemGranularity = sys_info.dwAllocationGranularity;


#pragma region HIPVisualization

	//Add a HIP Sphere Collision as RootComponent
	SphereHIP = CreateDefaultSubobject<USphereComponent>(TEXT("HIP"));
	RootComponent = SphereHIP;
	SphereHIP->InitSphereRadius(1.0f);
	SphereHIP->SetWorldLocation(FVector(0.0f, 0.0f, 0.0f));
	SphereHIP->SetCollisionProfileName(TEXT("NoCollision"));

	//Make a Sphere for visualization of HIP
	VisualHIP = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	VisualHIP->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Engine/BasicShapes/Sphere"));
	ConstructorHelpers::FObjectFinder<UMaterial> SphereMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));

	if (SphereVisualAsset.Succeeded() && SphereMaterial.Succeeded())
	{
		VisualHIP->SetStaticMesh(SphereVisualAsset.Object);
		VisualHIP->SetMaterial(0, SphereMaterial.Object);
		VisualHIP->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		VisualHIP->SetWorldScale3D(FVector(0.8f));
	}

#pragma endregion



}

// Called when the game starts or when spawned
void ASMemory::BeginPlay()
{
	Super::BeginPlay();

	EditListQueue = TArray<FObjectEdit>();

	FString meshPath;

	if (getMeshPath(meshPath)) {
		getMeshFiles(ArrayFileList);
	}

	if (getObjectListInWorld(ArraySceneObjectData)) {

		for (FSceneObjectData objData : ArraySceneObjectData) {
			//objNumCreated++;
			FObjectEdit mObjEdit = FObjectEdit();
			mObjEdit.command = 1;
			mObjEdit.objectNumTag = objData.objtag;
			strcpy(mObjEdit.filename, TCHAR_TO_ANSI(*(objData.path)));

			EditListQueue.Add(mObjEdit);
		}

		return;
	}
}


// Called every frame
void ASMemory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (currentStatus == SMCONNECTED) {

#pragma region InfoDataParsing

		infoData = (FSharedMemoryInfoData *)infoView;
		SphereHIP->SetWorldLocation(FVector(infoData->posX, infoData->posY, infoData->posZ));

#pragma endregion

#pragma region SharedMemoryLoadData

		//FObjectNumInView* mObjectNumInView;

		for (int i = 0; i < 1; i++) { ////////////////CHANGE!!!! ////////////
			
			if (memoryView[i] != NULL)
			{
				/*
				//Read the number of Data
				mObjectNumInView = (FObjectNumInView *)memoryView[i];

				if (mObjectNumInView == NULL) {
					SCREENMSG(TEXT("SharedMemory - Communication Error (View)"));
					currentStatus = SMUNCONNECTED;
					return;
				}

				//Reading 0 means Communication error
				if ( !(mObjectNumInView->objectConfNum || mObjectNumInView->objectEditNum || mObjectNumInView->verticesNum ) ) {
					SCREENMSG(TEXT("SharedMemory - Communication Error (Number Data)"));
					currentStatus = SMUNCONNECTED;
					return;
				}

				int address = (int)sizeof(FObjectNumInView);

				//TCHAR buf[100];
				//swprintf_s(buf, sizeof(buf), _T("Obj Num  : %d"), *objNumber);
				//SCREENMSG(buf);

				//Get Object Configuration Data along to object Number;
				for (int j = 0; j < mObjectNumInView->objectConfNum; j++) {
					FObjectConfiguration* objectConfigurationData = (FObjectConfiguration *)(memoryView[i] + address);
				
					//Applying new object configuration data from bridge program
					
					TCHAR buf[100];
					swprintf_s(buf, sizeof(buf), _T("Obj Conf[%d] : [%f,  %f,  %f]"), 
						j, objectConfigurationData->objposX,
						objectConfigurationData->objposY,
						objectConfigurationData->objposZ);
					SCREENMSG(buf);
					

					//Go to Next if there are next data
					address += (int)sizeof(FObjectConfiguration);
					
				}

				//Get Object Edit Data along to object Number;


				//Get Vertice Data along to object Number;

				*/
			}
		}

#pragma endregion
	
#pragma region MakingInfoData

		//************************Sequence**********************//
		//******* 1. Detection of object Configuration *********//
		//******* 2. if there were changes, Make infoData  *****//
		//******* 3. Send them                    **************//         
		//******************************************************//

		// 1. Detection of obj Configuration

		// 2. Making Info Data
		FSharedMemoryInfoData InfoDataToSend = FSharedMemoryInfoData();
		InfoDataToSend.posX = 0;
		InfoDataToSend.posY = 0;
		InfoDataToSend.posZ = 0;

		//Calculation of the size of data to send - will be added
		InfoDataToSend.numberOfView = 2;
		InfoDataToSend.systemGranularity = 65536;

		FObjectNumInView mObjectNumInView2 = FObjectNumInView();

		mObjectNumInView2.objectConfNum = ArraySceneObjectData.Num();// 1;
		mObjectNumInView2.verticesNum = 0;
		mObjectNumInView2.objectEditNum = EditListQueue.Num(); //objNumCreated + objNumDeleted;

		//nothing to send
		/*
		if (objectNumToSend <= 0) {
			return; 
		}
		*/

		/*
		TCHAR buf[100];
		swprintf_s(buf, sizeof(buf), _T("Cube Location : [%f,  %f,  %f]"),
			temppos.X, temppos.Y, temppos.Z);
		SCREENMSG(buf);
		*/

		// 3. Send Them - InfoView
		CopyMemory(infoView, &InfoDataToSend, sizeof(FSharedMemoryInfoData));
		_gettch();

		// 3. Send Them - memoryView

		//Change this part later
		//CopyMemory(memoryView[0], &InfoDataToSend, sizeof(int));
		//_gettch();

		int address = (int)sizeof(FObjectNumInView);

		CopyMemory(memoryView[0], &mObjectNumInView2, sizeof(FObjectNumInView));
		_gettch();

		//TEST!
		//getObjectListInWorld(ArraySceneObjectData);

		//Send object Creation / Deletion Data
		for (int j = 0; j < mObjectNumInView2.objectEditNum; j++) {

			FObjectEdit mObjectEdit = EditListQueue.Pop();

			CopyMemory(memoryView[0] + address, &(mObjectEdit), sizeof(FObjectEdit));
			_gettch();

			address += (int)sizeof(FObjectEdit);

		}

		address = (int)sizeof(FObjectNumInView);

		CopyMemory(memoryView[1], &mObjectNumInView2, sizeof(FObjectNumInView));
		_gettch();

		for (FSceneObjectData mSceneObjData : ArraySceneObjectData)
		{
			AStaticMeshActor* mesh = mSceneObjData.actorPtr;

			FVector objPosVector = mesh->GetActorLocation();
			FRotator objRotator = mesh->GetActorRotation();
			FVector objScale = mesh->GetActorScale();


			FObjectConfiguration objectConfiguration = FObjectConfiguration();
			objectConfiguration.objTag = mSceneObjData.objtag;
			objectConfiguration.objposX = objPosVector.X;
			objectConfiguration.objposY = objPosVector.Y;
			objectConfiguration.objposZ = objPosVector.Z;
			objectConfiguration.objrotX = objRotator.Roll;
			objectConfiguration.objrotY = objRotator.Pitch;
			objectConfiguration.objrotZ = objRotator.Yaw;
			objectConfiguration.objScaleX = objScale.X;
			objectConfiguration.objScaleY = objScale.Y;
			objectConfiguration.objScaleZ = objScale.Z;

			TCHAR buf[100];
			swprintf_s(buf, sizeof(buf), _T("Data info : [%d,  %f, %d]"),
				objectConfiguration.objTag, objectConfiguration.objScaleX, address);
			SCREENMSG(buf);

			CopyMemory(memoryView[1] + address, &objectConfiguration, sizeof(FObjectConfiguration));
			_gettch();


			address += sizeof(FObjectConfiguration);
		}

		SCREENMSG(TEXT("HI"));

#pragma endregion

	}

}

bool ASMemory::CreateSharedMemory(FString address, TEnumAsByte<ESMBraches>& Branches) {

	//If shared memory already connected, kill it
	if (handleFile) {
		CloseHandle(handleFile);
	}
	if (memoryView[0]) {
		memoryView[0] = NULL;
	}

#pragma region CreateSharedMemoryConnection

	//create shared memory
	const TCHAR* strAddress = *address;

	handleFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, strAddress);
	if (handleFile == NULL) {
		SCREENMSG(TEXT("CreateSharedMemory - Handle file missing"));
		currentStatus = SMUNCONNECTED;
		Branches = ESMBraches::Failed;
		return false;
	}

	infoView = (LPBYTE)MapViewOfFile(handleFile, FILE_MAP_ALL_ACCESS, 0, 0, systemGranularity); 
	if (!infoView)
	{
		SCREENMSG(TEXT("CreateSharedMemory - Can't not Find map of views"));
		CloseHandle(handleFile);
		currentStatus = SMUNCONNECTED;
		Branches = ESMBraches::Failed;
		return false;
	}

#pragma endregion

	SCREENMSG(TEXT("CreateSharedMemory - Created!"));
	currentStatus = SMCONNECTED;
	Branches = ESMBraches::Connected;
	return true;


}

bool ASMemory::ConnectSharedMemory(FString address, TEnumAsByte<ESMBraches>& Branches) {

	//If shared memory already connected, kill it
	
	if (handleFile) {
		CloseHandle(handleFile);
	}
	if (infoView) {
		infoView = NULL;
	}
	
#pragma region OpenSharedMemoryConnection

	//connect to shared memory
	const TCHAR* strAddress = *address;

	handleFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, strAddress);
	if (handleFile == NULL) {
		SCREENMSG(TEXT("ConnectSharedMemory - Handle file missing"));
		currentStatus = SMUNCONNECTED;
		Branches = ESMBraches::Failed;
		return false;
	}

	infoView = (LPBYTE)MapViewOfFile(handleFile, FILE_MAP_ALL_ACCESS, 0, 0, systemGranularity);
	if (!infoView)
	{
		SCREENMSG(TEXT("ConnectSharedMemory - Can't not Find InfoMap"));
		CloseHandle(handleFile);
		currentStatus = SMUNCONNECTED;
		Branches = ESMBraches::Failed;
		return false;
	}

#pragma endregion

	infoData = (FSharedMemoryInfoData *)infoView;

#pragma region SharedMemoryLoad

	int cumulatedSize = systemGranularity;
	//FObjectNumInView* mObjectNumInView;

	for (int i = 0; i < 2; i++) { //////////////////////// CHANGE!!!
		memoryView[i] = (LPBYTE) MapViewOfFile(handleFile, FILE_MAP_ALL_ACCESS, 0, cumulatedSize, systemGranularity);
		
		//DEBUGGING
		//TCHAR buf[100];
		//swprintf_s(buf, sizeof(buf), _T("%p"), memoryView[i]);
		//SCREENMSG(buf);
		
		if (memoryView[i] != NULL)
		{
			/*
			//Read the number of Data
			objNumber = (int *)memoryView[i];
			//Reading 0 means Communication error
			if (*objNumber == 0) {
				SCREENMSG(TEXT("ConnectSharedMemory - Communication Error"));
				currentStatus = SMUNCONNECTED;
				Branches = ESMBraches::Failed;
				return false;
			}

			//FOR DEBUGGING
			TCHAR buf[100];
			swprintf_s(buf, sizeof(buf), _T("%d"), *objNumber);
			SCREENMSG(buf);
			*/
		}
		else {
			SCREENMSG(TEXT("ConnectSharedMemory - Can't not Find InfoMap"));
			currentStatus = SMUNCONNECTED;
			Branches = ESMBraches::Failed;
			return false;
		}

		cumulatedSize += systemGranularity;
	}

#pragma endregion

	SCREENMSG(TEXT("ConnectSharedMemory - Connected!"));
	currentStatus = SMCONNECTED;
	Branches = ESMBraches::Connected;
	return true;
}


FVector ASMemory::getHIPConfiguration() {

	FVector HIPVector = FVector((*infoData).posX, (*infoData).posY, (*infoData).posZ);

	return HIPVector;
}

bool ASMemory::getMeshPath(FString& fileOut) {
	
	fileOut = FPaths::GameContentDir();

	fileOut.Append(FString("../../Mesh"));
	
	return true;
}

bool ASMemory::getMeshFiles(TArray<FString>& ArrayFileName) {

	FString mPath;
	FString mExtension = FString("*.obj");

	getMeshPath(mPath);
	FPaths::NormalizeDirectoryName(mPath);

	IFileManager& mFileManager = IFileManager::Get();

	//mFileManager.FindFiles(ArrayFileName, *mPath, NULL);
	mFileManager.FindFilesRecursive(ArrayFileName, *mPath, *mExtension, true, false, false);

	for (FString file : ArrayFileName) {

		SCREENMSG(*(FPaths::GetCleanFilename(file)));

	}


	return true;
}

bool ASMemory::getObjectListInWorld(TArray<FSceneObjectData>& ArraySceneObjData) {


	ArraySceneObjData = TArray<FSceneObjectData>();

	for (TActorIterator<AStaticMeshActor> ActorItr(mWorld); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AStaticMeshActor *Mesh = *ActorItr;

		//get Source file path (.obj, .fbx, .3ds ... etc)
		UAssetImportData* mImportData = Mesh->GetStaticMeshComponent()->GetStaticMesh()->AssetImportData;
		FString meshSourcePath = mImportData->GetFirstFilename();


		if (meshSourcePath.Equals("")) {
			SCREENMSG(TEXT("Primitives or Error"));
		}
		else {

			// if Object was created
			if (true) 
			{				
				FSceneObjectData mData = FSceneObjectData();
				mData.path = meshSourcePath;
				mData.filename = FPaths::GetCleanFilename(meshSourcePath);
				mData.objtag = ++lastObjIndex;
				mData.actorPtr = *ActorItr;

				//ArrayObjRef.Add(*ActorItr);
				ArraySceneObjData.Add(mData);
				SCREENMSG(*meshSourcePath);
			}
			else if (true) // 
			{

			}
		}

		
		//SCREENMSG(*(Mesh->GetName()));

	}

	return true;

}