TEST_F(QuotedString_ExtractFrom_Tests, EscapedNewline) {
  whenInputIs("\"hello \\nworld\\n\"");
  resultMustBe("hello \nworld\n");
}