TEST_F(QuotedString_ExtractFrom_Tests, SquareBraquets) {
  whenInputIs("\"[hello,world]\"");
  resultMustBe("[hello,world]");
}