static struct fuse_module *fuse_get_module(const char *module)
{
    struct fuse_module *m;

    pthread_mutex_lock(&fuse_context_lock);
    m = fuse_find_module(module);
    if (!m) {
        int err = fuse_load_so_module(module);
        if (!err)
            m = fuse_find_module(module);
    }
    pthread_mutex_unlock(&fuse_context_lock);
    return m;
}