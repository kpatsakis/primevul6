static void free_node(struct node *node)
{
    free(node->name);
    free(node);
}