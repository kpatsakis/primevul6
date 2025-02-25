static int read_quant_tables(RangeCoder *c,
                             int16_t quant_table[MAX_CONTEXT_INPUTS][256])
{
    int i;
    int context_count = 1;

    for (i = 0; i < 5; i++) {
        int ret = read_quant_table(c, quant_table[i], context_count);
        if (ret < 0)
            return ret;
        context_count *= ret;
        if (context_count > 32768U) {
            return AVERROR_INVALIDDATA;
        }
    }
    return (context_count + 1) / 2;
}