static int ReadNumber(RLByteStream& strm, int maxdigits = 0)
{
    int code;
    int64 val = 0;
    int digits = 0;

    code = strm.getByte();

    while (!isdigit(code))
    {
        if (code == '#' )
        {
            do
            {
                code = strm.getByte();
            }
            while (code != '\n' && code != '\r');
            code = strm.getByte();
        }
        else if (isspace(code))
        {
            while (isspace(code))
                code = strm.getByte();
        }
        else
        {
#if 1
            CV_ErrorNoReturn_(Error::StsError, ("PXM: Unexpected code in ReadNumber(): 0x%x (%d)", code, code));
#else
            code = strm.getByte();
#endif
        }
    }

    do
    {
        val = val*10 + (code - '0');
        CV_Assert(val <= INT_MAX && "PXM: ReadNumber(): result is too large");
        digits++;
        if (maxdigits != 0 && digits >= maxdigits) break;
        code = strm.getByte();
    }
    while (isdigit(code));

    return (int)val;
}