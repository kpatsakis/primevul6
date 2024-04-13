g_markup_parse_context_new (const GMarkupParser *parser,
                            GMarkupParseFlags    flags,
                            gpointer             user_data,
                            GDestroyNotify       user_data_dnotify)
{
  GMarkupParseContext *context;

  g_return_val_if_fail (parser != NULL, NULL);

  context = g_new (GMarkupParseContext, 1);

  context->ref_count = 1;
  context->parser = parser;
  context->flags = flags;
  context->user_data = user_data;
  context->dnotify = user_data_dnotify;

  context->line_number = 1;
  context->char_number = 1;

  context->partial_chunk = NULL;
  context->spare_chunks = NULL;
  context->spare_list_nodes = NULL;

  context->state = STATE_START;
  context->tag_stack = NULL;
  context->tag_stack_gstr = NULL;
  context->attr_names = NULL;
  context->attr_values = NULL;
  context->cur_attr = -1;
  context->alloc_attrs = 0;

  context->current_text = NULL;
  context->current_text_len = -1;
  context->current_text_end = NULL;

  context->start = NULL;
  context->iter = NULL;

  context->document_empty = TRUE;
  context->parsing = FALSE;

  context->awaiting_pop = FALSE;
  context->subparser_stack = NULL;
  context->subparser_element = NULL;

  /* this is only looked at if awaiting_pop = TRUE.  initialise anyway. */
  context->held_user_data = NULL;

  context->balance = 0;

  return context;
}