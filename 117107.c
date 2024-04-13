static void delete_node(struct fuse *f, struct node *node)
{
    if (f->conf.debug)
        fprintf(stderr, "delete: %llu\n", (unsigned long long) node->nodeid);

    assert(!node->name);
    unhash_id(f, node);
    free_node(node);
}