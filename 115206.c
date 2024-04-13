int exif_inf(png_structp png_ptr, unsigned char *source,
    unsigned char **dest, size_t n, png_uint_32 inflated_size)
{
    /* *source: compressed data stream (input)
       *dest:   inflated data (output)
       n: length of input

       Returns one of the following:
       return(-1);  chunk had an error
       return(n);  success, n is length of inflated data
     */

    int ret;
    z_stream strm;

    size_t inflated_length = inflated_size;

    if (inflated_length >= PNG_USER_CHUNK_MALLOC_MAX - 1U ||
        inflated_length == 0)
        return (-1);

    /* allocate dest */
#if PNG_LIBPNG_VER >= 14000
    *dest=(unsigned char *) png_malloc(png_ptr,
       (png_alloc_size_t) inflated_length);
#else
    *dest=(unsigned char *) png_malloc(png_ptr,
       (png_size_t) inflated_length);
#endif
    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return (-1);
    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = (int)n;
        strm.next_in = source;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = (int)inflated_length;
            strm.next_out = *dest;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return (-1);
            }
        } while (strm.avail_out == 0);
        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */

    /* To do: take care of too little or too much data */

    (void)inflateEnd(&strm);
    return (inflated_length);
}