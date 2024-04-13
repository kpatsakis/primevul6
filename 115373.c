inline std::shared_ptr<Ope> csc(const std::shared_ptr<Ope> &ope) {
  return std::make_shared<CaptureScope>(ope);
}