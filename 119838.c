static void lo_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
                       off_t offset, struct fuse_file_info *fi)
{
    lo_do_readdir(req, ino, size, offset, fi, 0);
}