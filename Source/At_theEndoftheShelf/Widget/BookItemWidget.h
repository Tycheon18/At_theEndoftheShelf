// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "HttpModule.h"
#include "../LibraryStruct.h"
#include "BookItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class AT_THEENDOFTHESHELF_API UBookItemWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Book Item")
	void SetBookData(const FBookInfo& InBookInfo);

protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AuthorsText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PublisherText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ThumbnailImage;

	UFUNCTION()
	void OnImageDownloaded(const FString& ImageUrl, UTexture2D* Texture);

	class UNetworkManager* NetworkManager;

	FString CurrentImageUrl;
};
