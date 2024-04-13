struct fuse_session *fuse_get_session(struct fuse *f)
{
    return f->se;
}