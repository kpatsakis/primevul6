  template <class T> void setTextEncoding(T *frame)
  {
    if(useDefaultEncoding)
      frame->setTextEncoding(defaultEncoding);
  }