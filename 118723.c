uint32_t readIntegral(Protocol_& prot, TType arg_type, T& value) {
  switch (arg_type) {
    case TType::T_BOOL: {
      bool boolv;
      auto res = prot.readBool(boolv);
      value = static_cast<T>(boolv);
      return res;
    }
    case TType::T_BYTE: {
      int8_t bytev;
      auto res = prot.readByte(bytev);
      value = static_cast<T>(bytev);
      return res;
    }
    case TType::T_I16: {
      int16_t i16;
      auto res = prot.readI16(i16);
      value = static_cast<T>(i16);
      return res;
    }
    case TType::T_I32: {
      int32_t i32;
      auto res = prot.readI32(i32);
      value = static_cast<T>(i32);
      return res;
    }
    case TType::T_I64: {
      int64_t i64;
      auto res = prot.readI64(i64);
      value = static_cast<T>(i64);
      return res;
    }
    default: {
      throw TProtocolException(
          std::string("Cannot parse integral number of ") +
          std::to_string(arg_type) + " type");
    }
  }
}