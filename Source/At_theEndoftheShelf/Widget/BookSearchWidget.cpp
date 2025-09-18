// Fill out your copyright notice in the Description page of Project Settings.


#include "BookSearchWidget.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "BookItemWidget.h"
#include "../AES_GameInstance.h"
#include "../Service/BookSearchService.h"

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

	SetLoadingState(false);

	if (ResultCountText)
	{
		ResultCountText->SetVisibility(ESlateVisibility::Collapsed);
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
	BookSearchService->SearchBooks(SearchQuery, ESearchCategory::All, 1, 10);
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
		return;
	}

	if(ResultCountText)
	{
		FString CountText = FString::Printf(TEXT("총 %d개 결과"), Result.Books.Num());
		ResultCountText->SetText(FText::FromString(CountText));
		ResultCountText->SetVisibility(ESlateVisibility::Visible);
	}

	DisplayResults(Result.Books);

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
}

void UBookSearchWidget::DisplayResults(const TArray<FBookInfo>& BookInfos)
{
	if (!ResultsScrollBox || !BookItemWidgetClass) return;

	for (const FBookInfo& BookInfo : BookInfos)
	{
		UBookItemWidget* BookItem = CreateWidget<UBookItemWidget>(GetWorld(), BookItemWidgetClass);
		if (BookItem)
		{
			BookItem->SetBookData(BookInfo);
			ResultsScrollBox->AddChild(BookItem);
		}
	}
}
