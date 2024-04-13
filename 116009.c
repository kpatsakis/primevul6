  MockArgFormatter(fmt::BasicFormatter<char, MockArgFormatter> &f,
                   fmt::FormatSpec &s, const char *)
    : fmt::internal::ArgFormatterBase<MockArgFormatter, char>(f.writer(), s) {
    EXPECT_CALL(*this, visit_int(42));
  }