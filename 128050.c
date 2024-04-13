get_list_node (GMarkupParseContext *context, gpointer data)
{
  GSList *node;
  if (context->spare_list_nodes != NULL)
    {
      node = context->spare_list_nodes;
      context->spare_list_nodes = g_slist_remove_link (context->spare_list_nodes, node);
    }
  else
    node = g_slist_alloc();
  node->data = data;
  return node;
}