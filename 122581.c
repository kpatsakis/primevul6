  Handle<String> ToString() const {
    if (str_)
      return String::New(str_, size_);
    else
      return String::Empty();
  }