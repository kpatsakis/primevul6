static void fuse_lib_help_modules(void)
{
    struct fuse_module *m;
    fprintf(stderr, "\nModule options:\n");
    pthread_mutex_lock(&fuse_context_lock);
    for (m = fuse_modules; m; m = m->next) {
        struct fuse_fs *fs = NULL;
        struct fuse_fs *newfs;
        struct fuse_args args = FUSE_ARGS_INIT(0, NULL);
        if (fuse_opt_add_arg(&args, "") != -1 &&
            fuse_opt_add_arg(&args, "-h") != -1) {
            fprintf(stderr, "\n[%s]\n", m->name);
            newfs = m->factory(&args, &fs);
            assert(newfs == NULL);
        }
        fuse_opt_free_args(&args);
    }
    pthread_mutex_unlock(&fuse_context_lock);
}