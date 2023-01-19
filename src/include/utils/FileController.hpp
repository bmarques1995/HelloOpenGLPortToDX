#ifndef FILE_CONTROLLER_HPP
#define FILE_CONTROLLER_HPP

#include <string>
#include <cstddef>

namespace APILearning
{
	class FileController
	{
	public:
		static bool ReadTextFile(std::string_view path, std::string* content);
		static bool WriteTextFile(std::string_view path, std::string content);
		static bool ReadBinFile(std::string_view path, std::byte** content);
		static bool WriteBinFile(std::string_view path, std::byte* content, size_t dataSize);

		static bool FileExists(std::string_view path);
	};
}

#endif // !1