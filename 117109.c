static int lookup_path(struct fuse *f, fuse_ino_t nodeid,
                       const char *name, const char *path,
                       struct fuse_entry_param *e, struct fuse_file_info *fi)
{
    int res;

    memset(e, 0, sizeof(struct fuse_entry_param));
    if (fi)
        res = fuse_fs_fgetattr(f->fs, path, &e->attr, fi);
    else
        res = fuse_fs_getattr(f->fs, path, &e->attr);
    if (res == 0) {
        struct node *node;

        node = find_node(f, nodeid, name);
        if (node == NULL)
            res = -ENOMEM;
        else {
            e->ino = node->nodeid;
            e->generation = node->generation;
            e->entry_timeout = f->conf.entry_timeout;
            e->attr_timeout = f->conf.attr_timeout;
#ifdef __SOLARIS__
            if (f->conf.auto_cache) {
                pthread_mutex_lock(&f->lock);
                update_stat(node, &e->attr);
                pthread_mutex_unlock(&f->lock);
            }
#endif /* __SOLARIS__ */
            set_stat(f, e->ino, &e->attr);
            if (f->conf.debug)
                fprintf(stderr, "   NODEID: %lu\n", (unsigned long) e->ino);
        }
    }
    return res;
}