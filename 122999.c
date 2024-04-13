TEST_F(QuotedString_ExtractFrom_Tests, EmptySingleQuotedString) {
  whenInputIs("''");

  resultMustBe("");
  trailingMustBe("");
}