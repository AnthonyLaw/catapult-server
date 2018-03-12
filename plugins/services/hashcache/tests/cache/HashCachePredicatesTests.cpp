#include "src/cache/HashCachePredicates.h"
#include "src/cache/HashCache.h"
#include "tests/test/HashCacheTestUtils.h"
#include "tests/TestHarness.h"

namespace catapult { namespace cache {

#define TEST_CLASS HashCachePredicatesTests

	namespace {
		Hash256 PopulateHashCache(CatapultCache& cache) {
			auto delta = cache.createDelta();
			auto& hashCacheDelta = delta.sub<HashCache>();

			for (auto i = 0u; i < 5u; ++i)
				hashCacheDelta.insert(state::TimestampedHash(Timestamp(i), test::GenerateRandomData<Hash256_Size>()));

			auto hash = test::GenerateRandomData<Hash256_Size>();
			hashCacheDelta.insert(state::TimestampedHash(Timestamp(5), hash));

			for (auto i = 6u; i < 10u; ++i)
				hashCacheDelta.insert(state::TimestampedHash(Timestamp(i), test::GenerateRandomData<Hash256_Size>()));

			cache.commit(Height());
			return hash;
		}
	}

	// region HashCacheContains

	TEST(TEST_CLASS, HashCacheContains_ReturnsTrueIfElementIsContainedInHashCache) {
		// Arrange:
		auto cache = test::HashCacheFactory::Create(model::BlockChainConfiguration::Uninitialized());
		auto hash = PopulateHashCache(cache);

		// Act + Assert:
		EXPECT_TRUE(HashCacheContains(cache, Timestamp(5), hash));
	}

	TEST(TEST_CLASS, HashCacheContains_ReturnsFalseIfElementIsNotContainedInHashCache) {
		// Arrange:
		auto cache = test::HashCacheFactory::Create(model::BlockChainConfiguration::Uninitialized());
		auto hash = PopulateHashCache(cache);

		// Act + Assert:
		EXPECT_FALSE(HashCacheContains(cache, Timestamp(6), hash));
		EXPECT_FALSE(HashCacheContains(cache, Timestamp(5), test::GenerateRandomData<Hash256_Size>()));
	}

	// endregion
}}
