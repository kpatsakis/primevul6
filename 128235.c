static void phys_section_destroy(MemoryRegion *mr)
{
    bool have_sub_page = mr->subpage;

    memory_region_unref(mr);

    if (have_sub_page) {
        subpage_t *subpage = container_of(mr, subpage_t, iomem);
        object_unref(OBJECT(&subpage->iomem));
        g_free(subpage);
    }
}