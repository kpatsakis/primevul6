lexer_parse_number (parser_context_t *context_p) /**< context */
{
  const uint8_t *source_p = context_p->source_p;
  const uint8_t *source_end_p = context_p->source_end_p;
  bool can_be_float = false;
  size_t length;

  context_p->token.type = LEXER_LITERAL;
  context_p->token.literal_is_reserved = false;
  context_p->token.extra_value = LEXER_NUMBER_DECIMAL;
  context_p->token.lit_location.char_p = source_p;
  context_p->token.lit_location.type = LEXER_NUMBER_LITERAL;
  context_p->token.lit_location.has_escape = false;

  if (source_p[0] == LIT_CHAR_0
      && source_p + 1 < source_end_p)
  {
    if (LEXER_TO_ASCII_LOWERCASE (source_p[1]) == LIT_CHAR_LOWERCASE_X)
    {
      context_p->token.extra_value = LEXER_NUMBER_HEXADECIMAL;
      source_p += 2;

      if (source_p >= source_end_p
          || !lit_char_is_hex_digit (source_p[0]))
      {
        parser_raise_error (context_p, PARSER_ERR_INVALID_HEX_DIGIT);
      }

      do
      {
        source_p++;
      }
      while (source_p < source_end_p
             && lit_char_is_hex_digit (source_p[0]));
    }
    else if (source_p[1] >= LIT_CHAR_0
             && source_p[1] <= LIT_CHAR_7)
    {
      context_p->token.extra_value = LEXER_NUMBER_OCTAL;

      if (context_p->status_flags & PARSER_IS_STRICT)
      {
        parser_raise_error (context_p, PARSER_ERR_OCTAL_NUMBER_NOT_ALLOWED);
      }

      do
      {
        source_p++;
      }
      while (source_p < source_end_p
             && source_p[0] >= LIT_CHAR_0
             && source_p[0] <= LIT_CHAR_7);

      if (source_p < source_end_p
          && source_p[0] >= LIT_CHAR_8
          && source_p[0] <= LIT_CHAR_9)
      {
        parser_raise_error (context_p, PARSER_ERR_INVALID_NUMBER);
      }
    }
    else if (source_p[1] >= LIT_CHAR_8
             && source_p[1] <= LIT_CHAR_9)
    {
      parser_raise_error (context_p, PARSER_ERR_INVALID_NUMBER);
    }
    else
    {
      can_be_float = true;
      source_p++;
    }
  }
  else
  {
    do
    {
      source_p++;
    }
    while (source_p < source_end_p
           && source_p[0] >= LIT_CHAR_0
           && source_p[0] <= LIT_CHAR_9);

    can_be_float = true;
  }

  if (can_be_float)
  {
    if (source_p < source_end_p
        && source_p[0] == LIT_CHAR_DOT)
    {
      source_p++;
      while (source_p < source_end_p
             && source_p[0] >= LIT_CHAR_0
             && source_p[0] <= LIT_CHAR_9)
      {
        source_p++;
      }
    }

    if (source_p < source_end_p
        && LEXER_TO_ASCII_LOWERCASE (source_p[0]) == LIT_CHAR_LOWERCASE_E)
    {
      source_p++;

      if (source_p < source_end_p
          && (source_p[0] == LIT_CHAR_PLUS || source_p[0] == LIT_CHAR_MINUS))
      {
        source_p++;
      }

      if (source_p >= source_end_p
          || source_p[0] < LIT_CHAR_0
          || source_p[0] > LIT_CHAR_9)
      {
        parser_raise_error (context_p, PARSER_ERR_MISSING_EXPONENT);
      }

      do
      {
        source_p++;
      }
      while (source_p < source_end_p
             && source_p[0] >= LIT_CHAR_0
             && source_p[0] <= LIT_CHAR_9);
    }
  }

  if (source_p < source_end_p
      && (lit_char_is_identifier_start (source_p) || source_p[0] == LIT_CHAR_BACKSLASH))
  {
    parser_raise_error (context_p, PARSER_ERR_IDENTIFIER_AFTER_NUMBER);
  }

  length = (size_t) (source_p - context_p->source_p);
  if (length > PARSER_MAXIMUM_IDENT_LENGTH)
  {
    parser_raise_error (context_p, PARSER_ERR_NUMBER_TOO_LONG);
  }

  context_p->token.lit_location.length = (uint16_t) length;
  PARSER_PLUS_EQUAL_LC (context_p->column, length);
  context_p->source_p = source_p;
} /* lexer_parse_number */