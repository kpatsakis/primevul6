struct fuse_fs *fuse_fs_new(const struct fuse_operations *op, size_t op_size,
                            void *user_data)
{
    struct fuse_fs *fs;

    if (sizeof(struct fuse_operations) < op_size) {
        fprintf(stderr, "fuse: warning: library too old, some operations may not not work\n");
        op_size = sizeof(struct fuse_operations);
    }

    fs = (struct fuse_fs *) calloc(1, sizeof(struct fuse_fs));
    if (!fs) {
        fprintf(stderr, "fuse: failed to allocate fuse_fs object\n");
        return NULL;
    }

    fs->user_data = user_data;
    if (op)
        memcpy(&fs->op, op, op_size);
    return fs;
}