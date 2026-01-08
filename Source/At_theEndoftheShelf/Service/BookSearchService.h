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
	All UMETA(DisplayName = "All"),
	Title UMETA(DisplayName = "Title"),
	Authors UMETA(DisplayName = "Authors"),
	Publisher UMETA(DisplayName = "Publisher"),
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchCompleted, const FSearchResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchFailed, const FString&, ErrorMessage);

/**
 * 
 */
UCLASS(Blueprintable)
class AT_THEENDOFTHESHELF_API UBookSearchService : public UObject
{
	GENERATED_BODY()
	
public:
	UBookSearchService();

	UFUNCTION(BlueprintCallable, Category = "Book Search")
	void SearchBooks(const FString& Query, ESearchCategory Category = ESearchCategory::All, int32 Page = 1, int32 Size = 50);

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
		if(NetworkManager)
		{
			NetworkManager->OnResponse.RemoveAll(this);
			NetworkManager->OnError.RemoveAll(this);
		}

		NetworkManager = InNetworkManager;

		if(NetworkManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("NetworkManager set successfully."));

			NetworkManager->OnResponse.AddDynamic(this, &UBookSearchService::OnResponse);
			NetworkManager->OnError.AddDynamic(this, &UBookSearchService::OnError);
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Book Search")
	bool IsSearching() const
	{
		return bIsSearching;
	}

	UFUNCTION(BlueprintCallable, Category = "Delegate")
	void CleanupDelegates();

protected:

	UFUNCTION()
	void OnResponse(const FString& Response);
	
	UFUNCTION()
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
