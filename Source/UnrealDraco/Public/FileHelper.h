#ifndef DRACO_IO_STDIO_FILE_READER_H_
#define DRACO_IO_STDIO_FILE_READER_H_

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "draco/io/file_reader_interface.h"
#include "draco/point_cloud/point_cloud.h"
#include "draco/mesh/mesh.h"
#include "draco/compression/encode.h"
#include "draco/io/file_writer_interface.h"

namespace draco {

class UD_FileReader : public FileReaderInterface {
 public:
  // Creates and returns a UD_FileReader that reads from |file_name|.
  // Returns nullptr when the file does not exist or cannot be read.
  static std::unique_ptr<FileReaderInterface> Open(
      const std::string &file_name);

  UD_FileReader() = delete;
  UD_FileReader(const UD_FileReader &) = delete;
  UD_FileReader &operator=(const UD_FileReader &) = delete;

  UD_FileReader(UD_FileReader &&) = default;
  UD_FileReader &operator=(UD_FileReader &&) = default;

  // Closes |file_|.
  ~UD_FileReader() override;

  // Reads the entire contents of the input file into |buffer| and returns true.
  bool ReadFileToBuffer(std::vector<char> *buffer) override;
  bool ReadFileToBuffer(std::vector<uint8_t> *buffer) override;

  // Returns the size of the file.
  size_t GetFileSize() override;

 private:
  UD_FileReader(FILE *file) : file_(file) {}

  FILE *file_ = nullptr;
  static bool registered_in_factory_;
};




class UD_FileWriter : public FileWriterInterface {
public:
	// Creates and returns a UD_FileWriter that writes to |file_name|.
	// Returns nullptr when |file_name| cannot be opened for writing.
	static std::unique_ptr<FileWriterInterface> Open(
		const std::string& file_name);

	UD_FileWriter() = delete;
	UD_FileWriter(const UD_FileWriter&) = delete;
	UD_FileWriter& operator=(const UD_FileWriter&) = delete;

	UD_FileWriter(UD_FileWriter&&) = default;
	UD_FileWriter& operator=(UD_FileWriter&&) = default;

	// Closes |file_|.
	~UD_FileWriter() override;

	// Writes |size| bytes to |file_| from |buffer|. Returns true for success.
	bool Write(const char* buffer, size_t size) override;

private:
	UD_FileWriter(FILE* file) : file_(file) {}

	FILE* file_ = nullptr;
	static bool registered_in_factory_;
};


}  // namespace draco

#endif  // DRACO_IO_STDIO_FILE_READER_H_
