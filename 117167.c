static int fuse_push_module(struct fuse *f, const char *module,
                            struct fuse_args *args)
{
    struct fuse_fs *newfs;
    struct fuse_module *m = fuse_get_module(module);
    struct fuse_fs *fs[2];

    fs[0] = f->fs;
    fs[1] = NULL;
    if (!m)
        return -1;

    newfs = m->factory(args, fs);
    if (!newfs) {
        fuse_put_module(m);
        return -1;
    }
    newfs->m = m;
    f->fs = newfs;
    return 0;
}