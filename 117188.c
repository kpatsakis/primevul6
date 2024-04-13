static int hash_name(struct fuse *f, struct node *node, fuse_ino_t parentid,
                     const char *name)
{
    size_t hash = name_hash(f, parentid, name);
    struct node *parent = get_node(f, parentid);
    node->name = strdup(name);
    if (node->name == NULL)
        return -1;

    parent->refctr ++;
    node->parent = parent;
    node->name_next = f->name_table[hash];
    f->name_table[hash] = node;
    return 0;
}