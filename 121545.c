bool aligned_memchr16ff(const unsigned char *local_huff_data) {
#if USE_SCALAR
    return memchr(local_huff_data, 0xff, 16) != NULL;
#else
    __m128i buf = _mm_load_si128((__m128i const*)local_huff_data);
    __m128i ff = _mm_set1_epi8(-1);
    __m128i res = _mm_cmpeq_epi8(buf, ff);
    uint32_t movmask = _mm_movemask_epi8(res);
    bool retval = movmask != 0x0;
    dev_assert (retval == (memchr(local_huff_data, 0xff, 16) != NULL));
    return retval;
#endif
}