advance_char (GMarkupParseContext *context)
{
  context->iter++;
  context->char_number++;

  if (G_UNLIKELY (context->iter == context->current_text_end))
      return FALSE;

  else if (G_UNLIKELY (*context->iter == '\n'))
    {
      context->line_number++;
      context->char_number = 1;
    }

  return TRUE;
}