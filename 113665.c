static char **build_envp(const std::vector<std::string> &env) {
  char **envp = nullptr;
  int size = env.size();
  if (size) {
    envp = (char **)malloc((size + 1) * sizeof(char *));
    int j = 0;
    for (unsigned int i = 0; i < env.size(); i++, j++) {
      *(envp + j) = (char *)env[i].c_str();
    }
    *(envp + j) = nullptr;
  }
  return envp;
}