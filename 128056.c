push_partial_as_tag (GMarkupParseContext *context)
{
  GString *str = context->partial_chunk;
  /* sadly, this is exported by gmarkup_get_element_stack as-is */
  context->tag_stack = g_slist_concat (get_list_node (context, str->str), context->tag_stack);
  context->tag_stack_gstr = g_slist_concat (get_list_node (context, str), context->tag_stack_gstr);
  context->partial_chunk = NULL;
}