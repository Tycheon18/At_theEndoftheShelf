// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkManager.h"

UNetworkManager::UNetworkManager()
{
	bIsRequesting = false;
	BaseURL = TEXT("");
	AuthToken = TEXT("");
}

void UNetworkManager::Get(const FString& URL, const TMap<FString, FString>& Headers)
{
	MakeRequest(TEXT("GET"), URL, "", Headers);
}

void UNetworkManager::Post(const FString& URL, const FString& Body, const TMap<FString, FString>& Headers)
{
	MakeRequest(TEXT("POST"), URL, Body, Headers);
}

void UNetworkManager::Put(const FString& URL, const FString& Body, const TMap<FString, FString>& Headers)
{
	MakeRequest(TEXT("PUT"), URL, Body, Headers);
}

void UNetworkManager::Delete(const FString& URL, const TMap<FString, FString>& Headers)
{
	MakeRequest(TEXT("DELETE"), URL, "", Headers);
}

FString UNetworkManager::GetHttpErrorMessage(int32 ResponseCode)
{
	if (ResponseCode >= 400 && ResponseCode < 500)
	{
		// 4xx: Client Error
		switch (ResponseCode)
		{
			case 400: return TEXT("Bad request. Please check your input.");
			case 401: return TEXT("Unauthorized. Please check your API key.");
			case 403: return TEXT("Access forbidden.");
			case 404: return TEXT("Resource not found.");
			case 429: return TEXT("Too many requests. Please try again later.");
			default: return FString::Printf(TEXT("Client error %d"), ResponseCode);
		}
	}
	else if (ResponseCode >= 500)
	{
		// 5xx: Server Error
		return FString::Printf(TEXT("Server error %d. Please try again later."), ResponseCode);
	}

	return FString::Printf(TEXT("HTTP Error %d"), ResponseCode);

}

void UNetworkManager::DownloadImage(const FString& ImageUrl)
{
	if(ImageUrl.IsEmpty())
	{
		OnImageDownload.Broadcast(ImageUrl, nullptr);
		return;
	}

	if (ActiveImageRequests.Contains(ImageUrl))
	{
		UE_LOG(LogTemp, Warning, TEXT("Image already downloading: %s"), *ImageUrl);
		return;
	}

	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(ImageUrl);
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this, ImageUrl](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			this->OnImageDownloadComplete(ImageUrl, Request, Response, bWasSuccessful);
		}
	);

	ActiveImageRequests.Add(ImageUrl, HttpRequest);
	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Log, TEXT("NetworkManager: Downloading image from %s"), *ImageUrl);

}

void UNetworkManager::CancelRequest()
{
	if(CurrentRequest.IsValid())
	{
		CurrentRequest->CancelRequest();
		bIsRequesting = false;
		CurrentRequest.Reset();
	}

}

void UNetworkManager::MakeRequest(const FString& Method, const FString& URL, const FString& Body, const TMap<FString, FString>& Headers)
{
	if (bIsRequesting)
	{
		UE_LOG(LogTemp, Warning, TEXT("NetworkManager: Request is already in progress."));
		return;
	}

	bIsRequesting = true;

	CurrentRequest = FHttpModule::Get().CreateRequest();
	CurrentRequest->SetVerb(Method);
	CurrentRequest->SetURL(BuildFullURL(URL));

	for (const auto& Header : DefaultHeaders)
	{
		CurrentRequest->SetHeader(Header.Key, Header.Value);
	}

	for (const auto& Header : Headers)
	{
		CurrentRequest->SetHeader(Header.Key, Header.Value);
	}

	if(!AuthToken.IsEmpty())
	{
		CurrentRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));
	}

	if(!Body.IsEmpty() && (Method == "POST" || Method == "PUT"))
	{
		CurrentRequest->SetContentAsString(Body);
		CurrentRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	}

	CurrentRequest->OnProcessRequestComplete().BindUObject(this, &UNetworkManager::OnHttpResponseReceived);

	CurrentRequest->ProcessRequest();

	UE_LOG(LogTemp, Log, TEXT("NetworkManager: %s Request to %s"), *Method, *BuildFullURL(URL));
}

void UNetworkManager::OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bIsRequesting = false;
	CurrentRequest.Reset();

	if(bWasSuccessful && Response.IsValid())
	{
		FString ResponseString = Response->GetContentAsString();
		int32 ResponseCode = Response->GetResponseCode();

		UE_LOG(LogTemp, Log, TEXT("NetworkManager: Response received (Code: %d)"), ResponseCode);

		if(ResponseCode >= 200 && ResponseCode < 300)
		{
			OnResponse.Broadcast(ResponseString);
		}
		else
		{
			FString ErrorMessage = GetHttpErrorMessage(ResponseCode);
			OnError.Broadcast(ErrorMessage);
		}
	}
	else
	{
		FString ErrorMessage = Response.IsValid() ? FString::Printf(TEXT("Network Error: %d"), Response->GetResponseCode()) : TEXT("Network Connection Failed");
		OnError.Broadcast(ErrorMessage);
	}
}

void UNetworkManager::OnImageDownloadComplete(const FString& OriginalUrl, FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{

}

FString UNetworkManager::BuildFullURL(const FString& URL)
{
	if(URL.StartsWith(TEXT("http://")) || URL.StartsWith(TEXT("https://")))
	{
		return URL; // Already a full URL
	}

	if(!BaseURL.IsEmpty())
	{
		return FString::Printf(TEXT("%s%s"), *BaseURL, *URL); // Combine with base URL
	}

	return URL;
}
