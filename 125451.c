lexer_scan_identifier (parser_context_t *context_p, /**< context */
                       bool propety_name) /**< property name */
{
  skip_spaces (context_p);
  context_p->token.line = context_p->line;
  context_p->token.column = context_p->column;

  if (context_p->source_p < context_p->source_end_p
      && (lit_char_is_identifier_start (context_p->source_p) || context_p->source_p[0] == LIT_CHAR_BACKSLASH))
  {
    lexer_parse_identifier (context_p, false);

    if (propety_name && context_p->token.lit_location.length == 3)
    {
      skip_spaces (context_p);

      if (context_p->source_p < context_p->source_end_p
          && context_p->source_p[0] != LIT_CHAR_COLON)
      {
        if (lexer_compare_identifier_to_current (context_p, &lexer_get_literal))
        {
          context_p->token.type = LEXER_PROPERTY_GETTER;
        }
        else if (lexer_compare_identifier_to_current (context_p, &lexer_set_literal))
        {
          context_p->token.type = LEXER_PROPERTY_SETTER;
        }
      }
    }
    return;
  }

  if (propety_name)
  {
    lexer_next_token (context_p);

    if (context_p->token.type == LEXER_LITERAL
        || context_p->token.type == LEXER_RIGHT_BRACE)
    {
      return;
    }
  }

  parser_raise_error (context_p, PARSER_ERR_IDENTIFIER_EXPECTED);
} /* lexer_scan_identifier */