MagickExport void *RelinquishAlignedMemory(void *memory)
{
  if (memory == (void *) NULL)
    return((void *) NULL);
#if defined(MAGICKCORE_HAVE_POSIX_MEMALIGN)
  free(memory);
#elif defined(MAGICKCORE_HAVE__ALIGNED_MALLOC)
  _aligned_free(memory);
#else
  free(*((void **) memory-1));
#endif
  return(NULL);
}