add_to_partial (GMarkupParseContext *context,
                const gchar         *text_start,
                const gchar         *text_end)
{
  if (context->partial_chunk == NULL)
    { /* allocate a new chunk to parse into */

      if (context->spare_chunks != NULL)
        {
          GSList *node = context->spare_chunks;
          context->spare_chunks = g_slist_remove_link (context->spare_chunks, node);
          context->partial_chunk = node->data;
          free_list_node (context, node);
        }
      else
        context->partial_chunk = g_string_sized_new (MAX (28, text_end - text_start));
    }

  if (text_start != text_end)
    g_string_insert_len (context->partial_chunk, -1,
                         text_start, text_end - text_start);
}