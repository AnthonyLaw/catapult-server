#include "catapult/consumers/InputUtils.h"
#include "tests/catapult/consumers/test/ConsumerTestUtils.h"
#include "tests/test/core/BlockTestUtils.h"
#include "tests/test/core/TransactionInfoTestUtils.h"
#include "tests/TestHarness.h"

using catapult::disruptor::ConsumerInput;

namespace catapult { namespace consumers {

#define TEST_CLASS InputUtilsTests

	namespace {
		auto CreateMultiBlockElements() {
			auto pBlock1 = test::GenerateBlockWithTransactionsAtHeight(1, 246);
			auto pBlock2 = test::GenerateBlockWithTransactionsAtHeight(0, 247);
			auto pBlock3 = test::GenerateBlockWithTransactionsAtHeight(3, 248);
			auto pBlock4 = test::GenerateBlockWithTransactionsAtHeight(2, 249);
			return test::CreateBlockElements({ pBlock1.get(), pBlock2.get(), pBlock3.get(), pBlock4.get() });
		}
	}

	// region ExtractTransactionHashes

	namespace {
		const Hash256& GetHash(const BlockElements& elements, size_t elementIndex, size_t txIndex) {
			return elements[elementIndex].Transactions[txIndex].EntityHash;
		}

		bool Contains(const utils::HashPointerSet& hashes, const Hash256& hash) {
			auto hashCopy = hash; // use a copy to ensure pointers are not being compared
			return hashes.cend() != hashes.find(&hashCopy);
		}
	}

	TEST(TEST_CLASS, ExtractTransactionHashes_CanExtractAllTransationHashesFromInput) {
		// Arrange:
		auto elements = CreateMultiBlockElements();

		// Act:
		auto hashes = ExtractTransactionHashes(elements);

		// Assert:
		ASSERT_EQ(6u, hashes.size());
		EXPECT_TRUE(Contains(hashes, GetHash(elements, 0, 0)));
		EXPECT_TRUE(Contains(hashes, GetHash(elements, 2, 0)));
		EXPECT_TRUE(Contains(hashes, GetHash(elements, 2, 1)));
		EXPECT_TRUE(Contains(hashes, GetHash(elements, 2, 2)));
		EXPECT_TRUE(Contains(hashes, GetHash(elements, 3, 0)));
		EXPECT_TRUE(Contains(hashes, GetHash(elements, 3, 1)));
	}

	// endregion

	// region ExtractBlocks

	TEST(TEST_CLASS, ExtractBlocks_CanExtractAllBlocksFromInput) {
		// Arrange:
		auto elements = CreateMultiBlockElements();

		// Act:
		auto blocks = ExtractBlocks(elements);

		// Assert:
		EXPECT_EQ(4u, blocks.size());

		for (auto i = 0u; i < blocks.size(); ++i)
			EXPECT_EQ(&elements[i].Block, blocks[i]) << "block at " << i;
	}

	// endregion

	// region ExtractEntityInfos

	namespace {
		void AssertEqual(const model::TransactionElement& expected, const model::WeakEntityInfo& actual, const char* id) {
			// Assert:
			EXPECT_EQ(expected.Transaction, actual.entity()) << "transaction at " << id;
			EXPECT_EQ(expected.EntityHash, actual.hash()) << "transaction at " << id;
		}
	}

	TEST(TEST_CLASS, ExtractEntityInfos_CanExtractAllEntitiesWhenNoneAreSkipped) {
		// Arrange:
		ConsumerInput input(test::CreateTransactionEntityRange(5));
		const auto& elements = input.transactions();

		// Act:
		model::WeakEntityInfos entityInfos;
		std::vector<size_t> entityInfoElementIndexes;
		ExtractEntityInfos(elements, entityInfos, entityInfoElementIndexes);

		// Assert:
		EXPECT_EQ(std::vector<size_t>({ 0, 1, 2, 3, 4 }), entityInfoElementIndexes);
		ASSERT_EQ(5u, entityInfos.size());

		auto i = 0u;
		for (const auto& entityInfo : entityInfos) {
			AssertEqual(elements[i], entityInfo, std::to_string(i).c_str());
			i++;
		}
	}

	TEST(TEST_CLASS, ExtractEntityInfos_CanExtractZeroEntitiesWhenAllAreSkipped) {
		// Arrange:
		ConsumerInput input(test::CreateTransactionEntityRange(5));
		auto& elements = input.transactions();
		for (auto& element : elements)
			element.Skip = true;

		// Act:
		model::WeakEntityInfos entityInfos;
		std::vector<size_t> entityInfoElementIndexes;
		ExtractEntityInfos(elements, entityInfos, entityInfoElementIndexes);

		// Assert:
		EXPECT_TRUE(entityInfoElementIndexes.empty());
		EXPECT_TRUE(entityInfos.empty());
	}

	TEST(TEST_CLASS, ExtractEntityInfos_CanExtractOnlyNonSkippedElementsWhenSomeAreSkipped) {
		// Arrange:
		ConsumerInput input(test::CreateTransactionEntityRange(5));
		auto& elements = input.transactions();
		elements[0].Skip = true;
		elements[2].Skip = true;
		elements[3].Skip = true;

		// Act:
		model::WeakEntityInfos entityInfos;
		std::vector<size_t> entityInfoElementIndexes;
		ExtractEntityInfos(elements, entityInfos, entityInfoElementIndexes);

		// Assert:
		EXPECT_EQ(std::vector<size_t>({ 1, 4 }), entityInfoElementIndexes);
		ASSERT_EQ(2u, entityInfos.size());

		AssertEqual(elements[1], entityInfos[0], "0");
		AssertEqual(elements[4], entityInfos[1], "1");
	}

	// endregion

	// region CollectRevertedTransactionInfos

	TEST(TEST_CLASS, CollectRevertedTransactionInfos_ReturnsNoInfosWhenHashesMatchAllTransactions) {
		// Arrange:
		auto transactionInfos = test::CreateTransactionInfos(4);

		// Act:
		auto revertedTransactionsInfos = CollectRevertedTransactionInfos(
				{
					&transactionInfos[0].EntityHash,
					&transactionInfos[1].EntityHash,
					&transactionInfos[2].EntityHash,
					&transactionInfos[3].EntityHash
				},
				test::CopyTransactionInfos(transactionInfos));

		// Assert:
		EXPECT_TRUE(revertedTransactionsInfos.empty());
	}

	TEST(TEST_CLASS, CollectRevertedTransactionInfos_ReturnsAllInfosWhenHashesMatchNoTransactions) {
		// Arrange:
		auto transactionInfos = test::CreateTransactionInfos(4);
		auto hash1 = test::GenerateRandomData<Hash256_Size>();
		auto hash2 = test::GenerateRandomData<Hash256_Size>();

		// Act:
		auto revertedTransactionsInfos = CollectRevertedTransactionInfos({ &hash1, &hash2 }, test::CopyTransactionInfos(transactionInfos));

		// Assert:
		ASSERT_EQ(4u, revertedTransactionsInfos.size());
		for (auto i = 0u; i < revertedTransactionsInfos.size(); ++i)
			test::AssertEqual(transactionInfos[i], revertedTransactionsInfos[i], "reverted info " + std::to_string(i));
	}

	TEST(TEST_CLASS, CollectRevertedTransactionInfos_OnlyReturnsInfosWithoutMatchingHashes) {
		// Arrange:
		auto transactionInfos = test::CreateTransactionInfos(4);
		auto hash1 = test::GenerateRandomData<Hash256_Size>();
		auto hash2 = test::GenerateRandomData<Hash256_Size>();

		// Act:
		auto revertedTransactionsInfos = CollectRevertedTransactionInfos(
				{ &hash1, &hash2, &transactionInfos[2].EntityHash, &transactionInfos[0].EntityHash },
				test::CopyTransactionInfos(transactionInfos));

		// Assert:
		ASSERT_EQ(2u, revertedTransactionsInfos.size());
		test::AssertEqual(transactionInfos[1], revertedTransactionsInfos[0], "reverted info 0");
		test::AssertEqual(transactionInfos[3], revertedTransactionsInfos[1], "reverted info 1");
	}

	// endregion
}}
