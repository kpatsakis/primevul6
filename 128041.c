mark_error (GMarkupParseContext *context,
            GError              *error)
{
  context->state = STATE_ERROR;

  if (context->parser->error)
    (*context->parser->error) (context, error, context->user_data);

  /* report the error all the way up to free all the user-data */
  while (context->subparser_stack)
    {
      pop_subparser_stack (context);
      context->awaiting_pop = FALSE; /* already been freed */

      if (context->parser->error)
        (*context->parser->error) (context, error, context->user_data);
    }
}