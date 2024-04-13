  template <typename T> Definition &operator,(T fn) {
    operator=(fn);
    return *this;
  }