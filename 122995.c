TEST_F(QuotedString_ExtractFrom_Tests, EscapedDoubleQuote) {
  whenInputIs("\"hello \\\"world\\\"\"");
  resultMustBe("hello \"world\"");
}