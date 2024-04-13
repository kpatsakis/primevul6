static inline size_t size_code_gen_buffer(size_t tb_size)
{
    /* Size the buffer.  */
    if (tb_size == 0) {
        tb_size = DEFAULT_CODE_GEN_BUFFER_SIZE;
    }
    if (tb_size < MIN_CODE_GEN_BUFFER_SIZE) {
        tb_size = MIN_CODE_GEN_BUFFER_SIZE;
    }
    if (tb_size > MAX_CODE_GEN_BUFFER_SIZE) {
        tb_size = MAX_CODE_GEN_BUFFER_SIZE;
    }
    return tb_size;
}