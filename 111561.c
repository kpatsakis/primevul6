static inline void tb_jmp_unlink(TranslationBlock *dest)
{
    TranslationBlock *tb;
    int n;

    TB_FOR_EACH_JMP(dest, tb, n) {
        tb_reset_jump(tb, n);
#ifdef _MSC_VER
        atomic_and((long *)&tb->jmp_dest[n], (uintptr_t)NULL | 1);
#else
        atomic_and(&tb->jmp_dest[n], (uintptr_t)NULL | 1);
#endif
        /* No need to clear the list entry; setting the dest ptr is enough */
    }
    dest->jmp_list_head = (uintptr_t)NULL;
}