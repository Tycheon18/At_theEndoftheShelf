// Fill out your copyright notice in the Description page of Project Settings.


#include "BookSearchManager.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "GenericPlatform/GenericPlatformHttp.h"

void UBookSearchManager::SearchBooks(const FString& Query)
{
	const FString Encoded = FGenericPlatformHttp::UrlEncode(Query);
	const FString Url = FString::Printf(TEXT("http://localhost:4000/books?query=%s"), *Encoded);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
	Req->SetURL(Url);
	Req->SetVerb("GET");
	Req->OnProcessRequestComplete().BindUObject(this, &UBookSearchManager::OnSearchResponse);
	Req->ProcessRequest();

}

void UBookSearchManager::OnSearchResponse(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bWasSuccessful)
{
	TArray<FBookInfo> Books;
	if (bWasSuccessful && Res->GetResponseCode() == 200)
	{
		FString Contect = Res->GetContentAsString();
		TSharedPtr<FJsonValue> RootValue;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Contect);

		TArray<TSharedPtr<FJsonValue>> JsonArray;
		if (FJsonSerializer::Deserialize(Reader, JsonArray))
		{
			for (auto& Val : JsonArray)
			{
				//TSharedPtr<FJsonObject> Obj = Val->AsObject();
				//FBookInfo B;
				//B.Title = Obj->GetStringField("title");
				//B.Publisher = Obj->GetStringField("publisher");
				//B.ISBN = Obj->GetStringField("isbn");
				//B.Thumbnail = Obj->GetStringField("thumbnail");
				//
			
				//for (auto& AuthVal : Obj->GetArrayField("authors"))
				//{
				//	B.Authors.Add(AuthVal->AsString());
				//}

				//Books.Add(B);
			}
		}
	}

	OnSearchCompleted.Broadcast(Books);
}
