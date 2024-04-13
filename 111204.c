  void operator()(const CPUDevice& d, typename TTypes<T>::ConstFlat x,
                  typename TTypes<T>::ConstFlat y, T tolerance,
                  typename TTypes<bool>::Flat z) {
    auto diff = x - y;
    z.device(d) = diff.abs() <= tolerance;
  }