TEST(Random, SecureFork) {
  unsigned char buffer = 0;
  // Init random buffer
  folly::Random::secureRandom(&buffer, 1);

  auto pid = fork();
  EXPECT_NE(pid, -1);
  if (pid) {
    // parent
    int status = 0;
    folly::Random::secureRandom(&buffer, 1);
    auto pid2 = wait(&status);
    EXPECT_NE(WEXITSTATUS(status), buffer);
    EXPECT_EQ(pid, pid2);
  } else {
    // child
    folly::Random::secureRandom(&buffer, 1);
    exit(buffer); // Do not print gtest results
  }
}