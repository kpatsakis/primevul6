static void unref_node(struct fuse *f, struct node *node)
{
    assert(node->refctr > 0);
    node->refctr --;
    if (!node->refctr)
        delete_node(f, node);
}