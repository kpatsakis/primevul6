current_element (GMarkupParseContext *context)
{
  return context->tag_stack->data;
}