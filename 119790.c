static void lo_mkdir(fuse_req_t req, fuse_ino_t parent, const char *name,
                     mode_t mode)
{
    lo_mknod_symlink(req, parent, name, S_IFDIR | mode, 0, NULL);
}