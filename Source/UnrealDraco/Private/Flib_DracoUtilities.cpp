// Fill out your copyright notice in the Description page of Project Settings.


#include "Flib_DracoUtilities.h"

#include <iostream>

#include "FileHelper.h"

#if defined(ERROR)
#define DRACO_MACRO_TEMP_ERROR      ERROR
#undef ERROR
#endif


#include "draco/mesh/mesh.h"
#include "draco/io/mesh_io.h"
#include "draco/core/options.h"
#include "draco/io/file_reader_factory.h"
#include "draco/point_cloud/point_cloud.h"
#include "draco/io/point_cloud_io.h"
#include "draco/compression/encode.h"
#include "draco/core/cycle_timer.h"
#include "draco/io/file_utils.h"
#include "draco/io/file_utils.h"

#if defined(DRACO_MACRO_TEMP_ERROR)
#define ERROR           DRACO_MACRO_TEMP_ERROR
#undef DRACO_MACRO_TEMP_ERROR
#endif



 


void UFlib_DracoUtilities::EncoderFromFile(const FString& fileName, const FString& outName, FOptions options)
{
	if (options.tex_coords_quantization_bits > 30)
	{
		UE_LOG(LogTemp, Warning, TEXT("error: The maximum number of quantization bits for the texture coordinate attribute is 30.\n"));
		return;
	}
	if (options.normals_quantization_bits > 30)
	{
		UE_LOG(LogTemp, Warning, TEXT("error: The maximum number of quantization bits for the normal attribute is 30.\n"));
		return;
	}
	if (options.generic_quantization_bits > 30)
	{
		UE_LOG(LogTemp, Warning, TEXT("error: The maximum number of quantization bits for generic attribute is 30.\n"));
		return;
	}
	if (fileName.IsEmpty() || outName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: FileName or OutName is invalid.\n"));
		return;
	}
	std::unique_ptr<draco::PointCloud> pc;
	draco::Mesh *mesh = nullptr;
	std::string inFile(TCHAR_TO_UTF8(*fileName));
	std::string outFile(TCHAR_TO_UTF8(*outName));
	
	if (!options.is_point_cloud)
	{
		draco::Options opt;

		auto maybe_mesh = draco::ReadMeshFromFile(inFile, opt, nullptr);

		if (!maybe_mesh.ok())
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed loading the input mesh\n"));
			return;
		}
		mesh = maybe_mesh.value().get();
		pc = std::move(maybe_mesh).value();
	}
	else
	{
		auto maybe_pc = draco::ReadPointCloudFromFile(inFile);
		if (!maybe_pc.ok())
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed loading the input point cloud\n"));
			return;
		}
		pc = std::move(maybe_pc).value();
	}
	if (options.pos_quantization_bits < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Position attribute cannot be skipped.\n"));
		return;
	}
	if (options.pos_quantization_bits < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Position attribute cannot be skipped.\n"));
		return;
	}
	if (options.tex_coords_quantization_bits < 0) {
		if (pc->NumNamedAttributes(draco::GeometryAttribute::TEX_COORD) > 0) {
			options.tex_coords_deleted = true;
		}
		while (pc->NumNamedAttributes(draco::GeometryAttribute::TEX_COORD) > 0) {
			pc->DeleteAttribute(
				pc->GetNamedAttributeId(draco::GeometryAttribute::TEX_COORD, 0));
		}
	}
	if (options.normals_quantization_bits < 0) {
		if (pc->NumNamedAttributes(draco::GeometryAttribute::NORMAL) > 0) {
			options.normals_deleted = true;
		}
		while (pc->NumNamedAttributes(draco::GeometryAttribute::NORMAL) > 0) {
			pc->DeleteAttribute(
				pc->GetNamedAttributeId(draco::GeometryAttribute::NORMAL, 0));
		}
	}
	if (options.generic_quantization_bits < 0) {
		if (pc->NumNamedAttributes(draco::GeometryAttribute::GENERIC) > 0) {
			options.generic_deleted = true;
		}
		while (pc->NumNamedAttributes(draco::GeometryAttribute::GENERIC) > 0) {
			pc->DeleteAttribute(
				pc->GetNamedAttributeId(draco::GeometryAttribute::GENERIC, 0));
		}
	}
#ifdef DRACO_ATTRIBUTE_INDICES_DEDUPLICATION_SUPPORTED
	if (options.tex_coords_deleted || options.normals_deleted ||
		options.generic_deleted) {
		pc->DeduplicatePointIds();
	}
#endif
	const int speed = 10 - options.compression_level;


	draco::Encoder encoder;
	if (options.pos_quantization_bits > 0) {
		encoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION,
			options.pos_quantization_bits);
	}
	if (options.tex_coords_quantization_bits > 0) {
		encoder.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD,
			options.tex_coords_quantization_bits);
	}
	if (options.normals_quantization_bits > 0) {
		encoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL,
			options.normals_quantization_bits);
	}
	if (options.generic_quantization_bits > 0) {
		encoder.SetAttributeQuantization(draco::GeometryAttribute::GENERIC,
			options.generic_quantization_bits);
	}
	encoder.SetSpeedOptions(speed, speed);

	int ret = -1;
	const bool input_is_mesh = mesh && mesh->num_faces() > 0;
	if (input_is_mesh)
	{
		ret = draco::EncodeMeshToFile(*mesh, outFile, &encoder);
	}
	else
	{
		ret = draco::EncodePointCloudToFile(*pc.get(), outFile, &encoder);
	}

	if (ret != -1 && options.compression_level < 10)
	{
		UE_LOG(LogTemp, Warning, TEXT("For better compression, increase the compression level up to '-cl 10"));
	}

}



