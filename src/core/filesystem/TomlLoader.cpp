// Created by block on 2/3/24.

#include "TomlLoader.hpp"

namespace engine::core::filesystem {

	toml::table TomlLoader::LoadTable(const std::filesystem::path &path) {
		toml::table table;

		try {
			table = toml::parse_file(path.c_str());
		}
		catch (const toml::parse_error &err) {
			if (err.description() == "File could not be opened for reading")
				LogWarning("File \"{}\" was not found.", path.string());
			else
				LogWarning("Couldn't load table! \"{}\" Using defaults.", err.description());
			return {};
		}

		return table;
	}

} // namespace engine::core::filesystem
