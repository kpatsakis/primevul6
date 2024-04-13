add_attribute (GMarkupParseContext *context, GString *str)
{
  if (context->cur_attr + 2 >= context->alloc_attrs)
    {
      context->alloc_attrs += 5; /* silly magic number */
      context->attr_names = g_realloc (context->attr_names, sizeof(GString*)*context->alloc_attrs);
      context->attr_values = g_realloc (context->attr_values, sizeof(GString*)*context->alloc_attrs);
    }
  context->cur_attr++;
  context->attr_names[context->cur_attr] = str;
  context->attr_values[context->cur_attr] = NULL;
  context->attr_names[context->cur_attr+1] = NULL;
  context->attr_values[context->cur_attr+1] = NULL;
}