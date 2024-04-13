g_markup_parse_context_pop (GMarkupParseContext *context)
{
  gpointer user_data;

  if (!context->awaiting_pop)
    possibly_finish_subparser (context);

  g_assert (context->awaiting_pop);

  context->awaiting_pop = FALSE;

  /* valgrind friendliness */
  user_data = context->held_user_data;
  context->held_user_data = NULL;

  return user_data;
}