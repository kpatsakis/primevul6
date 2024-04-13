resource_tree_compare_by_id (gconstpointer a, gconstpointer b)
{
	ResTreeNode *t1 = (ResTreeNode*)a;
	ResTreeNode *t2 = (ResTreeNode*)b;

	return t1->id - t2->id;
}