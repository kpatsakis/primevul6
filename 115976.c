  MakeValue(const T &value,
            typename EnableIf<Not<
              ConvertToInt<T>::value>::value, int>::type = 0) {
    custom.value = &value;
    custom.format = &format_custom_arg<T>;
  }