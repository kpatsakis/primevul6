static struct node *find_node(struct fuse *f, fuse_ino_t parent,
                              const char *name)
{
    struct node *node;

    pthread_mutex_lock(&f->lock);
    node = lookup_node(f, parent, name);
    if (node == NULL) {
        node = (struct node *) calloc(1, sizeof(struct node));
        if (node == NULL)
            goto out_err;

        node->refctr = 1;
        node->nodeid = next_id(f);
        node->open_count = 0;
        node->is_hidden = 0;
        node->generation = f->generation;
        if (hash_name(f, node, parent, name) == -1) {
            free(node);
            node = NULL;
            goto out_err;
        }
        hash_id(f, node);
    }
    node->nlookup ++;
 out_err:
    pthread_mutex_unlock(&f->lock);
    return node;
}