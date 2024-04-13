resource_tree_create (MonoArray *win32_resources)
{
	ResTreeNode *tree, *res_node, *type_node, *lang_node;
	GSList *l;
	int i;

	tree = g_new0 (ResTreeNode, 1);
	
	for (i = 0; i < mono_array_length (win32_resources); ++i) {
		MonoReflectionWin32Resource *win32_res =
			(MonoReflectionWin32Resource*)mono_array_addr (win32_resources, MonoReflectionWin32Resource, i);

		/* Create node */

		/* FIXME: BUG: this stores managed references in unmanaged memory */
		lang_node = g_new0 (ResTreeNode, 1);
		lang_node->id = win32_res->lang_id;
		lang_node->win32_res = win32_res;

		/* Create type node if neccesary */
		type_node = NULL;
		for (l = tree->children; l; l = l->next)
			if (((ResTreeNode*)(l->data))->id == win32_res->res_type) {
				type_node = (ResTreeNode*)l->data;
				break;
			}

		if (!type_node) {
			type_node = g_new0 (ResTreeNode, 1);
			type_node->id = win32_res->res_type;

			/* 
			 * The resource types have to be sorted otherwise
			 * Windows Explorer can't display the version information.
			 */
			tree->children = g_slist_insert_sorted (tree->children, 
				type_node, resource_tree_compare_by_id);
		}

		/* Create res node if neccesary */
		res_node = NULL;
		for (l = type_node->children; l; l = l->next)
			if (((ResTreeNode*)(l->data))->id == win32_res->res_id) {
				res_node = (ResTreeNode*)l->data;
				break;
			}

		if (!res_node) {
			res_node = g_new0 (ResTreeNode, 1);
			res_node->id = win32_res->res_id;
			type_node->children = g_slist_append (type_node->children, res_node);
		}

		res_node->children = g_slist_append (res_node->children, lang_node);
	}

	return tree;
}