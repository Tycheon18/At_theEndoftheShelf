// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "UObject/NoExportTypes.h"
#include "LibraryStruct.h"
#include "BookReviewManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReviewUploaded, bool, bSuccess);

/**
 * 
 */
UCLASS(Blueprintable)
class AT_THEENDOFTHESHELF_API UBookReviewManager : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable, Category = "Review")
	FOnReviewUploaded OnReviewUploaded;

	UFUNCTION(BlueprintCallable, Category = "Review")
	void UploadReviewWithCover(
		const FString& Title,
		const FString& Author,
		const FString& Content,
		const FBookInfo& BookInfo,
		const FString& CoverImagePath
	);

protected:

	void OnUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
