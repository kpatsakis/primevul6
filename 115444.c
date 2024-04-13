  using std::vector<any>::operator[];
  using std::vector<any>::at;
  using std::vector<any>::resize;
  using std::vector<any>::front;
  using std::vector<any>::back;
  using std::vector<any>::push_back;
  using std::vector<any>::pop_back;
  using std::vector<any>::insert;
  using std::vector<any>::erase;
  using std::vector<any>::clear;
  using std::vector<any>::swap;
  using std::vector<any>::emplace;
  using std::vector<any>::emplace_back;

private:
  friend class Context;
  friend class Sequence;
  friend class PrioritizedChoice;
  friend class Holder;
  friend class PrecedenceClimbing;

  const char *s_ = nullptr;
  size_t n_ = 0;
  size_t choice_count_ = 0;
  size_t choice_ = 0;
  std::string name_;
};

/*
 * Semantic action
 */
template <typename R, typename F,
          typename std::enable_if<std::is_void<R>::value,
                                  std::nullptr_t>::type = nullptr,
          typename... Args>
any call(F fn, Args &&... args) {
  fn(std::forward<Args>(args)...);
  return any();
}