lexer_expect_object_literal_id (parser_context_t *context_p, /**< context */
                                bool must_be_identifier) /**< only identifiers are accepted */
{
  skip_spaces (context_p);

  context_p->token.line = context_p->line;
  context_p->token.column = context_p->column;

  if (context_p->source_p < context_p->source_end_p)
  {
    bool create_literal_object = false;

    if (lit_char_is_identifier_start (context_p->source_p) || context_p->source_p[0] == LIT_CHAR_BACKSLASH)
    {
      lexer_parse_identifier (context_p, false);

      if (!must_be_identifier
          && context_p->token.lit_location.length == 3)
      {
        skip_spaces (context_p);

        if (context_p->source_p < context_p->source_end_p
            && context_p->source_p[0] != LIT_CHAR_COLON)
        {
          if (lexer_compare_identifier_to_current (context_p, &lexer_get_literal))
          {
            context_p->token.type = LEXER_PROPERTY_GETTER;
            return;
          }
          else if (lexer_compare_identifier_to_current (context_p, &lexer_set_literal))
          {
            context_p->token.type = LEXER_PROPERTY_SETTER;
            return;
          }
        }
      }

      create_literal_object = true;
    }
    else if (context_p->source_p[0] == LIT_CHAR_DOUBLE_QUOTE
             || context_p->source_p[0] == LIT_CHAR_SINGLE_QUOTE)
    {
      lexer_parse_string (context_p);
      create_literal_object = true;
    }
    else if (!must_be_identifier && context_p->source_p[0] == LIT_CHAR_RIGHT_BRACE)
    {
      context_p->token.type = LEXER_RIGHT_BRACE;
      context_p->source_p += 1;
      context_p->column++;
      return;
    }
    else
    {
      const uint8_t *char_p = context_p->source_p;

      if (char_p[0] == LIT_CHAR_DOT)
      {
        char_p++;
      }

      if (char_p < context_p->source_end_p
          && char_p[0] >= LIT_CHAR_0
          && char_p[0] <= LIT_CHAR_9)
      {
        lexer_parse_number (context_p);
        lexer_construct_number_object (context_p, false, false);
        return;
      }
    }

    if (create_literal_object)
    {
      lexer_construct_literal_object (context_p,
                                      &context_p->token.lit_location,
                                      LEXER_STRING_LITERAL);
      return;
    }
  }

  parser_raise_error (context_p, PARSER_ERR_PROPERTY_IDENTIFIER_EXPECTED);
} /* lexer_expect_object_literal_id */