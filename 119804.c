static struct lo_data *lo_data(fuse_req_t req)
{
    return (struct lo_data *)fuse_req_userdata(req);
}