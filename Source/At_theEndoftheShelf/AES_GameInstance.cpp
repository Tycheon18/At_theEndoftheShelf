// Fill out your copyright notice in the Description page of Project Settings.


#include "AES_GameInstance.h"
#include "Service/BookSearchService.h"
#include "NetworkManager.h"

void UAES_GameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Warning, TEXT("Init is Called"));

	NetworkManager = NewObject<UNetworkManager>(this, UNetworkManager::StaticClass());

	BookSearchService = NewObject<UBookSearchService>(this, UBookSearchService::StaticClass());

}

void UAES_GameInstance::Shutdown()
{
	if (BookSearchService)
	{
		BookSearchService->CleanupDelegates();
	}

	Super::Shutdown();
}

void UAES_GameInstance::StartGameInstance()
{
	Super::StartGameInstance();

	UE_LOG(LogTemp, Warning, TEXT("StartGameInstance is Called"));

}

void UAES_GameInstance::OnStart()
{
	Super::OnStart();

	UE_LOG(LogTemp, Warning, TEXT("OnStart is Called"));

	//if (BookSearchService && NetworkManager)
	//{
	//	BookSearchService->SetApiKey(TEXT("e1989147065390deb1db9d93a2668e86"));
	//	BookSearchService->SetNetworkManager(NetworkManager);
	//}
}
