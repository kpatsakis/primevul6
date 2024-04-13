TEST(Random, sanity) {
  // edge cases
  EXPECT_EQ(folly::Random::rand32(0), 0);
  EXPECT_EQ(folly::Random::rand32(12, 12), 0);
  EXPECT_EQ(folly::Random::rand64(0), 0);
  EXPECT_EQ(folly::Random::rand64(12, 12), 0);

  // 32-bit repeatability, uniqueness
  constexpr int kTestSize = 1000;
  {
    std::vector<uint32_t> vals;
    folly::Random::DefaultGenerator rng;
    rng.seed(0xdeadbeef);
    for (int i = 0; i < kTestSize; ++i) {
      vals.push_back(folly::Random::rand32(rng));
    }
    rng.seed(0xdeadbeef);
    for (int i = 0; i < kTestSize; ++i) {
      EXPECT_EQ(vals[i], folly::Random::rand32(rng));
    }
    EXPECT_EQ(
        vals.size(),
        std::unordered_set<uint32_t>(vals.begin(), vals.end()).size());
  }

  // 64-bit repeatability, uniqueness
  {
    std::vector<uint64_t> vals;
    folly::Random::DefaultGenerator rng;
    rng.seed(0xdeadbeef);
    for (int i = 0; i < kTestSize; ++i) {
      vals.push_back(folly::Random::rand64(rng));
    }
    rng.seed(0xdeadbeef);
    for (int i = 0; i < kTestSize; ++i) {
      EXPECT_EQ(vals[i], folly::Random::rand64(rng));
    }
    EXPECT_EQ(
        vals.size(),
        std::unordered_set<uint64_t>(vals.begin(), vals.end()).size());
  }
}