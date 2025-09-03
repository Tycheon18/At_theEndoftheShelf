// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AES_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AT_THEENDOFTHESHELF_API AAES_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

protected:

	UFUNCTION(BlueprintCallable, Category = "Initialize")
	void InitializeBookServices();
};
