// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "BookReviewObject.h"
#include "UObject/NoExportTypes.h"
#include "BookSearchManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBookSearchCompleted, const TArray<FBookInfo>&, Results);

/**
 * 
 */
UCLASS(Blueprintable)
class AT_THEENDOFTHESHELF_API UBookSearchManager : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Book Search")
	FOnBookSearchCompleted OnSearchCompleted;

	UFUNCTION(BlueprintCallable, Category = "Book Search")
	void SearchBooks(const FString& Query);

protected:
	void OnSearchResponse(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bWasSuccessful);
};
