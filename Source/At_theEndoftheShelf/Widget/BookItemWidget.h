// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "HttpModule.h"
#include "../LibraryStruct.h"
#include "BookItemWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBookItemClicked, const FBookInfo&, BookInfo);
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

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBookItemClicked OnBookItemClicked;
protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AuthorsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PublisherText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ThumbnailImage;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemButton;

	UFUNCTION()
	void OnImageDownloaded(const FString& ImageUrl, UTexture2D* Texture);

	class UNetworkManager* NetworkManager;
	FBookInfo CurrentBookInfo;
	FString CurrentImageUrl;

	UFUNCTION()
	void OnItemButtonClicked();

};
