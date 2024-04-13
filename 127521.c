static int getlblockinc(Jpeg2000DecoderContext *s)
{
    int res = 0, ret;
    while (ret = get_bits(s, 1)) {
        if (ret < 0)
            return ret;
        res++;
    }
    return res;
}