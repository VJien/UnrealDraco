// Copyright VJ. All Rights Reserved.


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool is_point_cloud;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int pos_quantization_bits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int tex_coords_quantization_bits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool tex_coords_deleted;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int normals_quantization_bits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool normals_deleted;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int generic_quantization_bits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool generic_deleted;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int compression_level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool use_metadata;


};




UCLASS()
class UNREALDRACO_API UFlib_DracoUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = UnrealDraco)
		static bool Encoder(const FString& inFileName,  const FString& outFileName, FOptions options);
	UFUNCTION(BlueprintCallable, Category = UnrealDraco)
		static bool Decoder(const FString& inFileName, const FString& outFileName);


};
