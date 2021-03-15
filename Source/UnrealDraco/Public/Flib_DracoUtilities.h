// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Flib_DracoUtilities.generated.h"



USTRUCT(BlueprintType)
struct FOptions 
{
	GENERATED_BODY()
		FOptions() :is_point_cloud(false),
		pos_quantization_bits(11),
		tex_coords_quantization_bits(10),
		tex_coords_deleted(false),
		normals_quantization_bits(8),
		normals_deleted(false),
		generic_quantization_bits(8),
		generic_deleted(false),
		compression_level(7),
		use_metadata(false) 
		{}


public:
	bool is_point_cloud;
	int pos_quantization_bits;
	int tex_coords_quantization_bits;
	bool tex_coords_deleted;
	int normals_quantization_bits;
	bool normals_deleted;
	int generic_quantization_bits;
	bool generic_deleted;
	int compression_level;
	bool use_metadata;
	FString input;
	FString output;
};




UCLASS()
class UNREALDRACO_API UFlib_DracoUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable)
		static void EncoderFromFile(const FString& fileName,  const FString& outName, FOptions options);



protected:
	};
