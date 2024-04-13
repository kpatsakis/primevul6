pop_tag (GMarkupParseContext *context)
{
  GSList *nodea, *nodeb;

  nodea = context->tag_stack;
  nodeb = context->tag_stack_gstr;
  release_chunk (context, nodeb->data);
  context->tag_stack = g_slist_remove_link (context->tag_stack, nodea);
  context->tag_stack_gstr = g_slist_remove_link (context->tag_stack_gstr, nodeb);
  free_list_node (context, nodea);
  free_list_node (context, nodeb);
}