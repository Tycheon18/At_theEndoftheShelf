// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AES_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AT_THEENDOFTHESHELF_API UAES_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	virtual void Init() override;

	virtual void Shutdown() override;

	virtual void StartGameInstance() override;

	virtual void OnStart() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBookSearchService* BookSearchService;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UNetworkManager* NetworkManager;

public:

	FORCEINLINE UBookSearchService* GetBookSearchService() const { return BookSearchService; }
	
	FORCEINLINE UNetworkManager* GetNetworkManager() const { return NetworkManager; }
};
