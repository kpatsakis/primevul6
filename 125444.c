lexer_construct_number_object (parser_context_t *context_p, /**< context */
                               bool push_number_allowed, /**< push number support is allowed */
                               bool is_negative_number) /**< sign is negative */
{
  parser_list_iterator_t literal_iterator;
  lexer_literal_t *literal_p;
  ecma_number_t num;
  uint32_t literal_index = 0;
  uint16_t length = context_p->token.lit_location.length;

  if (context_p->token.extra_value != LEXER_NUMBER_OCTAL)
  {
    num = ecma_utf8_string_to_number (context_p->token.lit_location.char_p,
                                      length);
  }
  else
  {
    const uint8_t *src_p = context_p->token.lit_location.char_p;
    const uint8_t *src_end_p = src_p + length - 1;

    num = 0;
    do
    {
      src_p++;
      num = num * 8 + (ecma_number_t) (*src_p - LIT_CHAR_0);
    }
    while (src_p < src_end_p);
  }

  if (push_number_allowed)
  {
    int32_t int_num = (int32_t) num;

    if (int_num == num)
    {
      if (int_num <= CBC_PUSH_NUMBER_BYTE_RANGE_END
          && (int_num != 0 || !is_negative_number))
      {
        context_p->lit_object.index = (uint16_t) int_num;
        return true;
      }
    }
  }

  if (is_negative_number)
  {
    num = -num;
  }

  jmem_cpointer_t lit_cp = ecma_find_or_create_literal_number (num);
  parser_list_iterator_init (&context_p->literal_pool, &literal_iterator);

  while ((literal_p = (lexer_literal_t *) parser_list_iterator_next (&literal_iterator)) != NULL)
  {
    if (literal_p->type == LEXER_NUMBER_LITERAL
        && literal_p->u.value == lit_cp)
    {
      context_p->lit_object.literal_p = literal_p;
      context_p->lit_object.index = (uint16_t) literal_index;
      context_p->lit_object.type = LEXER_LITERAL_OBJECT_ANY;
      return false;
    }

    literal_index++;
  }

  JERRY_ASSERT (literal_index == context_p->literal_count);

  if (literal_index >= PARSER_MAXIMUM_NUMBER_OF_LITERALS)
  {
    parser_raise_error (context_p, PARSER_ERR_LITERAL_LIMIT_REACHED);
  }

  literal_p = (lexer_literal_t *) parser_list_append (context_p, &context_p->literal_pool);
  literal_p->prop.length = context_p->token.lit_location.length;
  literal_p->type = LEXER_UNUSED_LITERAL;
  literal_p->status_flags = 0;

  context_p->literal_count++;

  literal_p->u.value = lit_cp;
  literal_p->type = LEXER_NUMBER_LITERAL;

  context_p->lit_object.literal_p = literal_p;
  context_p->lit_object.index = (uint16_t) literal_index;
  context_p->lit_object.type = LEXER_LITERAL_OBJECT_ANY;

  return false;
} /* lexer_construct_number_object */