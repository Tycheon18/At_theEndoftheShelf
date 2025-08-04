// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../NetworkManager.h"
#include "../LibraryStruct.h"
#include "BookSearchService.generated.h"

UENUM(BlueprintType)
enum class ESearchCategory : uint8
{
	All UMETA(DisplayName = "��ü"),
	Title UMETA(DisplayName = "å ����"),
	Authors UMETA(DisplayName = "����"),
	Publisher UMETA(DisplayName = "���ǻ�"),
};

USTRUCT(BlueprintType)
struct FSearchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Search")
	TArray<FBookInfo> Books;
	
	UPROPERTY(BlueprintReadWrite, Category = "Search")
	int32 TotalCount;
	
	UPROPERTY(BlueprintReadWrite, Category = "Search")
	bool bIsEnd;

	UPROPERTY(BlueprintReadWrite, Category = "Search")
	int32 PageableCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchCompleted, const FSearchResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchFailed, const FString&, ErrorMessage);

/**
 * 
 */
UCLASS()
class AT_THEENDOFTHESHELF_API UBookSearchService : public UObject
{
	GENERATED_BODY()
	
public:
	UBookSearchService();

	UFUNCTION(BlueprintCallable, Category = "Book Search")
	void SearchBooks(const FString& Query, ESearchCategory Category = ESearchCategory::All, int32 Page = 1, int32 Size = 10);

	UPROPERTY(BlueprintAssignable, Category = "Book Search")
	FOnSearchCompleted OnSearchCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Book Search")
	FOnSearchFailed OnSearchFailed;

	UFUNCTION(BlueprintCallable, Category = "Book Search")
	void SetApiKey(const FString& InApiKey)
	{
		ApiKey = InApiKey;
	}

	UFUNCTION(BlueprintCallable, Category = "Book Search")
	void SetNetworkManager(UNetworkManager* InNetworkManager)
	{
		NetworkManager = InNetworkManager;
	}

	UFUNCTION(BlueprintCallable, Category = "Book Search")
	bool IsSearching() const
	{
		return bIsSearching;
	}

protected:

	void OnResponse(const FString& Response);
	void OnError(const FString& Error);
	bool ParseSearchResponse(const FString& JsonString, FSearchResult& OutResult);
	FString CategoryToString(ESearchCategory Category);

	UPROPERTY()
	UNetworkManager* NetworkManager;

	UPROPERTY()
	FString ApiKey;

	UPROPERTY()
	bool bIsSearching;

	UPROPERTY()
	FString BaseUrl;
};
