static int getnpasses(Jpeg2000DecoderContext *s)
{
    int num;
    if (!get_bits(s, 1))
        return 1;
    if (!get_bits(s, 1))
        return 2;
    if ((num = get_bits(s, 2)) != 3)
        return num < 0 ? num : 3 + num;
    if ((num = get_bits(s, 5)) != 31)
        return num < 0 ? num : 6 + num;
    num = get_bits(s, 7);
    return num < 0 ? num : 37 + num;
}