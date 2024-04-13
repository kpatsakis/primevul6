void fuse_fs_destroy(struct fuse_fs *fs)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.destroy)
        fs->op.destroy(fs->user_data);
#ifdef __SOLARIS__
    if (fs->m)
        fuse_put_module(fs->m);
#endif /* __SOLARIS__ */
    free(fs);
}