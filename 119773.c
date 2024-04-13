static ssize_t lo_add_dirp_mapping(fuse_req_t req, struct lo_dirp *dirp)
{
    struct lo_map_elem *elem;

    elem = lo_map_alloc_elem(&lo_data(req)->dirp_map);
    if (!elem) {
        return -1;
    }

    elem->dirp = dirp;
    return elem - lo_data(req)->dirp_map.elems;
}