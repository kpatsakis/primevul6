resource_tree_free (ResTreeNode * node)
{
	GSList * list;
	for (list = node->children; list; list = list->next)
		resource_tree_free ((ResTreeNode*)list->data);
	g_slist_free(node->children);
	g_free (node);
}