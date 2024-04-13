TEST_F(QuotedString_ExtractFrom_Tests, EscapedBackspace) {
  whenInputIs("\"hello \\bworld\\b\"");
  resultMustBe("hello \bworld\b");
}