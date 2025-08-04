// Fill out your copyright notice in the Description page of Project Settings.


#include "BookSearchService.h"
#include "JsonUtilities.h"

UBookSearchService::UBookSearchService()
{
	NetworkManager = nullptr;
	bIsSearching = false;
	BaseUrl = TEXT("https://dapi.kakao.com/v3/search/book");
}

void UBookSearchService::SearchBooks(const FString& Query, ESearchCategory Category, int32 Page, int32 Size)
{
	if (Query.IsEmpty())
	{
		OnSearchFailed.Broadcast(TEXT("�˻�� �Է����ּ�."));
		return;
	}

	if(ApiKey.IsEmpty())
	{
		OnSearchFailed.Broadcast(TEXT("īī�� API Ű�� �������� �ʾҽ��ϴ�."));
		return;
	}

	if (!NetworkManager)
	{
		OnSearchFailed.Broadcast(TEXT("NetworkManager�� �������� �ʾҽ��ϴ�."));
		return;
	}

	bIsSearching = true;

	FString Url = FString::Printf(TEXT("%s?query=%s&page=%d&size=%d"), 
		*BaseUrl, *Query, Page, Size);

	if(Category != ESearchCategory::All)
	{
		Url += FString::Printf(TEXT("&Target=%s"), *CategoryToString(Category));
	}

	TMap<FString, FString> Headers;
	Headers.Add(TEXT("Authorization"), FString::Printf(TEXT("KakaoAK %s"), *ApiKey));

	NetworkManager->OnResponse.AddDynamic(this, &UBookSearchService::OnResponse);
	NetworkManager->OnError.AddDynamic(this, &UBookSearchService::OnError);

	NetworkManager->Get(Url, Headers);
}

void UBookSearchService::OnResponse(const FString& Response)
{
	bIsSearching = false;

	NetworkManager->OnResponse.RemoveDynamic(this, &UBookSearchService::OnResponse);
	NetworkManager->OnResponse.RemoveDynamic(this, &UBookSearchService::OnError);	
	FSearchResult SearchResult;

	if (ParseSearchResponse(Response, SearchResult))
	{
		OnSearchCompleted.Broadcast(SearchResult);
	}
	else
	{
		OnSearchFailed.Broadcast(TEXT("�˻� ����� �Ľ��ϴµ� �����߽��ϴ�."));
	}
}

void UBookSearchService::OnError(const FString& Error)
{
	bIsSearching = false;

	NetworkManager->OnResponse.RemoveDynamic(this, &UBookSearchService::OnResponse);
	NetworkManager->OnError.RemoveDynamic(this, &UBookSearchService::OnError);

	OnSearchFailed.Broadcast(Error);
}

bool UBookSearchService::ParseSearchResponse(const FString& JsonString, FSearchResult& OutResult)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if(!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("BookSearchService: Failed to parse JSON"));
		return false;
	}

	const TSharedPtr<FJsonObject>* MetaObject;
	if(JsonObject->TryGetObjectField("meta", MetaObject))
	{
		OutResult.TotalCount = (*MetaObject)->GetNumberField(TEXT("total_count"));
		OutResult.bIsEnd = (*MetaObject)->GetBoolField(TEXT("is_end"));
		OutResult.PageableCount = (*MetaObject)->GetNumberField(TEXT("pageable_count"));

	}

	const TArray<TSharedPtr<FJsonValue>>* Documents;
	if(JsonObject->TryGetArrayField("documents", Documents))
	{
		for(const auto& Doc : *Documents)
		{
			auto BookObject = Doc->AsObject();

			FBookInfo BookInfo;
			BookInfo.Title = BookObject->GetStringField(TEXT("title"));
			BookInfo.Publisher = BookObject->GetStringField(TEXT("publisher"));
			BookInfo.Thumbnail = BookObject->GetStringField(TEXT("thumbnail"));
			BookInfo.Contents = BookObject->GetStringField(TEXT("contents"));
			BookInfo.ISBN = BookObject->GetStringField(TEXT("isbn"));
			BookInfo.Price = BookObject->GetStringField(TEXT("price"));
			BookInfo.SalePrice = BookObject->GetStringField(TEXT("sale_price"));
			BookInfo.Status = BookObject->GetStringField(TEXT("status"));
						

			const TArray<TSharedPtr<FJsonValue>>* Authors;
			if(BookObject->TryGetArrayField(TEXT("authors"), Authors))
			{
				for(const auto& AuthorValue : *Authors)
				{
					BookInfo.Authors.Add(AuthorValue->AsString());
				}
			}

			OutResult.Books.Add(BookInfo);
			
		}
	}

	UE_LOG(LogTemp, Log, TEXT("BookSearchService: Parsed %d books"), OutResult.Books.Num());

	return true;
}

FString UBookSearchService::CategoryToString(ESearchCategory Category)
{
	switch(Category)
	{
		case ESearchCategory::Title: return TEXT("title");
		case ESearchCategory::Authors: return TEXT("authors");
		case ESearchCategory::Publisher: return TEXT("publisher");
		default: return TEXT("all");
	}
}

