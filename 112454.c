  explicit ArraySortTmp(Array& arr, SortFunction sf) : m_arr(arr) {
    m_ad = arr.get()->escalateForSort(sf);
    assert(m_ad == arr.get() || m_ad->hasExactlyOneRef());
  }