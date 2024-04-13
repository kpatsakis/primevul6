TEST(Random, StateSize) {
  using namespace folly::detail;

  // uint_fast32_t is uint64_t on x86_64, w00t
  EXPECT_EQ(
      sizeof(uint_fast32_t) / 4 + 3, StateSizeT<std::minstd_rand0>::value);
  EXPECT_EQ(624, StateSizeT<std::mt19937>::value);
#if FOLLY_HAVE_EXTRANDOM_SFMT19937
  EXPECT_EQ(624, StateSizeT<__gnu_cxx::sfmt19937>::value);
#endif
  EXPECT_EQ(24, StateSizeT<std::ranlux24_base>::value);
}