TEST_F(QuotedString_ExtractFrom_Tests, EscapedTab) {
  whenInputIs("\"hello \\tworld\\t\"");
  resultMustBe("hello \tworld\t");
}