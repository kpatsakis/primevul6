clear_attributes (GMarkupParseContext *context)
{
  /* Go ahead and free the attributes. */
  for (; context->cur_attr >= 0; context->cur_attr--)
    {
      int pos = context->cur_attr;
      release_chunk (context, context->attr_names[pos]);
      release_chunk (context, context->attr_values[pos]);
      context->attr_names[pos] = context->attr_values[pos] = NULL;
    }
  g_assert (context->cur_attr == -1);
  g_assert (context->attr_names == NULL ||
            context->attr_names[0] == NULL);
  g_assert (context->attr_values == NULL ||
            context->attr_values[0] == NULL);
}