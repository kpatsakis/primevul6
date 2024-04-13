TEST_F(QuotedString_ExtractFrom_Tests, EscapedReverseSolidus) {
  whenInputIs("\"hello \\\\world\\\\\"");
  resultMustBe("hello \\world\\");
}