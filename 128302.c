bool cpu_physical_memory_snapshot_get_dirty(DirtyBitmapSnapshot *snap,
                                            ram_addr_t start,
                                            ram_addr_t length)
{
    unsigned long page, end;

    assert(start >= snap->start);
    assert(start + length <= snap->end);

    end = TARGET_PAGE_ALIGN(start + length - snap->start) >> TARGET_PAGE_BITS;
    page = (start - snap->start) >> TARGET_PAGE_BITS;

    while (page < end) {
        if (test_bit(page, snap->dirty)) {
            return true;
        }
        page++;
    }
    return false;
}