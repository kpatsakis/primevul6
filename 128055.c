free_list_node (GMarkupParseContext *context, GSList *node)
{
  node->data = NULL;
  context->spare_list_nodes = g_slist_concat (node, context->spare_list_nodes);
}