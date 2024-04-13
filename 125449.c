lexer_parse_identifier (parser_context_t *context_p, /**< context */
                        bool check_keywords) /**< check keywords */
{
  /* Only very few identifiers contains \u escape sequences. */
  const uint8_t *source_p = context_p->source_p;
  const uint8_t *ident_start_p = context_p->source_p;
  /* Note: newline or tab cannot be part of an identifier. */
  parser_line_counter_t column = context_p->column;
  const uint8_t *source_end_p = context_p->source_end_p;
  size_t length = 0;

  context_p->token.type = LEXER_LITERAL;
  context_p->token.literal_is_reserved = false;
  context_p->token.lit_location.type = LEXER_IDENT_LITERAL;
  context_p->token.lit_location.has_escape = false;

  do
  {
    if (*source_p == LIT_CHAR_BACKSLASH)
    {
      uint16_t character;

      context_p->token.lit_location.has_escape = true;
      context_p->source_p = source_p;
      context_p->token.column = column;

      if ((source_p + 6 > source_end_p) || (source_p[1] != LIT_CHAR_LOWERCASE_U))
      {
        parser_raise_error (context_p, PARSER_ERR_INVALID_UNICODE_ESCAPE_SEQUENCE);
      }

      character = lexer_hex_to_character (context_p, source_p + 2, 4);

      if (length == 0)
      {
        if (!lit_char_is_identifier_start_character (character))
        {
          parser_raise_error (context_p, PARSER_ERR_INVALID_IDENTIFIER_START);
        }
      }
      else
      {
        if (!lit_char_is_identifier_part_character (character))
        {
          parser_raise_error (context_p, PARSER_ERR_INVALID_IDENTIFIER_PART);
        }
      }

      length += lit_char_get_utf8_length (character);
      source_p += 6;
      PARSER_PLUS_EQUAL_LC (column, 6);
      continue;
    }

    /* Valid identifiers cannot contain 4 byte long utf-8
     * characters, since those characters are represented
     * by 2 ecmascript (UTF-16) characters, and those
     * characters cannot be literal characters. */
    JERRY_ASSERT (source_p[0] < LEXER_UTF8_4BYTE_START);

    source_p++;
    length++;
    column++;

    while (source_p < source_end_p
           && IS_UTF8_INTERMEDIATE_OCTET (source_p[0]))
    {
      source_p++;
      length++;
    }
  }
  while (source_p < source_end_p
         && (lit_char_is_identifier_part (source_p) || *source_p == LIT_CHAR_BACKSLASH));

  context_p->source_p = ident_start_p;
  context_p->token.column = context_p->column;

  if (length > PARSER_MAXIMUM_IDENT_LENGTH)
  {
    parser_raise_error (context_p, PARSER_ERR_IDENTIFIER_TOO_LONG);
  }

  /* Check keywords (Only if there is no \u escape sequence in the pattern). */
  if (check_keywords
      && !context_p->token.lit_location.has_escape
      && (length >= 2 && length <= 10))
  {
    const keyword_string_t *keyword_p = keyword_string_list[length - 2];

    do
    {
      if (ident_start_p[0] == keyword_p->keyword_p[0]
          && ident_start_p[1] == keyword_p->keyword_p[1]
          && memcmp (ident_start_p, keyword_p->keyword_p, length) == 0)
      {
        if (keyword_p->type >= LEXER_FIRST_FUTURE_STRICT_RESERVED_WORD)
        {
          if (context_p->status_flags & PARSER_IS_STRICT)
          {
            parser_raise_error (context_p, PARSER_ERR_STRICT_IDENT_NOT_ALLOWED);
          }

          context_p->token.literal_is_reserved = true;
          break;
        }

        context_p->token.type = keyword_p->type;
        break;
      }
      keyword_p++;
    }
    while (keyword_p->type != LEXER_EOS);
  }

  if (context_p->token.type == LEXER_LITERAL)
  {
    /* Fill literal data. */
    context_p->token.lit_location.char_p = ident_start_p;
    context_p->token.lit_location.length = (uint16_t) length;
  }

  context_p->source_p = source_p;
  context_p->column = column;
} /* lexer_parse_identifier */