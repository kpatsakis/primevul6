TEST_F(QuotedString_ExtractFrom_Tests, CurlyBraces) {
  whenInputIs("\"{hello:world}\"");
  resultMustBe("{hello:world}");
}