#ifndef FILES_INCLUDED
#define FILES_INCLUDED

#include <vector>
#include <string>

namespace files
{
	void get_file_list(std::vector<std::string>* o_files, const std::string& filename, const std::string& extension = ".obj");
	void replace_extension(const std::string& i_input, const std::string& i_ext, std::string* o_output);
}

#endif //FILES_INCLUDED