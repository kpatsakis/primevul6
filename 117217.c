void fuse_fs_init(struct fuse_fs *fs, struct fuse_conn_info *conn)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.init)
        fs->user_data = fs->op.init(conn);
}