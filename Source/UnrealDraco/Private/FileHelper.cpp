#include "FileHelper.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#endif

#include "draco/io/file_reader_factory.h"
#include "draco/core/cycle_timer.h"
#include "draco/io/file_utils.h"
#include "draco/io/file_writer_factory.h"
//#include "draco/io/file_writer_utils.h"



namespace draco {

#define FILEREADER_LOG_ERROR(error_string)                             \
  do {                                                                 \
    fprintf(stderr, "%s:%d (%s): %s.\n", __FILE__, __LINE__, __func__, \
            error_string);                                             \
  } while (false)

bool UD_FileReader::registered_in_factory_ =
    FileReaderFactory::RegisterReader(UD_FileReader::Open);

UD_FileReader::~UD_FileReader() { fclose(file_); }

std::unique_ptr<FileReaderInterface> UD_FileReader::Open(
    const std::string &file_name) {
  if (file_name.empty()) {
    return nullptr;
  }

  FILE *raw_file_ptr = fopen(file_name.c_str(), "rb");

  if (raw_file_ptr == nullptr) {
    return nullptr;
  }

  std::unique_ptr<FileReaderInterface> file(new (std::nothrow)
                                                UD_FileReader(raw_file_ptr));
  if (file == nullptr) {
    FILEREADER_LOG_ERROR("Out of memory");
    fclose(raw_file_ptr);
    return nullptr;
  }

  return file;
}

bool UD_FileReader::ReadFileToBuffer(std::vector<char> *buffer) {
  if (buffer == nullptr) {
    return false;
  }
  buffer->clear();

  const size_t file_size = GetFileSize();
  if (file_size == 0) {
    FILEREADER_LOG_ERROR("Unable to obtain file size or file empty");
    return false;
  }

  buffer->resize(file_size);
  return fread(buffer->data(), 1, file_size, file_) == file_size;
}

bool UD_FileReader::ReadFileToBuffer(std::vector<uint8_t> *buffer) {
  if (buffer == nullptr) {
    return false;
  }
  buffer->clear();

  const size_t file_size = GetFileSize();
  if (file_size == 0) {
    FILEREADER_LOG_ERROR("Unable to obtain file size or file empty");
    return false;
  }

  buffer->resize(file_size);
  return fread(buffer->data(), 1, file_size, file_) == file_size;
}


size_t UD_FileReader::GetFileSize() {
  if (fseek(file_, SEEK_SET, SEEK_END) != 0) {
    FILEREADER_LOG_ERROR("Seek to EoF failed");
    return false;
  }

  const size_t file_size = static_cast<size_t>(ftell(file_));
  rewind(file_);

  return file_size;
}



int EncodeMeshToFile(const draco::Mesh& mesh, const std::string& file,
	draco::Encoder* encoder) {
	draco::CycleTimer timer;
	// Encode the geometry.
	draco::EncoderBuffer buffer;
	timer.Start();
	const draco::Status status = encoder->EncodeMeshToBuffer(mesh, &buffer);
	if (!status.ok()) {
		printf("Failed to encode the mesh.\n");
		printf("%s\n", status.error_msg());
		return -1;
	}
	timer.Stop();
	// Save the encoded geometry into a file.
	if (!draco::WriteBufferToFile(buffer.data(), buffer.size(), file)) {
		printf("Failed to create the output file.\n");
		return -1;
	}
	printf("Encoded mesh saved to %s (%" PRId64 " ms to encode).\n", file.c_str(),
		timer.GetInMs());
	printf("\nEncoded size = %zu bytes\n\n", buffer.size());
	return 0;
}
int EncodePointCloudToFile(const draco::PointCloud& pc, const std::string& file,
	draco::Encoder* encoder) {
	draco::CycleTimer timer;
	// Encode the geometry.
	draco::EncoderBuffer buffer;
	timer.Start();
	const draco::Status status = encoder->EncodePointCloudToBuffer(pc, &buffer);
	if (!status.ok()) {
		printf("Failed to encode the point cloud.\n");
		printf("%s\n", status.error_msg());
		return -1;
	}
	timer.Stop();
	// Save the encoded geometry into a file.
	if (!draco::WriteBufferToFile(buffer.data(), buffer.size(), file)) {
		printf("Failed to write the output file.\n");
		return -1;
	}
	printf("Encoded point cloud saved to %s (%" PRId64 " ms to encode).\n",
		file.c_str(), timer.GetInMs());
	printf("\nEncoded size = %zu bytes\n\n", buffer.size());
	return 0;
}

void SplitPathPrivate(const std::string& full_path,
	std::string* out_folder_path,
	std::string* out_file_name) {
	const auto pos = full_path.find_last_of("/\\");
	if (pos != std::string::npos) {
		if (out_folder_path) {
			*out_folder_path = full_path.substr(0, pos);
		}
		if (out_file_name) {
			*out_file_name = full_path.substr(pos + 1, full_path.length());
		}
	}
	else {
		if (out_folder_path) {
			*out_folder_path = ".";
		}
		if (out_file_name) {
			*out_file_name = full_path;
		}
	}
}

bool DirectoryExists(const std::string& path) {
	struct stat path_stat;

	// Check if |path| exists.
	if (stat(path.c_str(), &path_stat) != 0) {
		return false;
	}

	// Check if |path| is a directory.
	if (path_stat.st_mode & S_IFDIR) {
		return true;
	}
	return false;
}

bool CheckAndCreatePathForFile(const std::string& filename) {
	std::string path;
	std::string basename;
	SplitPathPrivate(filename, &path, &basename);

	const bool directory_exists = DirectoryExists(path);
	return directory_exists;
}

#define FILEWRITER_LOG_ERROR(error_string)                             \
  do {                                                                 \
    fprintf(stderr, "%s:%d (%s): %s.\n", __FILE__, __LINE__, __func__, \
            error_string);                                             \
  } while (false)

bool UD_FileWriter::registered_in_factory_ =
draco::FileWriterFactory::RegisterWriter(UD_FileWriter::Open);

UD_FileWriter::~UD_FileWriter() { fclose(file_); }

std::unique_ptr<FileWriterInterface> UD_FileWriter::Open(
	const std::string& file_name) {
	if (file_name.empty()) {
		return nullptr;
	}
	if (!CheckAndCreatePathForFile(file_name)) {
		return nullptr;
	}

	FILE* raw_file_ptr = fopen(file_name.c_str(), "wb");
	if (raw_file_ptr == nullptr) {
		return nullptr;
	}

	std::unique_ptr<UD_FileWriter> file(new (std::nothrow)
		UD_FileWriter(raw_file_ptr));
	if (file == nullptr) {
		FILEWRITER_LOG_ERROR("Out of memory");
		fclose(raw_file_ptr);
		return nullptr;
	}

#ifndef DRACO_OLD_GCC
	return file;
#else
	return std::move(file);
#endif
}

bool UD_FileWriter::Write(const char* buffer, size_t size) {
	return fwrite(buffer, 1, size, file_) == size;
}




}  // namespace draco
