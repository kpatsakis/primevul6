TEST_F(QuotedString_ExtractFrom_Tests, NoQuotes) {
  whenInputIs("hello world");

  resultMustBe(0);
}