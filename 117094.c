static void fuse_lib_destroy(void *data)
{
    struct fuse *f = (struct fuse *) data;
    struct fuse_context_i *c = fuse_get_context_internal();

    memset(c, 0, sizeof(*c));
    c->ctx.fuse = f;
    fuse_fs_destroy(f->fs);
    f->fs = NULL;
}