inline std::shared_ptr<Ope> dic(const std::vector<std::string> &v) {
  return std::make_shared<Dictionary>(v);
}