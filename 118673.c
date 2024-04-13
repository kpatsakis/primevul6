uint32_t readFloatingPoint(Protocol_& prot, TType arg_type, T& value) {
  switch (arg_type) {
    case TType::T_DOUBLE: {
      double dub;
      auto res = prot.readDouble(dub);
      value = static_cast<T>(dub);
      return res;
    }
    case TType::T_FLOAT: {
      float flt;
      auto res = prot.readFloat(flt);
      value = static_cast<T>(flt);
      return res;
    }
    default: {
      throw TProtocolException(
          std::string("Cannot parse floating number of ") +
          std::to_string(arg_type) + " type");
    }
  }
}