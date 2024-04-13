struct fuse_context *fuse_get_context(void)
{
    return &fuse_get_context_internal()->ctx;
}