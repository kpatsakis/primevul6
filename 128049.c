g_markup_parse_context_unref (GMarkupParseContext *context)
{
  g_return_if_fail (context != NULL);
  g_return_if_fail (context->ref_count > 0);

  if (g_atomic_int_dec_and_test (&context->ref_count))
    g_markup_parse_context_free (context);
}