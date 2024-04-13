static int rename_node(struct fuse *f, fuse_ino_t olddir, const char *oldname,
                        fuse_ino_t newdir, const char *newname, int hide)
{
    struct node *node;
    struct node *newnode;
    int err = 0;

    pthread_mutex_lock(&f->lock);
    node  = lookup_node(f, olddir, oldname);
    newnode  = lookup_node(f, newdir, newname);
    if (node == NULL)
        goto out;

    if (newnode != NULL) {
        if (hide) {
            fprintf(stderr, "fuse: hidden file got created during hiding\n");
            err = -EBUSY;
            goto out;
        }
        unhash_name(f, newnode);
    }

    unhash_name(f, node);
    if (hash_name(f, node, newdir, newname) == -1) {
        err = -ENOMEM;
        goto out;
    }

    if (hide)
        node->is_hidden = 1;

 out:
    pthread_mutex_unlock(&f->lock);
    return err;
}