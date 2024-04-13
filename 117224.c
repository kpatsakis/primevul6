static void unhash_name(struct fuse *f, struct node *node)
{
    if (node->name) {
        size_t hash = name_hash(f, node->parent->nodeid, node->name);
        struct node **nodep = &f->name_table[hash];

        for (; *nodep != NULL; nodep = &(*nodep)->name_next)
            if (*nodep == node) {
                *nodep = node->name_next;
                node->name_next = NULL;
                unref_node(f, node->parent);
                free(node->name);
                node->name = NULL;
                node->parent = NULL;
                return;
            }
        fprintf(stderr, "fuse internal error: unable to unhash node: %llu\n",
                (unsigned long long) node->nodeid);
        abort();
    }
}