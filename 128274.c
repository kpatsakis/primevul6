static int find_max_supported_pagesize(Object *obj, void *opaque)
{
    char *mem_path;
    long *hpsize_min = opaque;

    if (object_dynamic_cast(obj, TYPE_MEMORY_BACKEND)) {
        mem_path = object_property_get_str(obj, "mem-path", NULL);
        if (mem_path) {
            long hpsize = qemu_mempath_getpagesize(mem_path);
            if (hpsize < *hpsize_min) {
                *hpsize_min = hpsize;
            }
        } else {
            *hpsize_min = getpagesize();
        }
    }

    return 0;
}