TEST(FormatterTest, FormatStringFromSpeedTest) {
  EXPECT_EQ("1.2340000000:0042:+3.13:str:0x3e8:X:%",
      format("{0:0.10f}:{1:04}:{2:+g}:{3}:{4}:{5}:%",
          1.234, 42, 3.13, "str", reinterpret_cast<void*>(1000), 'X'));
}