static inline void tb_reset_jump(TranslationBlock *tb, int n)
{
    uintptr_t addr = (uintptr_t)((char *)tb->tc.ptr + tb->jmp_reset_offset[n]);
    tb_set_jmp_target(tb, n, addr);
}