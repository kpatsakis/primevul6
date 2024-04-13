  void expectSame(const Buf& buf, const std::string& hex) {
    auto str = buf->moveToFbString().toStdString();
    EXPECT_EQ(hexlify(str), hex);
  }