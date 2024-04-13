TEST_F(QuotedString_ExtractFrom_Tests, AllEscapedCharsTogether) {
  whenInputIs("\"1\\\"2\\\\3\\/4\\b5\\f6\\n7\\r8\\t9\"");
  resultMustBe("1\"2\\3/4\b5\f6\n7\r8\t9");
}