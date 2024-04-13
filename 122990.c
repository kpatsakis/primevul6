TEST_F(QuotedString_ExtractFrom_Tests, DoubleQuotedStringWithTrailing) {
  whenInputIs("\"hello\" world");

  resultMustBe("hello");
  trailingMustBe(" world");
}