auto createZeroedBuffer(size_t size) {
  auto ret = IOBuf::create(size);
  ret->append(size);
  std::memset(ret->writableData(), 0x00, size);
  return ret;
}