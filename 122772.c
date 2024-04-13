TEST(Random, FixedSeed) {
  // clang-format off
  struct ConstantRNG {
    typedef uint32_t result_type;
    result_type operator()() {
      return 4; // chosen by fair dice roll.
                // guaranteed to be random.
    }
    static constexpr result_type min() {
      return std::numeric_limits<result_type>::min();
    }
    static constexpr result_type max() {
      return std::numeric_limits<result_type>::max();
    }
  };
  // clang-format on

  ConstantRNG gen;

  // Pick a constant random number...
  auto value = Random::rand32(10, gen);

  // Loop to make sure it really is constant.
  for (int i = 0; i < 1024; ++i) {
    auto result = Random::rand32(10, gen);
    EXPECT_EQ(value, result);
  }
}