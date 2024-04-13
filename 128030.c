g_markup_parse_context_get_position (GMarkupParseContext *context,
                                     gint                *line_number,
                                     gint                *char_number)
{
  g_return_if_fail (context != NULL);

  if (line_number)
    *line_number = context->line_number;

  if (char_number)
    *char_number = context->char_number;
}