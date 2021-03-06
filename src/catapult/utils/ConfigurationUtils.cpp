/**
*** Copyright (c) 2016-present,
*** Jaguar0625, gimre, BloodyRookie, Tech Bureau, Corp. All rights reserved.
***
*** This file is part of Catapult.
***
*** Catapult is free software: you can redistribute it and/or modify
*** it under the terms of the GNU Lesser General Public License as published by
*** the Free Software Foundation, either version 3 of the License, or
*** (at your option) any later version.
***
*** Catapult is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*** GNU Lesser General Public License for more details.
***
*** You should have received a copy of the GNU Lesser General Public License
*** along with Catapult. If not, see <http://www.gnu.org/licenses/>.
**/

#include "ConfigurationUtils.h"
#include <cctype>

namespace catapult { namespace utils {

	std::string GetIniPropertyName(const char* cppVariableName) {
		if (!cppVariableName || strlen(cppVariableName) < 2)
			CATAPULT_THROW_INVALID_ARGUMENT("cpp variable name must be at least two characters");

		auto firstChar = cppVariableName[0];
		if (!std::isalpha(firstChar))
			CATAPULT_THROW_INVALID_ARGUMENT("cpp variable name must start with a letter");

		// lowercase the first character
		return static_cast<char>(std::tolower(firstChar)) + std::string(&cppVariableName[1]);
	}

	void VerifyBagSizeLte(const ConfigurationBag& bag, size_t expectedSize) {
		if (bag.size() > expectedSize)
			CATAPULT_THROW_INVALID_ARGUMENT_1("configuration bag contains too many properties", bag.size());
	}

	ConfigurationBag ExtractSectionAsBag(const ConfigurationBag& bag, const char* section) {
		ConfigurationBag::ValuesContainer values;
		values.emplace("", bag.getAll<std::string>(section));
		return ConfigurationBag(std::move(values));
	}

	std::pair<std::unordered_set<std::string>, size_t> ExtractSectionAsUnorderedSet(const ConfigurationBag& bag, const char* section) {
		auto keyValuePairs = bag.getAll<bool>(section);

		std::unordered_set<std::string> enabledKeys;
		for (const auto& pair : keyValuePairs) {
			if (pair.second)
				enabledKeys.emplace(pair.first);
		}

		return std::make_pair(std::move(enabledKeys), keyValuePairs.size());
	}
}}
