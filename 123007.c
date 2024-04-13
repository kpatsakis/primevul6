TEST_F(QuotedString_ExtractFrom_Tests, SimpleDoubleQuotedString) {
  whenInputIs("\"hello world\"");

  resultMustBe("hello world");
  trailingMustBe("");
}