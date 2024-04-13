static struct node *get_node(struct fuse *f, fuse_ino_t nodeid)
{
    struct node *node = get_node_nocheck(f, nodeid);
    if (!node) {
        fprintf(stderr, "fuse internal error: node %llu not found\n",
                (unsigned long long) nodeid);
        abort();
    }
    return node;
}