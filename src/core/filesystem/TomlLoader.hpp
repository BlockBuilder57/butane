// Created by block on 2/3/24.

#pragma once

#include <magic_enum.hpp>
#include <toml++/toml.hpp>
#include <filesystem>
#include <core/Logger.hpp>

namespace engine::core::filesystem {

	struct TomlLoader {
		static toml::table LoadTable(const std::filesystem::path& path);

		template<typename T>
		static T GetEnum(toml::node_view<toml::node> node, T defaultValue) {
			std::string stringThing = std::string(node.as_string()->value_or(magic_enum::enum_name(defaultValue)));
			auto nodeVal = magic_enum::enum_cast<T>(stringThing, magic_enum::case_insensitive);
			return nodeVal.value_or(defaultValue);
		}
	};

} // namespace engine::core::filesystem
