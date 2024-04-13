do_assert_page_locked(const PageDesc *pd, const char *file, int line)
{
    if (unlikely(!page_is_locked(pd))) {
        // error_report("assert_page_lock: PageDesc %p not locked @ %s:%d",
        //              pd, file, line);
        abort();    // unreachable in unicorn.
    }
}