TEST(Random, MultiThreaded) {
  const int n = 100;
  std::vector<uint32_t> seeds(n);
  std::vector<std::thread> threads;
  for (int i = 0; i < n; ++i) {
    threads.push_back(std::thread([i, &seeds] {
      seeds[i] = randomNumberSeed();
    }));
  }
  for (auto& t : threads) {
    t.join();
  }
  std::sort(seeds.begin(), seeds.end());
  for (int i = 0; i < n-1; ++i) {
    EXPECT_LT(seeds[i], seeds[i+1]);
  }
}