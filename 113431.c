  void *imRealloc(void *ptr, size_t size
#ifdef IM_MEMORY_CHECK
, int ln
#endif
  ) {
    assertx(m_mallocSize < (size_t)RuntimeOption::ImageMemoryMaxBytes);

#ifdef IM_MEMORY_CHECK
    if (!ptr) return imMalloc(size, ln);
    if (!size) {
      imFree(ptr, ln);
      return nullptr;
    }
#else
    if (!ptr) return imMalloc(size);
    if (!size) {
      imFree(ptr);
      return nullptr;
    }
#endif
    void *sizePtr = (char *)ptr - sizeof(size);
    size_t oldSize = 0;
    if (ptr) memcpy(&oldSize, sizePtr, sizeof(oldSize));
    int diff = size - oldSize;
    void *tmp;

#ifdef IM_MEMORY_CHECK
    void *lnPtr = (char *)sizePtr - sizeof(ln);
    if (m_mallocSize + diff > (size_t)RuntimeOption::ImageMemoryMaxBytes ||
        !(tmp = local_realloc(lnPtr, sizeof(ln) + sizeof(size) + size))) {
      int count = m_alloced.erase(ptr);
      assertx(count == 1); // double free on failure
      local_free(lnPtr);
      return nullptr;
    }
    memcpy(tmp, &ln, sizeof(ln));
    memcpy((char*)tmp + sizeof(ln), &size, sizeof(size));
    m_mallocSize += diff;
    if (tmp != lnPtr) {
      int count = m_alloced.erase(lnPtr);
      assertx(count == 1);
      m_alloced.insert(tmp);
    }
    return ((char *)tmp + sizeof(ln) + sizeof(size));
#else
    if (m_mallocSize + diff > (size_t)RuntimeOption::ImageMemoryMaxBytes ||
        !(tmp = local_realloc(sizePtr, sizeof(size) + size))) {
      local_free(sizePtr);
      return nullptr;
    }
    memcpy(tmp, &size, sizeof(size));
    m_mallocSize += diff;
    return ((char *)tmp + sizeof(size));
#endif
  }