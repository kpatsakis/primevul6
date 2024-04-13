static void lo_read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
    struct fuse_bufvec buf = FUSE_BUFVEC_INIT(size);

    fuse_log(FUSE_LOG_DEBUG,
             "lo_read(ino=%" PRIu64 ", size=%zd, "
             "off=%lu)\n",
             ino, size, (unsigned long)offset);

    buf.buf[0].flags = FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK;
    buf.buf[0].fd = lo_fi_fd(req, fi);
    buf.buf[0].pos = offset;

    fuse_reply_data(req, &buf);
}