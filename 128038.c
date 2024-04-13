possibly_finish_subparser (GMarkupParseContext *context)
{
  if (current_element (context) == context->subparser_element)
    pop_subparser_stack (context);
}