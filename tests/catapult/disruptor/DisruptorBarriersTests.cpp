#include "catapult/disruptor/DisruptorBarriers.h"
#include "tests/TestHarness.h"

namespace catapult { namespace disruptor {

#define TEST_CLASS DisruptorBarriersTests

	TEST(TEST_CLASS, CanCreateBarriersWithoutBarriers) {
		// Arrange+Act:
		DisruptorBarriers barriers(0);

		// Assert:
		EXPECT_EQ(0u, barriers.size());
	}

	TEST(TEST_CLASS, CanCreateBarriersWithASingleBarrier) {
		// Arrange+Act:
		DisruptorBarriers barriers(1);

		// Assert:
		EXPECT_EQ(1u, barriers.size());
		EXPECT_EQ(0u, barriers[0].position());
		EXPECT_EQ(0u, barriers[0].level());
	}

	TEST(TEST_CLASS, CanCreateBarriersWithMultipleBarriers) {
		// Arrange+Act:
		DisruptorBarriers barriers(10);

		// Assert:
		EXPECT_EQ(10u, barriers.size());
		for (auto i = 0u; i < 10; ++i) {
			EXPECT_EQ(0u, barriers[i].position());
			EXPECT_EQ(i, barriers[i].level());
		}
	}

	TEST(TEST_CLASS, CanAccessBarriersViaConstInterface) {
		// Arrange+Act:
		DisruptorBarriers testedBarriers(10);

		// Assert:
		const auto& barriers = testedBarriers;
		EXPECT_EQ(10u, barriers.size());
		for (auto i = 0u; i < 10; ++i) {
			EXPECT_EQ(0u, barriers[i].position());
			EXPECT_EQ(i, barriers[i].level());
		}
	}
}}
