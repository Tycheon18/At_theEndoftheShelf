// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "../LibraryStruct.h"
#include "BookSearchWidget.generated.h"


/**
 * 
 */
UCLASS()
class AT_THEENDOFTHESHELF_API UBookSearchWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* ResultList;

	UPROPERTY(meta = (BindWidget))
	class UButton* SearchButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* SearchInput;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LoadingText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultCountText;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ResultsScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UBookItemWidget> BookItemWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	class UBookSearchService* BookSearchService;

protected:

	UFUNCTION()
	void OnSearchButtonClicked();

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnSearchCompleted(const FSearchResult& Result);

	UFUNCTION()
	void OnSearchFailed(const FString& ErrorMessage);

	void SetLoadingState(bool bIsLoading);
	void ClearResults();
	void DisplayResults(const TArray<FBookInfo>& BookInfos);
	
	bool bIsSearching = false;
};
