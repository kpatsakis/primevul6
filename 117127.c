int fuse_loop(struct fuse *f)
{
    if (f)
        return fuse_session_loop(f->se);
    else
        return -1;
}