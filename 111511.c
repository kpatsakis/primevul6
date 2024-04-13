static bool page_entry_trylock(struct page_entry *pe)
{
    bool busy;

    busy = qemu_spin_trylock(&pe->pd->lock);
    if (!busy) {
        g_assert(!pe->locked);
        pe->locked = true;
        page_lock__debug(pe->pd);
    }
    return busy;
}