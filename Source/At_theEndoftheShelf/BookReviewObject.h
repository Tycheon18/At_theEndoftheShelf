// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LibraryStruct.h"
#include "BookReviewObject.generated.h"

/**
 * 
 */
UCLASS()
class AT_THEENDOFTHESHELF_API UBookReviewObject : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Review")
	FBookReview ReviewData;
};
