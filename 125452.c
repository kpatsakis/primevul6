lexer_construct_function_object (parser_context_t *context_p, /**< context */
                                 uint32_t extra_status_flags) /**< extra status flags */
{
  ecma_compiled_code_t *compiled_code_p;
  lexer_literal_t *literal_p;
  uint16_t result_index;

  if (context_p->literal_count >= PARSER_MAXIMUM_NUMBER_OF_LITERALS)
  {
    parser_raise_error (context_p, PARSER_ERR_LITERAL_LIMIT_REACHED);
  }

  if (context_p->status_flags & PARSER_RESOLVE_THIS_FOR_CALLS)
  {
    extra_status_flags |= PARSER_RESOLVE_THIS_FOR_CALLS;
  }

  literal_p = (lexer_literal_t *) parser_list_append (context_p, &context_p->literal_pool);
  literal_p->type = LEXER_UNUSED_LITERAL;
  literal_p->status_flags = 0;

  result_index = context_p->literal_count;
  context_p->literal_count++;

  compiled_code_p = parser_parse_function (context_p, extra_status_flags);

  literal_p->u.bytecode_p = compiled_code_p;
  literal_p->type = LEXER_FUNCTION_LITERAL;

  return result_index;
} /* lexer_construct_function_object */