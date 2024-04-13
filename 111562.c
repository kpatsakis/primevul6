static inline PageDesc *page_find(struct uc_struct *uc, tb_page_addr_t index)
{
    return page_find_alloc(uc, index, 0);
}