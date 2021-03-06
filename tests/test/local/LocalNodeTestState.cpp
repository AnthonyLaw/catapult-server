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

#include "LocalNodeTestState.h"
#include "LocalTestUtils.h"
#include "catapult/extensions/LocalNodeChainScore.h"
#include "catapult/io/BlockStorageCache.h"
#include "catapult/state/CatapultState.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/core/mocks/MockMemoryBasedStorage.h"

namespace catapult { namespace test {

	struct LocalNodeTestState::Impl {
	public:
		explicit Impl(config::LocalNodeConfiguration&& config, cache::CatapultCache&& cache)
				: m_config(std::move(config))
				, m_cache(std::move(cache))
				, m_storage(std::make_unique<mocks::MockMemoryBasedStorage>())
		{}

	public:
		extensions::LocalNodeStateRef ref() {
			return extensions::LocalNodeStateRef(m_config, m_state, m_cache, m_storage, m_score);
		}

		extensions::LocalNodeStateConstRef cref() const {
			return extensions::LocalNodeStateConstRef(m_config, m_state, m_cache, m_storage, m_score);
		}

	private:
		config::LocalNodeConfiguration m_config;
		state::CatapultState m_state;
		cache::CatapultCache m_cache;
		io::BlockStorageCache m_storage;
		extensions::LocalNodeChainScore m_score;
	};

	LocalNodeTestState::LocalNodeTestState() : LocalNodeTestState(CreateEmptyCatapultCache())
	{}

	LocalNodeTestState::LocalNodeTestState(const model::BlockChainConfiguration& config)
			: LocalNodeTestState(config, "", CreateEmptyCatapultCache(config))
	{}

	LocalNodeTestState::LocalNodeTestState(cache::CatapultCache&& cache)
			: m_pImpl(std::make_unique<Impl>(CreatePrototypicalLocalNodeConfiguration(), std::move(cache)))
	{}

	LocalNodeTestState::LocalNodeTestState(
			const model::BlockChainConfiguration& config,
			const std::string& userDataDirectory,
			cache::CatapultCache&& cache)
			: m_pImpl(std::make_unique<Impl>(
					LoadLocalNodeConfiguration(model::BlockChainConfiguration(config), userDataDirectory),
					std::move(cache)))
	{}

	LocalNodeTestState::~LocalNodeTestState() = default;

	extensions::LocalNodeStateRef LocalNodeTestState::ref() {
		return m_pImpl->ref();
	}

	extensions::LocalNodeStateConstRef LocalNodeTestState::cref() const {
		return m_pImpl->cref();
	}
}}
