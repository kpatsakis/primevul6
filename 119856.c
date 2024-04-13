static void lo_mknod(fuse_req_t req, fuse_ino_t parent, const char *name,
                     mode_t mode, dev_t rdev)
{
    lo_mknod_symlink(req, parent, name, mode, rdev, NULL);
}