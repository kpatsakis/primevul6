  void imFree(void *ptr
#ifdef IM_MEMORY_CHECK
, int ln
#endif
  ) {
    size_t size;
    void *sizePtr = (char *)ptr - sizeof(size);
    memcpy(&size, sizePtr, sizeof(size));
    m_mallocSize -= size;
#ifdef IM_MEMORY_CHECK
    void *lnPtr = (char *)sizePtr - sizeof(ln);
    int count = m_alloced.erase((char*)sizePtr - sizeof(ln));
    assertx(count == 1); // double free on failure
    assertx(m_mallocSize < (size_t)RuntimeOption::ImageMemoryMaxBytes);
    local_free(lnPtr);
#else
    assertx(m_mallocSize < (size_t)RuntimeOption::ImageMemoryMaxBytes);
    local_free(sizePtr);
#endif
  }