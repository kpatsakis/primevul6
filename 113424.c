  void *imMalloc(size_t size
#ifdef IM_MEMORY_CHECK
, int ln
#endif
  ) {
    assertx(m_mallocSize < (size_t)RuntimeOption::ImageMemoryMaxBytes);
    if (m_mallocSize + size < (size_t)RuntimeOption::ImageMemoryMaxBytes) {
#ifdef IM_MEMORY_CHECK
      void *ptr = local_malloc(sizeof(ln) + sizeof(size) + size);
      if (!ptr) return nullptr;
      memcpy(ptr, &ln, sizeof(ln));
      memcpy((char*)ptr + sizeof(ln), &size, sizeof(size));
      m_mallocSize += size;
      m_alloced.insert(ptr);
      return ((char *)ptr + sizeof(ln) + sizeof(size));
#else
      void *ptr = local_malloc(sizeof(size) + size);
      if (!ptr) return nullptr;
      memcpy(ptr, &size, sizeof(size));
      m_mallocSize += size;
      return ((char *)ptr + sizeof(size));
#endif
    }
    return nullptr;
  }