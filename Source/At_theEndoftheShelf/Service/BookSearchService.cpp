// Fill out your copyright notice in the Description page of Project Settings.


#include "BookSearchService.h"
#include "JsonUtilities.h"

UBookSearchService::UBookSearchService()
{
	NetworkManager = nullptr;
	bIsSearching = false;
	//BaseUrl = TEXT("https://dapi.kakao.com/v3/search/book");
	BaseUrl = TEXT("http://localhost:3000/books");
}

void UBookSearchService::SearchBooks(const FString& Query, ESearchCategory Category, int32 Page, int32 Size)
{
	if (Query.IsEmpty())
	{
		OnSearchFailed.Broadcast(TEXT("Please enter a search query."));
		return;
	}

	if (bIsSearching)
	{
		UE_LOG(LogTemp, Warning, TEXT("Canceling previous search request"));
		NetworkManager->CancelRequest();
	}

	if(ApiKey.IsEmpty())
	{
		OnSearchFailed.Broadcast(TEXT("Kakao API key is not configured."));
		return;
	}

	if (!NetworkManager)
	{
		OnSearchFailed.Broadcast(TEXT("NetworkManager is not initialized."));
		return;
	}

	bIsSearching = true;

	FString EncodedQuery = FGenericPlatformHttp::UrlEncode(Query);

	FString Url = FString::Printf(TEXT("%s?query=%s&page=%d&size=%d"), 
		*BaseUrl, *EncodedQuery, Page, Size);

	if(Category != ESearchCategory::All)
	{
		Url += FString::Printf(TEXT("&Target=%s"), *CategoryToString(Category));
	}

	TMap<FString, FString> Headers;
	Headers.Add(TEXT("Authorization"), FString::Printf(TEXT("KakaoAK %s"), *ApiKey));

	NetworkManager->Get(Url, Headers);

	UE_LOG(LogTemp, Warning, TEXT("Final URL: %s"), *Url);
}

void UBookSearchService::OnResponse(const FString& Response)
{
	bIsSearching = false;
	
	FSearchResult SearchResult;

	UE_LOG(LogTemp, Warning, TEXT("OnResponse is Called"));

	UE_LOG(LogTemp, Warning, TEXT("Response Length: %d"), Response.Len());
	UE_LOG(LogTemp, Warning, TEXT("Response Preview: %s"),
		Response.Len() > 100 ? *Response.Left(100) : *Response);

	UE_LOG(LogTemp, Log, TEXT("Full Response: %s"), *Response);

	if (ParseSearchResponse(Response, SearchResult))
	{
		OnSearchCompleted.Broadcast(SearchResult);
	}
	else
	{
		OnSearchFailed.Broadcast(TEXT("Failed to parse search results."));
	}
}

void UBookSearchService::OnError(const FString& Error)
{
	bIsSearching = false;

	OnSearchFailed.Broadcast(Error);
}

bool UBookSearchService::ParseSearchResponse(const FString& JsonString, FSearchResult& OutResult)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	//if(!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	//{
	//	UE_LOG(LogTemp, Error, TEXT("BookSearchService: Failed to parse JSON"));
	//	return false;
	//}

	if (!FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		UE_LOG(LogTemp, Error, TEXT("ParseSearchResponse: Deserialize failed"));
		return false;
	}

	OutResult.TotalCount = JsonArray.Num();
	OutResult.bIsEnd = true; // Assuming all results are returned in one go
	OutResult.PageableCount = JsonArray.Num();

	//const TSharedPtr<FJsonObject>* MetaObject;
	//if(JsonObject->TryGetObjectField("meta", MetaObject))
	//{
	//	OutResult.TotalCount = (*MetaObject)->GetNumberField(TEXT("total_count"));
	//	OutResult.bIsEnd = (*MetaObject)->GetBoolField(TEXT("is_end"));
	//	OutResult.PageableCount = (*MetaObject)->GetNumberField(TEXT("pageable_count"));

	//}

	//const TArray<TSharedPtr<FJsonValue>>* Documents;
	//if(JsonObject->TryGetArrayField("documents", Documents))
	//{
		for(const auto& JsonValue : JsonArray)
		{
			auto BookObject = JsonValue->AsObject();
			if(!BookObject.IsValid())
			{
				continue;
			}

			FBookInfo BookInfo;
			BookInfo.Title = BookObject->GetStringField(TEXT("title"));
			BookInfo.Publisher = BookObject->GetStringField(TEXT("publisher"));
			BookInfo.Thumbnail = BookObject->GetStringField(TEXT("thumbnail"));
			BookInfo.Contents = BookObject->GetStringField(TEXT("summary"));
			BookInfo.ISBN = BookObject->GetStringField(TEXT("isbn"));
			//BookInfo.Price = BookObject->GetStringField(TEXT("price"));
			//BookInfo.SalePrice = BookObject->GetStringField(TEXT("sale_price"));
			//BookInfo.Status = BookObject->GetStringField(TEXT("status"));
			//			

			const TArray<TSharedPtr<FJsonValue>>* AuthorsArray;
			if(BookObject->TryGetArrayField(TEXT("authors"), AuthorsArray))
			{
				for(const auto& AuthorValue : *AuthorsArray)
				{
					BookInfo.Authors.Add(AuthorValue->AsString());
				}
			}

			OutResult.Books.Add(BookInfo);
			
		}
//	}

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

void UBookSearchService::CleanupDelegates()
{
	if (NetworkManager)
	{
		NetworkManager->OnResponse.RemoveAll(this);
		NetworkManager->OnError.RemoveAll(this);
	}

	OnSearchCompleted.Clear();
	OnSearchFailed.Clear();
}

