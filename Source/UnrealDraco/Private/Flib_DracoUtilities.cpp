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
#include "draco/io/parser_utils.h"
#include "draco/io/obj_encoder.h"
#include "draco/io/ply_encoder.h"

#if defined(DRACO_MACRO_TEMP_ERROR)
#define ERROR           DRACO_MACRO_TEMP_ERROR
#undef DRACO_MACRO_TEMP_ERROR
#endif



 


bool UFlib_DracoUtilities::EncoderFromFile(const FString& inFileName, const FString& outFileName, FOptions options)
{
	if (options.pos_quantization_bits > 30)
	{
		UDWARNING("error: The maximum number of quantization bits for the position attribute is 30.\n");
		return false;
	}
	if (options.tex_coords_quantization_bits > 30)
	{
		UDWARNING("error: The maximum number of quantization bits for the texture coordinate attribute is 30.\n");
		return false;
	}
	if (options.normals_quantization_bits > 30)
	{
		UDWARNING("error: The maximum number of quantization bits for the normal attribute is 30.\n");
		return false;
	}
	if (options.generic_quantization_bits > 30)
	{
		UDWARNING("error: The maximum number of quantization bits for generic attribute is 30.\n");
		return false;
	}
	if (inFileName.IsEmpty() || outFileName.IsEmpty())
	{
		UDWARNING("Error: inFileName or outFileName is invalid.\n");
		return false;
	}
	std::unique_ptr<draco::PointCloud> pc;
	draco::Mesh *mesh = nullptr;
	std::string inFile(TCHAR_TO_UTF8(*inFileName));
	std::string outFile(TCHAR_TO_UTF8(*outFileName));
	
	if (!options.is_point_cloud)
	{
		draco::Options opt;

		auto maybe_mesh = draco::ReadMeshFromFile(inFile, opt, nullptr);

		if (!maybe_mesh.ok())
		{
			UDWARNING("Failed loading the input mesh\n");
			return false;
		}
		mesh = maybe_mesh.value().get();
		pc = std::move(maybe_mesh).value();
	}
	else
	{
		auto maybe_pc = draco::ReadPointCloudFromFile(inFile);
		if (!maybe_pc.ok())
		{
			UDWARNING("Failed loading the input point cloud\n");
			return false;
		}
		pc = std::move(maybe_pc).value();
	}
	if (options.pos_quantization_bits < 0)
	{
		UDWARNING("Error: Position attribute cannot be skipped.\n");
		return false;
	}
	if (options.pos_quantization_bits < 0)
	{
		UDWARNING("Error: Position attribute cannot be skipped.\n");
		return false;
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
		UDWARNING("For better compression, increase the compression level up to '-cl 10");
	}
	return true;
}

bool UFlib_DracoUtilities::DecoderToFile(const FString& inFileName, const FString& outFileName)
{
	if (inFileName.IsEmpty() || outFileName.IsEmpty())
	{
		UDWARNING("DecoderToFile : invalid file name.\n");
		return false;
	}
	std::string inFile(TCHAR_TO_UTF8(*inFileName));
	std::string outFile(TCHAR_TO_UTF8(*outFileName));
	std::vector<char> data;
	if (!draco::ReadFileToBuffer(inFile, &data)) 
	{
		UDWARNING("Failed opening the input file.\n");
		return false;
	}
	if (data.empty()) 
	{
		UDWARNING("Empty input file.\n");
		return false;
	}

	draco::DecoderBuffer buffer;
	buffer.Init(data.data(), data.size());

	draco::CycleTimer timer;
	// Decode the input data into a geometry.
	std::unique_ptr<draco::PointCloud> pc;
	draco::Mesh* mesh = nullptr;

	auto type_statusor = draco::Decoder::GetEncodedGeometryType(&buffer);
	if (!type_statusor.ok()) 
	{
		UDWARNING1("Failed to decode the input file %s\n", type_statusor.status().error_msg_string().c_str());
		return false;
	}
	const draco::EncodedGeometryType geom_type = type_statusor.value();

	if (geom_type == draco::TRIANGULAR_MESH) {
		timer.Start();
		draco::Decoder decoder;
		auto statusor = decoder.DecodeMeshFromBuffer(&buffer);
		if (!statusor.ok()) 
		{
			UDWARNING1("Failed to decode the input file %s\n", statusor.status().error_msg_string().c_str());
			return false;
		}
		std::unique_ptr<draco::Mesh> in_mesh = std::move(statusor).value();
		timer.Stop();
		if (in_mesh) {
			mesh = in_mesh.get();
			pc = std::move(in_mesh);
		}
	}
	else if (geom_type == draco::POINT_CLOUD) {
		// Failed to decode it as mesh, so let's try to decode it as a point cloud.
		timer.Start();
		draco::Decoder decoder;
		auto statusor = decoder.DecodePointCloudFromBuffer(&buffer);
		if (!statusor.ok()) 
		{
			UDWARNING1("Failed to decode the input file %s\n", statusor.status().error_msg_string().c_str());
			return false;
		}
		pc = std::move(statusor).value();
		timer.Stop();
	}

	if (pc == nullptr) {
		UDWARNING("Failed to decode the input file.\n");
		return false;
	}
	const std::string extension = draco::parser::ToLower(
		outFile.size() >= 4
		? outFile.substr(outFile.size() - 4)
		: outFile);

	if (extension == ".obj") {
		draco::ObjEncoder obj_encoder;
		if (mesh) {
			if (!obj_encoder.EncodeToFile(*mesh, outFile)) {
				UDWARNING("Failed to store the decoded mesh as OBJ.\n");
				return false;
			}
		}
		else {
			if (!obj_encoder.EncodeToFile(*pc.get(), outFile)) {
				UDWARNING("Failed to store the decoded point cloud as OBJ.\n");
				return false;
			}
		}
	}
	else if (extension == ".ply") {
		draco::PlyEncoder ply_encoder;
		if (mesh) {
			if (!ply_encoder.EncodeToFile(*mesh, outFile)) {
				UDWARNING("Failed to store the decoded mesh as PLY.\n");
				return false;
			}
		}
		else {
			if (!ply_encoder.EncodeToFile(*pc.get(), outFile)) {
				UDWARNING("Failed to store the decoded point cloud as PLY.\n");
				return false;
			}
		}
	}
	else {
		UDWARNING("Invalid extension of the output file. Use either .ply or .obj.\n");
		return false;
	}
	UDWARNING2("Decoded geometry saved to %s (%" PRId64 " ms to decode)\n",outFile.c_str(), timer.GetInMs());

	return true;
}


