static void remove_node(struct fuse *f, fuse_ino_t dir, const char *name)
{
    struct node *node;

    pthread_mutex_lock(&f->lock);
    node = lookup_node(f, dir, name);
    if (node != NULL)
        unhash_name(f, node);
    pthread_mutex_unlock(&f->lock);
}