static bool checkreturn allocate_field(pb_istream_t *stream, void *pData, size_t data_size, size_t array_size)
{    
    void *ptr = *(void**)pData;
    
    if (data_size == 0 || array_size == 0)
        PB_RETURN_ERROR(stream, "invalid size");
    
#ifdef __AVR__
    /* Workaround for AVR libc bug 53284: http://savannah.nongnu.org/bugs/?53284
     * Realloc to size of 1 byte can cause corruption of the malloc structures.
     */
    if (data_size == 1 && array_size == 1)
    {
        data_size = 2;
    }
#endif

    /* Check for multiplication overflows.
     * This code avoids the costly division if the sizes are small enough.
     * Multiplication is safe as long as only half of bits are set
     * in either multiplicand.
     */
    {
        const size_t check_limit = (size_t)1 << (sizeof(size_t) * 4);
        if (data_size >= check_limit || array_size >= check_limit)
        {
            const size_t size_max = (size_t)-1;
            if (size_max / array_size < data_size)
            {
                PB_RETURN_ERROR(stream, "size too large");
            }
        }
    }
    
    /* Allocate new or expand previous allocation */
    /* Note: on failure the old pointer will remain in the structure,
     * the message must be freed by caller also on error return. */
    ptr = pb_realloc(ptr, array_size * data_size);
    if (ptr == NULL)
        PB_RETURN_ERROR(stream, "realloc failed");
    
    *(void**)pData = ptr;
    return true;
}