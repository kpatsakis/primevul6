TEST_F(QuotedString_ExtractFrom_Tests, EscapedCarriageReturn) {
  whenInputIs("\"hello \\rworld\\r\"");
  resultMustBe("hello \rworld\r");
}