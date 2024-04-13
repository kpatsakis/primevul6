lexer_compare_identifier_to_current (parser_context_t *context_p,        /**< context */
                                     const lexer_lit_location_t *right)  /**< identifier */
{
  lexer_lit_location_t *left = &context_p->token.lit_location;
  const uint8_t *left_p;
  const uint8_t *right_p;
  size_t count;

  JERRY_ASSERT (left->length > 0 && right->length > 0);

  if (left->length != right->length)
  {
    return 0;
  }

  if (!left->has_escape && !right->has_escape)
  {
    return memcmp (left->char_p, right->char_p, left->length) == 0;
  }

  left_p = left->char_p;
  right_p = right->char_p;
  count = left->length;

  do
  {
    uint8_t utf8_buf[3];
    size_t utf8_len, offset;

    /* Backslash cannot be part of a multibyte UTF-8 character. */
    if (*left_p != LIT_CHAR_BACKSLASH && *right_p != LIT_CHAR_BACKSLASH)
    {
      if (*left_p++ != *right_p++)
      {
        return false;
      }
      count--;
      continue;
    }

    if (*left_p == LIT_CHAR_BACKSLASH && *right_p == LIT_CHAR_BACKSLASH)
    {
      uint16_t left_chr = lexer_hex_to_character (context_p, left_p, 6);

      if (left_chr != lexer_hex_to_character (context_p, right_p, 6))
      {
        return false;
      }

      left_p += 6;
      right_p += 6;
      count += lit_char_get_utf8_length (left_chr);
      continue;
    }

    /* One character is encoded as unicode sequence. */
    if (*right_p == LIT_CHAR_BACKSLASH)
    {
      /* The pointers can be swapped. */
      const uint8_t *swap_p = left_p;
      left_p = right_p;
      right_p = swap_p;
    }

    utf8_len = lit_char_to_utf8_bytes (utf8_buf, lexer_hex_to_character (context_p, left_p, 6));
    JERRY_ASSERT (utf8_len > 0);
    count -= utf8_len;
    offset = 0;

    do
    {
      if (utf8_buf[offset] != *right_p++)
      {
        return false;
      }
      offset++;
    }
    while (offset < utf8_len);

    left_p += 6;
  }
  while (count > 0);

  return true;
} /* lexer_compare_identifier_to_current */