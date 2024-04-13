TEST_F(QuotedString_ExtractFrom_Tests, UnterminatedEscapeSequence) {
  whenInputIs("\"\\\0\"", 4);
  resultMustBe(0);
}