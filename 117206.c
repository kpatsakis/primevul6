void fuse_exit(struct fuse *f)
{
    fuse_session_exit(f->se);
}