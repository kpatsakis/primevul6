TEST_F(QuotedString_ExtractFrom_Tests, MissingClosingQuote) {
  whenInputIs("\"hello world");

  resultMustBe(0);
}