ThreadLocalPRNG::result_type ThreadLocalPRNG::operator()() {
  struct Wrapper {
    Random::DefaultGenerator object{Random::create()};
  };
  using Single = SingletonThreadLocal<Wrapper, RandomTag>;
  return Single::get().object();
}