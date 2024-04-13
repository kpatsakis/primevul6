g_markup_parse_context_get_element (GMarkupParseContext *context)
{
  g_return_val_if_fail (context != NULL, NULL);

  if (context->tag_stack == NULL)
    return NULL;
  else
    return current_element (context);
}