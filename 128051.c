g_markup_parse_context_ref (GMarkupParseContext *context)
{
  g_return_val_if_fail (context != NULL, NULL);
  g_return_val_if_fail (context->ref_count > 0, NULL);

  g_atomic_int_inc (&context->ref_count);

  return context;
}