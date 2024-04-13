  void *imCalloc(size_t nmemb, size_t size
#ifdef IM_MEMORY_CHECK
, int ln
#endif
  ) {
    assertx(m_mallocSize < (size_t)RuntimeOption::ImageMemoryMaxBytes);
    size_t bytes = nmemb * size;
    if (m_mallocSize + bytes < (size_t)RuntimeOption::ImageMemoryMaxBytes) {
#ifdef IM_MEMORY_CHECK
      void *ptr = local_malloc(sizeof(ln) + sizeof(size) + bytes);
      if (!ptr) return nullptr;
      memset(ptr, 0, sizeof(ln) + sizeof(size) + bytes);
      memcpy(ptr, &ln, sizeof(ln));
      memcpy((char*)ptr + sizeof(ln), &bytes, sizeof(bytes));
      m_mallocSize += bytes;
      m_alloced.insert(ptr);
      return ((char *)ptr + sizeof(ln) + sizeof(size));
#else
      void *ptr = local_malloc(sizeof(size) + bytes);
      if (!ptr) return nullptr;
      memcpy(ptr, &bytes, sizeof(bytes));
      memset((char *)ptr + sizeof(size), 0, bytes);
      m_mallocSize += bytes;
      return ((char *)ptr + sizeof(size));
#endif
    }
    return nullptr;
  }