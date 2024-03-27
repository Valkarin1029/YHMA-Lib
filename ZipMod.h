#pragma once

#include <string>
#include <zip.h>

#include <Godot.hpp>
#include <Node.hpp>

using namespace std;
namespace godot {

class ZipMod : public Node
{
private:
	GODOT_CLASS(ZipMod, Node)

public:
	static void _register_methods();

	ZipMod();
	~ZipMod();

	void _init();
	static bool is_dir(const std::string& dir);
	static bool walk_dir(const std::string& startdir, const std::string& inputdir, zip_t* zipper);
	bool zipDirectory(String _input_dir, String _output_dir);

};

}


