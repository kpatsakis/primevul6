TEST_F(QuotedString_ExtractFrom_Tests, EscapedFormfeed) {
  whenInputIs("\"hello \\fworld\\f\"");
  resultMustBe("hello \fworld\f");
}