bool target_words_bigendian(void)
{
#if defined(TARGET_WORDS_BIGENDIAN)
    return true;
#else
    return false;
#endif
}