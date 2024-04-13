lexer_construct_regexp_object (parser_context_t *context_p, /**< context */
                               bool parse_only) /**< parse only */
{
#ifndef CONFIG_DISABLE_REGEXP_BUILTIN
  const uint8_t *source_p = context_p->source_p;
  const uint8_t *regex_start_p = context_p->source_p;
  const uint8_t *regex_end_p = regex_start_p;
  const uint8_t *source_end_p = context_p->source_end_p;
  parser_line_counter_t column = context_p->column;
  lexer_literal_t *literal_p;
  bool in_class = false;
  uint16_t current_flags;
  lit_utf8_size_t length;

  JERRY_ASSERT (context_p->token.type == LEXER_DIVIDE
                || context_p->token.type == LEXER_ASSIGN_DIVIDE);

  if (context_p->token.type == LEXER_ASSIGN_DIVIDE)
  {
    regex_start_p--;
  }

  while (true)
  {
    if (source_p >= source_end_p)
    {
      parser_raise_error (context_p, PARSER_ERR_UNTERMINATED_REGEXP);
    }

    if (!in_class && source_p[0] == LIT_CHAR_SLASH)
    {
      regex_end_p = source_p;
      source_p++;
      column++;
      break;
    }

    switch (source_p[0])
    {
      case LIT_CHAR_CR:
      case LIT_CHAR_LF:
      case LEXER_NEWLINE_LS_PS_BYTE_1:
      {
        if (source_p[0] != LEXER_NEWLINE_LS_PS_BYTE_1
            || LEXER_NEWLINE_LS_PS_BYTE_23 (source_p))
        {
          parser_raise_error (context_p, PARSER_ERR_NEWLINE_NOT_ALLOWED);
        }
        break;
      }
      case LIT_CHAR_TAB:
      {
        column = align_column_to_tab (column);
         /* Subtract -1 because column is increased below. */
        column--;
        break;
      }
      case LIT_CHAR_LEFT_SQUARE:
      {
        in_class = true;
        break;
      }
      case LIT_CHAR_RIGHT_SQUARE:
      {
        in_class = false;
        break;
      }
      case LIT_CHAR_BACKSLASH:
      {
        if (source_p + 1 >= source_end_p)
        {
          parser_raise_error (context_p, PARSER_ERR_UNTERMINATED_REGEXP);
        }

        if (source_p[1] >= 0x20 && source_p[1] <= LIT_UTF8_1_BYTE_CODE_POINT_MAX)
        {
          source_p++;
          column++;
        }
      }
    }

    source_p++;
    column++;

    while (source_p < source_end_p
           && IS_UTF8_INTERMEDIATE_OCTET (source_p[0]))
    {
      source_p++;
    }
  }

  current_flags = 0;
  while (source_p < source_end_p)
  {
    uint32_t flag = 0;

    if (source_p[0] == LIT_CHAR_LOWERCASE_G)
    {
      flag = RE_FLAG_GLOBAL;
    }
    else if (source_p[0] == LIT_CHAR_LOWERCASE_I)
    {
      flag = RE_FLAG_IGNORE_CASE;
    }
    else if (source_p[0] == LIT_CHAR_LOWERCASE_M)
    {
      flag = RE_FLAG_MULTILINE;
    }

    if (flag == 0)
    {
      break;
    }

    if (current_flags & flag)
    {
      parser_raise_error (context_p, PARSER_ERR_DUPLICATED_REGEXP_FLAG);
    }

    current_flags = (uint16_t) (current_flags | flag);
    source_p++;
    column++;
  }

  if (source_p < source_end_p
      && lit_char_is_identifier_part (source_p))
  {
    parser_raise_error (context_p, PARSER_ERR_UNKNOWN_REGEXP_FLAG);
  }

  context_p->source_p = source_p;
  context_p->column = column;

  length = (lit_utf8_size_t) (regex_end_p - regex_start_p);
  if (length > PARSER_MAXIMUM_STRING_LENGTH)
  {
    parser_raise_error (context_p, PARSER_ERR_REGEXP_TOO_LONG);
  }

  context_p->column = column;
  context_p->source_p = source_p;

  if (parse_only)
  {
    return;
  }

  if (context_p->literal_count >= PARSER_MAXIMUM_NUMBER_OF_LITERALS)
  {
    parser_raise_error (context_p, PARSER_ERR_LITERAL_LIMIT_REACHED);
  }

  literal_p = (lexer_literal_t *) parser_list_append (context_p, &context_p->literal_pool);
  literal_p->prop.length = (uint16_t) length;
  literal_p->type = LEXER_UNUSED_LITERAL;
  literal_p->status_flags = 0;

  context_p->literal_count++;

  /* Compile the RegExp literal and store the RegExp bytecode pointer */
  const re_compiled_code_t *re_bytecode_p = NULL;
  ecma_value_t completion_value;

  ecma_string_t *pattern_str_p = ecma_new_ecma_string_from_utf8 (regex_start_p, length);
  completion_value = re_compile_bytecode (&re_bytecode_p,
                                          pattern_str_p,
                                          current_flags);
  ecma_deref_ecma_string (pattern_str_p);

  bool is_throw = ECMA_IS_VALUE_ERROR (completion_value);

  ecma_free_value (completion_value);

  if (is_throw)
  {
    parser_raise_error (context_p, PARSER_ERR_INVALID_REGEXP);
  }

  literal_p->type = LEXER_REGEXP_LITERAL;
  literal_p->u.bytecode_p = (ecma_compiled_code_t *) re_bytecode_p;

  context_p->token.type = LEXER_LITERAL;
  context_p->token.literal_is_reserved = false;
  context_p->token.lit_location.type = LEXER_REGEXP_LITERAL;

  context_p->lit_object.literal_p = literal_p;
  context_p->lit_object.index = (uint16_t) (context_p->literal_count - 1);
  context_p->lit_object.type = LEXER_LITERAL_OBJECT_ANY;
#else /* CONFIG_DISABLE_REGEXP_BUILTIN */
  JERRY_UNUSED (parse_only);
  parser_raise_error (context_p, PARSER_ERR_UNSUPPORTED_REGEXP);
#endif /* !CONFIG_DISABLE_REGEXP_BUILTIN */
} /* lexer_construct_regexp_object */