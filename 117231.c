int fuse_fs_bmap(struct fuse_fs *fs, const char *path, size_t blocksize,
                 uint64_t *idx)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.bmap)
        return fs->op.bmap(path, blocksize, idx);
    else
        return -ENOSYS;
}