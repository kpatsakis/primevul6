std::string WddxPacket::wrapValue(const std::string& start,
                                  const std::string& end,
                                  const std::string& varValue,
                                  const String& varName,
                                  bool hasVarTag) {
  std::string startVar = "";
  std::string endVar = "";
  if (hasVarTag) {
    startVar += "<var name='";
    startVar += varName.data();
    startVar += "'>";
    endVar = "</var>";
  }
  return startVar + start + varValue + end + endVar;
}