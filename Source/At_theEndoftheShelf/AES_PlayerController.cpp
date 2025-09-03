// Fill out your copyright notice in the Description page of Project Settings.


#include "AES_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "AES_GameInstance.h"
#include "Service/BookSearchService.h"

void AAES_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	//InitializeBookServices();
}

void AAES_PlayerController::InitializeBookServices()
{
	UAES_GameInstance* GameInstance = Cast<UAES_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if(GameInstance && GameInstance->GetBookSearchService() && GameInstance->GetNetworkManager())
	{
		GameInstance->GetBookSearchService()->SetApiKey(TEXT("e1989147065390deb1db9d93a2668e86"));
		GameInstance->GetBookSearchService()->SetNetworkManager(GameInstance->GetNetworkManager());
	}
}
