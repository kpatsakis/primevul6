static void lo_forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup)
{
    lo_forget_one(req, ino, nlookup);
    fuse_reply_none(req);
}