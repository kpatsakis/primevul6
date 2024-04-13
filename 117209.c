void fuse_destroy(struct fuse *f)
{
    size_t i;

    if (f->conf.intr && f->intr_installed)
        fuse_restore_intr_signal(f->conf.intr_signal);

    if (f->fs) {
        struct fuse_context_i *c = fuse_get_context_internal();

        memset(c, 0, sizeof(*c));
        c->ctx.fuse = f;

        for (i = 0; i < f->id_table_size; i++) {
            struct node *node;

            for (node = f->id_table[i]; node != NULL; node = node->id_next) {
                if (node->is_hidden) {
                    char *path = get_path(f, node->nodeid);
                    if (path) {
                        fuse_fs_unlink(f->fs, path);
                        free(path);
                    }
                }
            }
        }
    }
    for (i = 0; i < f->id_table_size; i++) {
        struct node *node;
        struct node *next;

        for (node = f->id_table[i]; node != NULL; node = next) {
            next = node->id_next;
            free_node(node);
        }
    }
    free(f->id_table);
    free(f->name_table);
    pthread_mutex_destroy(&f->lock);
    pthread_rwlock_destroy(&f->tree_lock);
    fuse_session_destroy(f->se);
#ifdef __SOLARIS__
    free(f->conf.modules);
#endif /* __SOLARIS__ */
    free(f);
    fuse_delete_context_key();
}