TEST_F(QuotedString_ExtractFrom_Tests, EmptyDoubleQuotedString) {
  whenInputIs("\"\"");

  resultMustBe("");
  trailingMustBe("");
}