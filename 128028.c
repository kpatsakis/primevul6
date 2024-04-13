g_markup_parse_context_push (GMarkupParseContext *context,
                             const GMarkupParser *parser,
                             gpointer             user_data)
{
  GMarkupRecursionTracker *tracker;

  tracker = g_slice_new (GMarkupRecursionTracker);
  tracker->prev_element = context->subparser_element;
  tracker->prev_parser = context->parser;
  tracker->prev_user_data = context->user_data;

  context->subparser_element = current_element (context);
  context->parser = parser;
  context->user_data = user_data;

  context->subparser_stack = g_slist_prepend (context->subparser_stack,
                                              tracker);
}