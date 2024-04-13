TEST_F(QuotedString_ExtractFrom_Tests, SingleQuotedStringWithTrailing) {
  whenInputIs("'hello' world");

  resultMustBe("hello");
  trailingMustBe(" world");
}