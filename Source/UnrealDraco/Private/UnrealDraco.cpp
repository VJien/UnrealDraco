// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealDraco.h"
#include "draco/io/file_reader_factory.h"
#include "FileHelper.h"
#include "draco/io/file_writer_factory.h"

#define LOCTEXT_NAMESPACE "FUnrealDracoModule"

void FUnrealDracoModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	draco::FileReaderFactory::RegisterReader(draco::UD_FileReader::Open);
	draco::FileWriterFactory::RegisterWriter(draco::UD_FileWriter::Open);
}

void FUnrealDracoModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealDracoModule, UnrealDraco)