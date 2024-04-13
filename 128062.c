g_markup_parse_context_get_element_stack (GMarkupParseContext *context)
{
  g_return_val_if_fail (context != NULL, NULL);
  return context->tag_stack;
}