lexer_construct_literal_object (parser_context_t *context_p, /**< context */
                                lexer_lit_location_t *literal_p, /**< literal location */
                                uint8_t literal_type) /**< final literal type */
{
  uint8_t *destination_start_p;
  const uint8_t *source_p;
  uint8_t local_byte_array[LEXER_MAX_LITERAL_LOCAL_BUFFER_SIZE];

  JERRY_ASSERT (literal_p->type == LEXER_IDENT_LITERAL
                || literal_p->type == LEXER_STRING_LITERAL);
  JERRY_ASSERT (context_p->allocated_buffer_p == NULL);

  destination_start_p = local_byte_array;
  source_p = literal_p->char_p;

  if (literal_p->has_escape)
  {
    uint8_t *destination_p;

    if (literal_p->length > LEXER_MAX_LITERAL_LOCAL_BUFFER_SIZE)
    {
      destination_start_p = (uint8_t *) parser_malloc_local (context_p, literal_p->length);
      context_p->allocated_buffer_p = destination_start_p;
      context_p->allocated_buffer_size = literal_p->length;
    }

    destination_p = destination_start_p;

    if (literal_p->type == LEXER_IDENT_LITERAL)
    {
      const uint8_t *source_end_p = context_p->source_end_p;

      JERRY_ASSERT (literal_p->length <= PARSER_MAXIMUM_IDENT_LENGTH);

      do
      {
        if (*source_p == LIT_CHAR_BACKSLASH)
        {
          destination_p += lit_char_to_utf8_bytes (destination_p,
                                                   lexer_hex_to_character (context_p, source_p + 2, 4));
          source_p += 6;
          continue;
        }

        *destination_p++ = *source_p++;

        while (source_p < source_end_p
               && IS_UTF8_INTERMEDIATE_OCTET (*source_p))
        {
          *destination_p++ = *source_p++;
        }
      }
      while (source_p < source_end_p
             && (lit_char_is_identifier_part (source_p) || *source_p == LIT_CHAR_BACKSLASH));

      JERRY_ASSERT (destination_p == destination_start_p + literal_p->length);
    }
    else
    {
      uint8_t str_end_character = source_p[-1];

      while (true)
      {
        if (*source_p == str_end_character)
        {
          break;
        }

        if (*source_p == LIT_CHAR_BACKSLASH)
        {
          uint8_t conv_character;

          source_p++;
          JERRY_ASSERT (source_p < context_p->source_end_p);

          /* Newline is ignored. */
          if (*source_p == LIT_CHAR_CR
              || *source_p == LIT_CHAR_LF
              || (*source_p == LEXER_NEWLINE_LS_PS_BYTE_1 && LEXER_NEWLINE_LS_PS_BYTE_23 (source_p)))
          {
            if (*source_p == LIT_CHAR_CR)
            {
              source_p++;
              JERRY_ASSERT (source_p < context_p->source_end_p);

              if (*source_p == LIT_CHAR_LF)
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
            continue;
          }

          if (*source_p >= LIT_CHAR_0 && *source_p <= LIT_CHAR_3)
          {
            uint32_t octal_number = (uint32_t) (*source_p - LIT_CHAR_0);

            source_p++;
            JERRY_ASSERT (source_p < context_p->source_end_p);

            if (*source_p >= LIT_CHAR_0 && *source_p <= LIT_CHAR_7)
            {
              octal_number = octal_number * 8 + (uint32_t) (*source_p - LIT_CHAR_0);
              source_p++;
              JERRY_ASSERT (source_p < context_p->source_end_p);

              if (*source_p >= LIT_CHAR_0 && *source_p <= LIT_CHAR_7)
              {
                octal_number = octal_number * 8 + (uint32_t) (*source_p - LIT_CHAR_0);
                source_p++;
                JERRY_ASSERT (source_p < context_p->source_end_p);
              }
            }

            destination_p += lit_char_to_utf8_bytes (destination_p, (uint16_t) octal_number);
            continue;
          }

          if (*source_p >= LIT_CHAR_4 && *source_p <= LIT_CHAR_7)
          {
            uint32_t octal_number = (uint32_t) (*source_p - LIT_CHAR_0);

            source_p++;
            JERRY_ASSERT (source_p < context_p->source_end_p);

            if (*source_p >= LIT_CHAR_0 && *source_p <= LIT_CHAR_7)
            {
              octal_number = octal_number * 8 + (uint32_t) (*source_p - LIT_CHAR_0);
              source_p++;
              JERRY_ASSERT (source_p < context_p->source_end_p);
            }

            *destination_p++ = (uint8_t) octal_number;
            continue;
          }

          if (*source_p == LIT_CHAR_LOWERCASE_X || *source_p == LIT_CHAR_LOWERCASE_U)
          {
            int hex_part_length = (*source_p == LIT_CHAR_LOWERCASE_X) ? 2 : 4;
            JERRY_ASSERT (source_p + 1 + hex_part_length <= context_p->source_end_p);

            destination_p += lit_char_to_utf8_bytes (destination_p,
                                                     lexer_hex_to_character (context_p,
                                                                             source_p + 1,
                                                                             hex_part_length));
            source_p += hex_part_length + 1;
            continue;
          }

          conv_character = *source_p;
          switch (*source_p)
          {
            case LIT_CHAR_LOWERCASE_B:
            {
              conv_character = 0x08;
              break;
            }
            case LIT_CHAR_LOWERCASE_T:
            {
              conv_character = 0x09;
              break;
            }
            case LIT_CHAR_LOWERCASE_N:
            {
              conv_character = 0x0a;
              break;
            }
            case LIT_CHAR_LOWERCASE_V:
            {
              conv_character = 0x0b;
              break;
            }
            case LIT_CHAR_LOWERCASE_F:
            {
              conv_character = 0x0c;
              break;
            }
            case LIT_CHAR_LOWERCASE_R:
            {
              conv_character = 0x0d;
              break;
            }
          }

          if (conv_character != *source_p)
          {
            *destination_p++ = conv_character;
            source_p++;
            continue;
          }
        }

        if (*source_p >= LEXER_UTF8_4BYTE_START)
        {
          /* Processing 4 byte unicode sequence (even if it is
           * after a backslash). Always converted to two 3 byte
           * long sequence. */

          uint32_t character = ((((uint32_t) source_p[0]) & 0x7) << 18);
          character |= ((((uint32_t) source_p[1]) & LIT_UTF8_LAST_6_BITS_MASK) << 12);
          character |= ((((uint32_t) source_p[2]) & LIT_UTF8_LAST_6_BITS_MASK) << 6);
          character |= (((uint32_t) source_p[3]) & LIT_UTF8_LAST_6_BITS_MASK);

          JERRY_ASSERT (character >= 0x10000);
          character -= 0x10000;
          destination_p += lit_char_to_utf8_bytes (destination_p,
                                                   (ecma_char_t) (0xd800 | (character >> 10)));
          destination_p += lit_char_to_utf8_bytes (destination_p,
                                                   (ecma_char_t) (0xdc00 | (character & LIT_UTF16_LAST_10_BITS_MASK)));
          source_p += 4;
          continue;
        }

        *destination_p++ = *source_p++;

        /* There is no need to check the source_end_p
         * since the string is terminated by a quotation mark. */
        while (IS_UTF8_INTERMEDIATE_OCTET (*source_p))
        {
          *destination_p++ = *source_p++;
        }
      }

      JERRY_ASSERT (destination_p == destination_start_p + literal_p->length);
    }

    source_p = destination_start_p;
  }

  lexer_process_char_literal (context_p,
                              source_p,
                              literal_p->length,
                              literal_type,
                              literal_p->has_escape);

  context_p->lit_object.type = LEXER_LITERAL_OBJECT_ANY;

  if (literal_type == LEXER_IDENT_LITERAL)
  {
    if ((context_p->status_flags & PARSER_INSIDE_WITH)
        && context_p->lit_object.literal_p->type == LEXER_IDENT_LITERAL)
    {
      context_p->lit_object.literal_p->status_flags |= LEXER_FLAG_NO_REG_STORE;
    }

    if (literal_p->length == 4
        && source_p[0] == LIT_CHAR_LOWERCASE_E
        && source_p[3] == LIT_CHAR_LOWERCASE_L
        && source_p[1] == LIT_CHAR_LOWERCASE_V
        && source_p[2] == LIT_CHAR_LOWERCASE_A)
    {
      context_p->lit_object.type = LEXER_LITERAL_OBJECT_EVAL;
    }

    if (literal_p->length == 9
        && source_p[0] == LIT_CHAR_LOWERCASE_A
        && source_p[8] == LIT_CHAR_LOWERCASE_S
        && memcmp (source_p + 1, "rgument", 7) == 0)
    {
      context_p->lit_object.type = LEXER_LITERAL_OBJECT_ARGUMENTS;
      if (!(context_p->status_flags & PARSER_ARGUMENTS_NOT_NEEDED))
      {
        context_p->status_flags |= PARSER_ARGUMENTS_NEEDED | PARSER_LEXICAL_ENV_NEEDED;
        context_p->lit_object.literal_p->status_flags |= LEXER_FLAG_NO_REG_STORE;
      }
    }
  }

  if (destination_start_p != local_byte_array)
  {
    JERRY_ASSERT (context_p->allocated_buffer_p == destination_start_p);

    context_p->allocated_buffer_p = NULL;
    parser_free_local (destination_start_p,
                       context_p->allocated_buffer_size);
  }

  JERRY_ASSERT (context_p->allocated_buffer_p == NULL);
} /* lexer_construct_literal_object */