// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LibraryStruct.h"
#include "Blueprint/UserWidget.h"
#include "BookReviewWidget.generated.h"

class UBookReviewManager;
class UBookSearchManager;

/**
 * 
 */
UCLASS()
class AT_THEENDOFTHESHELF_API UBookReviewWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Book Review")
	void OnSearchButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Book Review")
	void OnSubmitReview();

protected:

	UPROPERTY(meta = (BindWidget))
	class UButton* SearchButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* SerachTextBox;

	UPROPERTY(meta = (BindWidget))
	class UListView* SearchResultsList;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ReviewTitleBox;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ReviewContentBox;

	UPROPERTY(meta = (BindWidget))
	UButton* SubmitReviewButton;

	UFUNCTION()
	void HandleSearchResults(const TArray<FBookInfo>& Results);

	UFUNCTION()
	void HandleReviewUploaded(bool bSuccess);

	UFUNCTION()
	void HandleBookItemClicked(UObject* Item);

	UPROPERTY()
	UBookSearchManager* SearchManager;

	UPROPERTY()
	UBookReviewManager* ReviewManager;

	FBookInfo CurrentBook;
};
