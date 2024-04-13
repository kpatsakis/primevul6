lexer_expect_identifier (parser_context_t *context_p, /**< context */
                         uint8_t literal_type) /**< literal type */
{
  JERRY_ASSERT (literal_type == LEXER_STRING_LITERAL
                || literal_type == LEXER_IDENT_LITERAL);

  skip_spaces (context_p);
  context_p->token.line = context_p->line;
  context_p->token.column = context_p->column;

  if (context_p->source_p < context_p->source_end_p
      && (lit_char_is_identifier_start (context_p->source_p) || context_p->source_p[0] == LIT_CHAR_BACKSLASH))
  {
    lexer_parse_identifier (context_p, literal_type != LEXER_STRING_LITERAL);

    if (context_p->token.type == LEXER_LITERAL)
    {
      lexer_construct_literal_object (context_p,
                                      &context_p->token.lit_location,
                                      literal_type);

      if (literal_type == LEXER_IDENT_LITERAL
          && (context_p->status_flags & PARSER_IS_STRICT)
          && context_p->lit_object.type != LEXER_LITERAL_OBJECT_ANY)
      {
        parser_error_t error;

        if (context_p->lit_object.type == LEXER_LITERAL_OBJECT_EVAL)
        {
          error = PARSER_ERR_EVAL_NOT_ALLOWED;
        }
        else
        {
          JERRY_ASSERT (context_p->lit_object.type == LEXER_LITERAL_OBJECT_ARGUMENTS);
          error = PARSER_ERR_ARGUMENTS_NOT_ALLOWED;
        }

        parser_raise_error (context_p, error);
      }

      context_p->token.lit_location.type = literal_type;
      return;
    }
  }

  parser_raise_error (context_p, PARSER_ERR_IDENTIFIER_EXPECTED);
} /* lexer_expect_identifier */