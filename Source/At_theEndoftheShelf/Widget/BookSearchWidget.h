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

	UPROPERTY(meta = (BindWidget))
	UButton* PreviousPageButton;

	UPROPERTY(meta = (BindWidget))
	UButton* NextPageButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PageInfoText;

	UPROPERTY(meta = (BindWidget))
	UWidget* PaginationPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UBookItemWidget> BookItemWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	class UBookSearchService* BookSearchService;

	UPROPERTY()
	class ABookshelfManager* BookshelfManager;

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
	
	// DisplayResults has been replaced with the corresponding function.
	void DisplayCurrentSearchPage();

	bool bIsSearching = false;

public:

	UPROPERTY(EditAnywhere, Category = "3D")
	TSubclassOf<class ABookActor> BookActorClass;

	UPROPERTY(EditAnywhere, Category = "3D")
	FVector SpawnLocation = FVector(1000, 1000, 100);

	UPROPERTY(EditAnywhere, Category = "3D")
	float SpawnSpacing = 150.f;

protected:

	UFUNCTION()
	void OnBookItemClicked(const FBookInfo& BookInfo);

	int32 SpawnedBookCount = 0;

	UPROPERTY()
	TArray<FBookInfo> AllSearchResults;
	
	UPROPERTY()
	int32 CurrentSearchPage = 0;

	UPROPERTY()
	int32 TotalSearchPages = 0;

	UPROPERTY()
	int32 ResultsPerPage = 10;

	UFUNCTION()
	void OnPreviousPageClicked();

	UFUNCTION()
	void OnNextPageClicked();

	UFUNCTION()
	void UpdatePaginationUI();

	//UFUNCTION()
	//void UpdateSearchPagination();

	//UFUNCTION()
	//void DisplayCurrentSearchPage();

	UFUNCTION()
	void OnSearchPreviousPage();

	UFUNCTION()
	void OnSearchNextPage();

};
