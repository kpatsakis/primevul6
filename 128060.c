current_attribute (GMarkupParseContext *context)
{
  g_assert (context->cur_attr >= 0);
  return context->attr_names[context->cur_attr]->str;
}