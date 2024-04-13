static struct fuse *req_fuse(fuse_req_t req)
{
    return (struct fuse *) fuse_req_userdata(req);
}