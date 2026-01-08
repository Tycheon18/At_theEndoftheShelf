// Fill out your copyright notice in the Description page of Project Settings.


#include "BookSearchWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "BookItemWidget.h"
#include "../NetworkManager.h"
#include "../AES_GameInstance.h"
#include "../BookshelfManager.h"
#include "../Service/BookSearchService.h"
#include "../Actor/BookActor.h"


void UBookSearchWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!BookSearchService)
	{
		BookSearchService = Cast<UAES_GameInstance>(GetWorld()->GetGameInstance())->GetBookSearchService();
	}

	BookSearchService->OnSearchCompleted.AddDynamic(this, &UBookSearchWidget::OnSearchCompleted);
	BookSearchService->OnSearchFailed.AddDynamic(this, &UBookSearchWidget::OnSearchFailed);

	if(SearchButton)
	{
		SearchButton->OnClicked.AddDynamic(this, &UBookSearchWidget::OnSearchButtonClicked);
	}

	if(SearchInput)
	{
		SearchInput->OnTextCommitted.AddDynamic(this, &UBookSearchWidget::OnTextCommitted);
	}

	if (PreviousPageButton)
	{
		PreviousPageButton->OnClicked.AddDynamic(this, &UBookSearchWidget::OnSearchPreviousPage);
	}

	if (NextPageButton)
	{
		NextPageButton->OnClicked.AddDynamic(this, &UBookSearchWidget::OnSearchNextPage);
	}

	if (PaginationPanel)
	{
		PaginationPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	UWorld* World = GetWorld();
	if (World)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, ABookshelfManager::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0)
		{
			BookshelfManager = Cast<ABookshelfManager>(FoundActors[0]);
			UE_LOG(LogTemp, Log, TEXT("BookSearchWidget: Found BookshelfManager"));
		}
	}

}

void UBookSearchWidget::OnSearchButtonClicked()
{
	if (bIsSearching) return;

	FString SearchQuery = SearchInput->GetText().ToString();
	if (SearchQuery.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Search query is empty"));
		return;
	}

	SetLoadingState(true);
	ClearResults();
	BookSearchService->SearchBooks(SearchQuery, ESearchCategory::All, 1, 50);
}

void UBookSearchWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		OnSearchButtonClicked();
	}
}

void UBookSearchWidget::OnSearchCompleted(const FSearchResult& Result)
{
	SetLoadingState(false);

	// Update result count
	if (Result.Books.Num() == 0)
	{
		if (LoadingText)
		{
			LoadingText->SetText(FText::FromString(TEXT("검색 결과가 없습니다.")));
			LoadingText->SetVisibility(ESlateVisibility::Visible);
		}

		if (PaginationPanel)
		{
			PaginationPanel->SetVisibility(ESlateVisibility::Collapsed);
		}
		return;
	}

	if (BookshelfManager)
	{
		BookshelfManager->ClearAllBooks();

		for (const FBookInfo& Book : Result.Books)
		{
			BookshelfManager->AddBook(Book);
		}

		UE_LOG(LogTemp, Log, TEXT("BookSearchWidget: Added %d books to bookshelf"),
			Result.Books.Num());
	}


	if(ResultCountText)
	{
		FString CountText;

		if (Result.TotalCount > 0 && Result.TotalCount > Result.Books.Num())
		{
			CountText = FString::Printf(
				TEXT("%d개 표시 중 (전체 %d개)"),
				Result.Books.Num(),
				Result.TotalCount
			);
		}
		else
		{
			CountText = FString::Printf(TEXT("총 %d개 결과"), Result.Books.Num());
		}

		ResultCountText->SetText(FText::FromString(CountText));
		ResultCountText->SetVisibility(ESlateVisibility::Visible);
	}

	AllSearchResults = Result.Books;
	CurrentSearchPage = 0;
	TotalSearchPages = FMath::CeilToInt((float)AllSearchResults.Num() / (float)ResultsPerPage);

	//DisplayResults(AllSearchResults);
	DisplayCurrentSearchPage();

	if (PaginationPanel)
	{
		PaginationPanel->SetVisibility(ESlateVisibility::Visible);
		UpdatePaginationUI();
	}
}

void UBookSearchWidget::OnSearchFailed(const FString& ErrorMessage)
{
	SetLoadingState(false);

	UE_LOG(LogTemp, Error, TEXT("Search failed: %s"), *ErrorMessage);

	// Show error message to user
	if (LoadingText)
	{
		LoadingText->SetText(FText::FromString(TEXT("검색 실패. 다시 시도해주세요.")));
		LoadingText->SetVisibility(ESlateVisibility::Visible);
	}

	if (PaginationPanel)
	{
		PaginationPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBookSearchWidget::SetLoadingState(bool bIsLoading)
{
	bIsSearching = bIsLoading;

	if (SearchButton)
	{
		SearchButton->SetIsEnabled(!bIsLoading);
	}

	if (LoadingText)
	{
		if (bIsLoading)
		{
			LoadingText->SetText(FText::FromString(TEXT("검색 중...")));
			LoadingText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			LoadingText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UBookSearchWidget::ClearResults()
{
	if (ResultsScrollBox)
	{
		ResultsScrollBox->ClearChildren();
	}

	if (ResultCountText)
	{
		ResultCountText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (PaginationPanel)
	{
		PaginationPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBookSearchWidget::DisplayResults(const TArray<FBookInfo>& BookInfos)
{
	if (!ResultsScrollBox || !BookItemWidgetClass) return;

	ResultsScrollBox->ClearChildren();

	UE_LOG(LogTemp, Warning, TEXT("Displaying results for page %d"), CurrentSearchPage);

	int32 StartIndex = CurrentSearchPage * ResultsPerPage;
	int32 EndIndex = FMath::Min(StartIndex + ResultsPerPage, AllSearchResults.Num());

	for (int32 i = StartIndex; i < EndIndex; i++)
	{
		const FBookInfo& Book = BookInfos[i];

		UBookItemWidget* BookItem = CreateWidget<UBookItemWidget>(GetWorld(), BookItemWidgetClass);
		if (BookItem)
		{
			BookItem->SetBookData(Book);
			BookItem->OnBookItemClicked.AddDynamic(this, &UBookSearchWidget::OnBookItemClicked);
			ResultsScrollBox->AddChild(BookItem);
		}
	}
}

void UBookSearchWidget::DisplayCurrentSearchPage()
{
	if (!ResultsScrollBox || !BookItemWidgetClass) return;

	ResultsScrollBox->ClearChildren();

	int32 StartIndex = CurrentSearchPage * ResultsPerPage;
	int32 EndIndex = FMath::Min(StartIndex + ResultsPerPage, AllSearchResults.Num());

	for (int32 i = StartIndex; i < EndIndex; i++)
	{
		const FBookInfo& Book = AllSearchResults[i];

		UBookItemWidget* BookItem = CreateWidget<UBookItemWidget>(GetWorld(), BookItemWidgetClass);
		if (BookItem)
		{
			BookItem->SetBookData(Book);
			BookItem->OnBookItemClicked.AddDynamic(this, &UBookSearchWidget::OnBookItemClicked);
			ResultsScrollBox->AddChild(BookItem);
		}
	}
}

void UBookSearchWidget::OnBookItemClicked(const FBookInfo& BookInfo)
{
	UE_LOG(LogTemp, Log, TEXT("BookSearchWidget: Adding book to shelf - %s"), *BookInfo.Title);

	if (BookshelfManager)
	{
		BookshelfManager->AddBook(BookInfo);

		UpdatePaginationUI();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BookshelfManager not found in level!"));
	}
}

void UBookSearchWidget::OnPreviousPageClicked()
{
	if (BookshelfManager)
	{
		BookshelfManager->PreviousPage();
		UpdatePaginationUI();
	}
}

void UBookSearchWidget::OnNextPageClicked()
{
	if (BookshelfManager)
	{
		BookshelfManager->NextPage();
		UpdatePaginationUI();
	}
}

void UBookSearchWidget::UpdatePaginationUI()
{
	if (!BookshelfManager)
	{
		return;
	}

	int32 CurrentPage = BookshelfManager->GetCurrentPage();
	int32 TotalPages = BookshelfManager->GetTotalPages();

	if (PageInfoText)
	{
		if (TotalPages > 0)
		{
			FString PageInfo = FString::Printf(TEXT("페이지 %d / %d"), CurrentSearchPage + 1, TotalSearchPages);
			PageInfoText->SetText(FText::FromString(PageInfo));
		}
		else
		{
			PageInfoText->SetText(FText::FromString(TEXT("페이지 0 / 0")));
		}
	}

	if (PreviousPageButton)
	{
		PreviousPageButton->SetIsEnabled(CurrentSearchPage > 0);
	}

	if (NextPageButton)
	{
		NextPageButton->SetIsEnabled(CurrentSearchPage < TotalSearchPages - 1);
	}

	UE_LOG(LogTemp, Log, TEXT("Pagination UI updated: Page %d/%d"), CurrentSearchPage + 1, TotalSearchPages);
}

void UBookSearchWidget::OnSearchPreviousPage()
{
	UE_LOG(LogTemp, Log, TEXT("OnSearchNextPage called: CurrentPage=%d, TotalPages=%d"),
		CurrentSearchPage, TotalSearchPages);

	if (CurrentSearchPage > 0)
	{
		CurrentSearchPage--;
		//DisplayResults(AllSearchResults);
		DisplayCurrentSearchPage();
		UpdatePaginationUI();
	}
}

void UBookSearchWidget::OnSearchNextPage()
{
	UE_LOG(LogTemp, Log, TEXT("OnSearchNextPage called: CurrentPage=%d, TotalPages=%d"),
		CurrentSearchPage, TotalSearchPages);

	if (CurrentSearchPage < TotalSearchPages - 1)
	{
		CurrentSearchPage++;
		//DisplayResults(AllSearchResults);
		DisplayCurrentSearchPage();
		UpdatePaginationUI();
	}
}

