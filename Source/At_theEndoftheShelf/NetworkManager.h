// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "Json.h"
#include "NetworkManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHttpResponse, const FString&, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHttpError, const FString&, ErrorMessage);
/**
 * 
 */
UCLASS()
class AT_THEENDOFTHESHELF_API UNetworkManager : public UObject
{
	GENERATED_BODY()

public:
	UNetworkManager();
	
	UFUNCTION(BlueprintCallable, Category = "Network")
	void Get(const FString& URL, const TMap<FString, FString>& Headers);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void Post(const FString& URL, const FString& Body, const TMap<FString, FString>& Headers);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void Put(const FString& URL, const FString& Body, const TMap<FString, FString>& Headers);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void Delete(const FString& URL, const TMap<FString, FString>& Headers);

	UPROPERTY(BlueprintAssignable, Category = "Network")
	FOnHttpResponse OnResponse;

	UPROPERTY(BlueprintAssignable, Category = "Network")
	FOnHttpError OnError;

	UFUNCTION(BlueprintCallable, Category = "Network")
	void SetAuthToken(const FString& Token)
	{
		AuthToken = Token;
	}

	UFUNCTION(BlueprintCallable, Category = "Network")
	void SetBaseURL(const FString& URL)
	{
		BaseURL = URL;
	}

	UFUNCTION(BlueprintCallable, Category = "Network")	
	void SetDefaultHeaders(const TMap<FString, FString>& Headers)
	{
		DefaultHeaders = Headers;
	}

	UFUNCTION(BlueprintCallable, Category = "Network")
	bool IsRequesting() const
	{
		return bIsRequesting;
	}

	UFUNCTION(BlueprintCallable, Category = "Network")
	void CancelRequest();

protected:

	void MakeRequest(const FString& Method, const FString& URL, const FString& Body, const TMap<FString, FString>& Headers);
	
	void OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FString BuildFullURL(const FString& URL);

	UPROPERTY()
	FString AuthToken;

	UPROPERTY()
	FString BaseURL;

	UPROPERTY()
	TMap<FString, FString> DefaultHeaders;

	UPROPERTY()
	bool bIsRequesting;

	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> CurrentRequest;
};
