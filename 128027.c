g_markup_parse_context_free (GMarkupParseContext *context)
{
  g_return_if_fail (context != NULL);
  g_return_if_fail (!context->parsing);
  g_return_if_fail (!context->subparser_stack);
  g_return_if_fail (!context->awaiting_pop);

  if (context->dnotify)
    (* context->dnotify) (context->user_data);

  clear_attributes (context);
  g_free (context->attr_names);
  g_free (context->attr_values);

  g_slist_free_full (context->tag_stack_gstr, string_full_free);
  g_slist_free (context->tag_stack);

  g_slist_free_full (context->spare_chunks, string_full_free);
  g_slist_free (context->spare_list_nodes);

  if (context->partial_chunk)
    g_string_free (context->partial_chunk, TRUE);

  g_free (context);
}