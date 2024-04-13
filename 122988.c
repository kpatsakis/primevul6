TEST_F(QuotedString_ExtractFrom_Tests, EscapedSingleQuote) {
  whenInputIs("\"hello \\\'world\\\'\"");
  resultMustBe("hello 'world'");
}