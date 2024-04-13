static ssize_t lo_add_inode_mapping(fuse_req_t req, struct lo_inode *inode)
{
    struct lo_map_elem *elem;

    elem = lo_map_alloc_elem(&lo_data(req)->ino_map);
    if (!elem) {
        return -1;
    }

    elem->inode = inode;
    return elem - lo_data(req)->ino_map.elems;
}