lexer_parse_string (parser_context_t *context_p) /**< context */
{
  uint8_t str_end_character = context_p->source_p[0];
  const uint8_t *source_p = context_p->source_p + 1;
  const uint8_t *string_start_p = source_p;
  const uint8_t *source_end_p = context_p->source_end_p;
  parser_line_counter_t line = context_p->line;
  parser_line_counter_t column = (parser_line_counter_t) (context_p->column + 1);
  parser_line_counter_t original_line = line;
  parser_line_counter_t original_column = column;
  size_t length = 0;
  uint8_t has_escape = false;

  while (true)
  {
    if (source_p >= source_end_p)
    {
      context_p->token.line = original_line;
      context_p->token.column = (parser_line_counter_t) (original_column - 1);
      parser_raise_error (context_p, PARSER_ERR_UNTERMINATED_STRING);
    }

    if (*source_p == str_end_character)
    {
      break;
    }

    if (*source_p == LIT_CHAR_BACKSLASH)
    {
      source_p++;
      column++;
      if (source_p >= source_end_p)
      {
        /* Will throw an unterminated string error. */
        continue;
      }

      has_escape = true;

      /* Newline is ignored. */
      if (*source_p == LIT_CHAR_CR
          || *source_p == LIT_CHAR_LF
          || (*source_p == LEXER_NEWLINE_LS_PS_BYTE_1 && LEXER_NEWLINE_LS_PS_BYTE_23 (source_p)))
      {
        if (*source_p == LIT_CHAR_CR)
        {
          source_p++;
          if (source_p < source_end_p
              && *source_p == LIT_CHAR_LF)
          {
            source_p++;
          }
        }
        else if (*source_p == LIT_CHAR_LF)
        {
          source_p++;
        }
        else
        {
          source_p += 3;
        }

        line++;
        column = 1;
        continue;
      }

      /* Except \x, \u, and octal numbers, everything is
       * converted to a character which has the same byte length. */
      if (*source_p >= LIT_CHAR_0 && *source_p <= LIT_CHAR_3)
      {
        if (context_p->status_flags & PARSER_IS_STRICT)
        {
          parser_raise_error (context_p, PARSER_ERR_OCTAL_ESCAPE_NOT_ALLOWED);
        }

        source_p++;
        column++;

        if (source_p < source_end_p && *source_p >= LIT_CHAR_0 && *source_p <= LIT_CHAR_7)
        {
          source_p++;
          column++;

          if (source_p < source_end_p && *source_p >= LIT_CHAR_0 && *source_p <= LIT_CHAR_7)
          {
            /* Numbers >= 0x200 (0x80) requires
             * two bytes for encoding in UTF-8. */
            if (source_p[-2] >= LIT_CHAR_2)
            {
              length++;
            }

            source_p++;
            column++;
          }
        }

        length++;
        continue;
      }

      if (*source_p >= LIT_CHAR_4 && *source_p <= LIT_CHAR_7)
      {
        if (context_p->status_flags & PARSER_IS_STRICT)
        {
          parser_raise_error (context_p, PARSER_ERR_OCTAL_ESCAPE_NOT_ALLOWED);
        }

        source_p++;
        column++;

        if (source_p < source_end_p && *source_p >= LIT_CHAR_0 && *source_p <= LIT_CHAR_7)
        {
          source_p++;
          column++;
        }

        /* The maximum number is 0x4d so the UTF-8
         * representation is always one byte. */
        length++;
        continue;
      }

      if (*source_p == LIT_CHAR_LOWERCASE_X || *source_p == LIT_CHAR_LOWERCASE_U)
      {
        uint8_t hex_part_length = (*source_p == LIT_CHAR_LOWERCASE_X) ? 2 : 4;

        context_p->token.line = line;
        context_p->token.column = (parser_line_counter_t) (column - 1);
        if (source_p + 1 + hex_part_length > source_end_p)
        {
          parser_raise_error (context_p, PARSER_ERR_INVALID_ESCAPE_SEQUENCE);
        }

        length += lit_char_get_utf8_length (lexer_hex_to_character (context_p,
                                                                    source_p + 1,
                                                                    hex_part_length));
        source_p += hex_part_length + 1;
        PARSER_PLUS_EQUAL_LC (column, hex_part_length + 1u);
        continue;
      }
    }

    if (*source_p >= LEXER_UTF8_4BYTE_START)
    {
      /* Processing 4 byte unicode sequence (even if it is
       * after a backslash). Always converted to two 3 byte
       * long sequence. */
      length += 2 * 3;
      has_escape = true;
      source_p += 4;
      column++;
      continue;
    }
    else if (*source_p == LIT_CHAR_CR
             || *source_p == LIT_CHAR_LF
             || (*source_p == LEXER_NEWLINE_LS_PS_BYTE_1 && LEXER_NEWLINE_LS_PS_BYTE_23 (source_p)))
    {
      context_p->token.line = line;
      context_p->token.column = column;
      parser_raise_error (context_p, PARSER_ERR_NEWLINE_NOT_ALLOWED);
    }
    else if (*source_p == LIT_CHAR_TAB)
    {
      column = align_column_to_tab (column);
      /* Subtract -1 because column is increased below. */
      column--;
    }

    source_p++;
    column++;
    length++;

    while (source_p < source_end_p
           && IS_UTF8_INTERMEDIATE_OCTET (*source_p))
    {
      source_p++;
      length++;
    }
  }

  if (length > PARSER_MAXIMUM_STRING_LENGTH)
  {
    parser_raise_error (context_p, PARSER_ERR_STRING_TOO_LONG);
  }

  context_p->token.type = LEXER_LITERAL;

  /* Fill literal data. */
  context_p->token.lit_location.char_p = string_start_p;
  context_p->token.lit_location.length = (uint16_t) length;
  context_p->token.lit_location.type = LEXER_STRING_LITERAL;
  context_p->token.lit_location.has_escape = has_escape;

  context_p->source_p = source_p + 1;
  context_p->line = line;
  context_p->column = (parser_line_counter_t) (column + 1);
} /* lexer_parse_string */