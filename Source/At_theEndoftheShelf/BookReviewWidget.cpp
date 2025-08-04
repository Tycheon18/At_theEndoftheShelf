// Fill out your copyright notice in the Description page of Project Settings.


#include "BookReviewWidget.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "BookReviewManager.h"
#include "BookSearchManager.h"

void UBookReviewWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SearchButton->OnClicked.AddDynamic(this, &UBookReviewWidget::OnSearchButtonClicked);
	SubmitReviewButton->OnClicked.AddDynamic(this, &UBookReviewWidget::OnSubmitReview);

	SearchManager = NewObject<UBookSearchManager>(this);
	SearchManager->OnSearchCompleted.AddDynamic(this, &UBookReviewWidget::HandleSearchResults);

	ReviewManager = NewObject<UBookReviewManager>(this);
	ReviewManager->OnReviewUploaded.AddDynamic(this, &UBookReviewWidget::HandleReviewUploaded);

	SearchResultsList->OnItemClicked().AddUObject(this, &UBookReviewWidget::HandleBookItemClicked);
}

void UBookReviewWidget::OnSearchButtonClicked()
{
	//SearchManager->SearchBooks(Query);
}

void UBookReviewWidget::OnSubmitReview()
{

}

void UBookReviewWidget::HandleSearchResults(const TArray<FBookInfo>& Results)
{
	SearchResultsList->ClearListItems();

	for (auto& Info : Results)
	{
		//UBookEntryWidget* 
	}
}

void UBookReviewWidget::HandleReviewUploaded(bool bSuccess)
{
	if (bSuccess)
	{

	}
	else
	{

	}
}

void UBookReviewWidget::HandleBookItemClicked(UObject* Item)
{
	//CurrentBook = Selected;
}
