truncate_partial (GMarkupParseContext *context)
{
  if (context->partial_chunk != NULL)
    string_blank (context->partial_chunk);
}