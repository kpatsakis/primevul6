static void lo_symlink(fuse_req_t req, const char *link, fuse_ino_t parent,
                       const char *name)
{
    lo_mknod_symlink(req, parent, name, S_IFLNK, 0, link);
}