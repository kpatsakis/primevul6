static inline void reply_err(fuse_req_t req, int err)
#endif /* __SOLARIS__ */
{
    /* fuse_reply_err() uses non-negated errno values */
    fuse_reply_err(req, -err);
}