TEST_F(QuotedString_ExtractFrom_Tests, EscapedSolidus) {
  whenInputIs("\"hello \\/world\\/\"");
  resultMustBe("hello /world/");
}