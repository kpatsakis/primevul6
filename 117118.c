static int hide_node(struct fuse *f, const char *oldpath,
                     fuse_ino_t dir, const char *oldname)
{
    char newname[64];
    char *newpath;
    int err = -EBUSY;

    newpath = hidden_name(f, dir, oldname, newname, sizeof(newname));
    if (newpath) {
        err = fuse_fs_rename(f->fs, oldpath, newpath);
        if (!err)
            err = rename_node(f, dir, oldname, dir, newname, 1);
        free(newpath);
    }
    return err;
}