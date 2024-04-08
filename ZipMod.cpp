#include "ZipMod.h"

#include <string>
#include <format>
#include <filesystem>

#include <zip.h>

#include <Godot.hpp>
#include <Node.hpp>


using namespace std;
using namespace godot;
namespace fs = std::filesystem;

void ZipMod::_register_methods()
{
	register_method("zipDirectory", &ZipMod::zipDirectory);
}

ZipMod::ZipMod()
{
}

ZipMod::~ZipMod()
{
}

void ZipMod::_init()
{
}


bool ZipMod::is_dir(const std::string& dir) 
{
	return fs::is_directory(dir);
}

bool godot::ZipMod::walk_dir(const std::string& startdir, const std::string& inputdir, zip_t* zipper)
{
	std::string mod_folder = startdir.substr(startdir.rfind("/") + 1, startdir.length() - startdir.rfind("/"));

	for (auto const& dir_entry : fs::recursive_directory_iterator(inputdir)) {
		std::string path = dir_entry.path().string();

		if (dir_entry.is_directory()) {
			if (zip_dir_add(zipper, std::string(mod_folder + "/" + path.substr(startdir.length() + 1)).c_str(), ZIP_FL_ENC_UTF_8) < 0) {
				//throw std::runtime_error("[YHMA] Failed to add subdirectory to zip: " + std::string(zip_strerror(zipper)));
				Godot::print_error("[YHMA] Failed to add subdirectory to zip",
					"ZipMod::walk_dir", "ZipMod.cpp", 56);
				return false;
			}
		}
		else {
			zip_source_t* source = zip_source_file(zipper, path.c_str(), 0, 0);
			if (source == nullptr) {
				//throw std::runtime_error("[YHMA] Failed to add file to zip: " + std::string(zip_strerror(zipper)));
				Godot::print_error(std::format("[YHMA] Failed to add file to zip: {}", std::string(zip_strerror(zipper))).c_str(),
					"ZipMod::walk_dir", "ZipMod.cpp", 66);
				return false;
			}
			if (zip_file_add(zipper, std::string(mod_folder + "/" + path.substr(inputdir.length() + 1)).c_str(), source, ZIP_FL_ENC_UTF_8) < 0) {
				zip_source_free(source);
				//throw std::runtime_error("[YHMA] Failed to add file to zip: " + std::string(zip_strerror(zipper)));
				Godot::print_error(std::format("[YHMA] Failed to add file to zip: {}", std::string(zip_strerror(zipper))).c_str(),
					"ZipMod::walk_dir", "ZipMod.cpp", 73);
				return false;
			}
		}

	}
	return true;
}


bool ZipMod::zipDirectory(String _input_dir, String _output_dir, bool _truncate = true)
{

	const char* input_dir = _input_dir.alloc_c_string();
	const char* output_dir = _output_dir.alloc_c_string();

	zip_t* zipper = NULL;

	int err;
	if (_truncate) {
		zipper = zip_open(output_dir, ZIP_TRUNCATE | ZIP_CREATE, &err);
	}
	else
	{
		zipper = zip_open(output_dir, 0, &err);
	}
	


	if (zipper == NULL) {
		zip_error_t error;
		zip_error_init_with_code(&error, err);
		std::string f = std::format("Cannot open zip archive {} : {}", output_dir, zip_error_strerror(&error));
		Godot::print(f.c_str());
		return false;
	}

	if (not walk_dir(input_dir, input_dir, zipper)) {
		zip_discard(zipper);
		Godot::print("[YHMA] Failed to zip");
		return false;
	}

	Godot::print("[YHMA] Mod Has Been Zipped");
	zip_close(zipper);
	return true;
}




