#include "ZipMod.h"

#include <string>
#include <format>
#include <filesystem>

#include <dirent.h>
#include <sys/stat.h>
#include <zip.h>

#include <Godot.hpp>
#include <Node.hpp>

using namespace std;
using namespace godot;

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
	return std::filesystem::is_directory(dir);
}

bool ZipMod::walk_dir(const std::string& startdir, const std::string& inputdir, zip_t* zipper)
{
	DIR* dp = ::opendir(inputdir.c_str());
	if (dp == nullptr) {
		Godot::print_error(std::format("[YHMA] Failed to open input directory: {}", std::string(::strerror(errno))).c_str(),
			"ZipMod::walk_dir", "ZipMod.cpp", 45);
		return false;
	}

	struct dirent* dirp;
	while ((dirp = readdir(dp)) != NULL) {
		if (dirp->d_name != std::string(".") && dirp->d_name != std::string("..")) {
			std::string fullname = inputdir + "/" + dirp->d_name;
			if (is_dir(fullname)) {
				if (zip_dir_add(zipper, fullname.substr(startdir.length() + 1).c_str(), ZIP_FL_ENC_UTF_8) < 0) {
					//throw std::runtime_error("[YHMA] Failed to add subdirectory to zip: " + std::string(zip_strerror(zipper)));
					Godot::print_error("[YHMA] Failed to add subdirectory to zip",
						"ZipMod::walk_dir", "ZipMod.cpp", 56);
					return false;
				}
				walk_dir(startdir, fullname, zipper);
			}
			else {
				zip_source_t* source = zip_source_file(zipper, fullname.c_str(), 0, 0);
				if (source == nullptr) {
					//throw std::runtime_error("[YHMA] Failed to add file to zip: " + std::string(zip_strerror(zipper)));
					Godot::print_error(std::format("[YHMA] Failed to add file to zip: {}", std::string(zip_strerror(zipper))).c_str(),
						"ZipMod::walk_dir", "ZipMod.cpp", 66);
					return false;
				}
				if (zip_file_add(zipper, fullname.substr(startdir.length() + 1).c_str(), source, ZIP_FL_ENC_UTF_8) < 0) {
					zip_source_free(source);
					//throw std::runtime_error("[YHMA] Failed to add file to zip: " + std::string(zip_strerror(zipper)));
					Godot::print_error(std::format("[YHMA] Failed to add file to zip: {}", std::string(zip_strerror(zipper))).c_str(),
						"ZipMod::walk_dir", "ZipMod.cpp", 73);
					return false;
				}
			}
		}
	}
	::closedir(dp);
	return true;
}


bool ZipMod::zipDirectory(String _input_dir, String _output_dir)
{

	const char* input_dir = _input_dir.alloc_c_string();
	const char* output_dir = _output_dir.alloc_c_string();

	int err;
	zip_t* zipper = zip_open(output_dir, ZIP_TRUNCATE | ZIP_CREATE, &err);

	//Godot::print(std::format("{}", is_dir(input_dir)).c_str());

	if (zipper == NULL) {
		zip_error_t error;
		zip_error_init_with_code(&error, err);
		std::string f = std::format("Cannot open zip archive {} : {}", output_dir, zip_error_strerror(&error));
		Godot::print(f.c_str());
		return false;
	}

	if (not walk_dir(input_dir, input_dir, zipper)) {
		//Godot::print_error("[YHMA] Failed to zip directory", "ZipMod::zipDirectory", "ZipMod.cpp", 102);
		zip_discard(zipper);
		Godot::print("[YHMA] Failed to zip");
		return false;
	}

	Godot::print("[YHMA] Mod Has Been Zipped");
	zip_close(zipper);
	return true;
}


