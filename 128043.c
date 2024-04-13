pop_subparser_stack (GMarkupParseContext *context)
{
  GMarkupRecursionTracker *tracker;

  g_assert (context->subparser_stack);

  tracker = context->subparser_stack->data;

  context->awaiting_pop = TRUE;
  context->held_user_data = context->user_data;

  context->user_data = tracker->prev_user_data;
  context->parser = tracker->prev_parser;
  context->subparser_element = tracker->prev_element;
  g_slice_free (GMarkupRecursionTracker, tracker);

  context->subparser_stack = g_slist_delete_link (context->subparser_stack,
                                                  context->subparser_stack);
}