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

#include "SizePrefixedEntity.h"
#include <cstring>

namespace catapult { namespace model {

	const uint8_t* SizePrefixedEntity::ToBytePointer() const {
		return reinterpret_cast<const uint8_t*>(this);
	}

	uint8_t* SizePrefixedEntity::ToBytePointer() {
		return reinterpret_cast<uint8_t*>(this);
	}

	bool SizePrefixedEntity::operator==(const SizePrefixedEntity& rhs) const {
		return Size == rhs.Size && 0 == std::memcmp(this, &rhs, Size);
	}

	bool SizePrefixedEntity::operator!=(const SizePrefixedEntity& rhs) const {
		return !(*this == rhs);
	}
}}
