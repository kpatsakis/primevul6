void Random::secureRandom(void* data, size_t size) {
  using Single = SingletonThreadLocal<BufferedRandomDevice, RandomTag>;
  Single::get().get(data, size);
}