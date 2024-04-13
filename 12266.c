static void rose_remove_node(struct rose_node *rose_node)
{
	struct rose_node *s;

	if ((s = rose_node_list) == rose_node) {
		rose_node_list = rose_node->next;
		kfree(rose_node);
		return;
	}

	while (s != NULL && s->next != NULL) {
		if (s->next == rose_node) {
			s->next = rose_node->next;
			kfree(rose_node);
			return;
		}

		s = s->next;
	}
}