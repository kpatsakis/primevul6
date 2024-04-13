ensure_no_outstanding_subparser (GMarkupParseContext *context)
{
  if (context->awaiting_pop)
    g_critical ("During the first end_element call after invoking a "
                "subparser you must pop the subparser stack and handle "
                "the freeing of the subparser user_data.  This can be "
                "done by calling the end function of the subparser.  "
                "Very probably, your program just leaked memory.");

  /* let valgrind watch the pointer disappear... */
  context->held_user_data = NULL;
  context->awaiting_pop = FALSE;
}