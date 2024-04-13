WddxPacket::WddxPacket(const Variant& comment, bool manualPacket, bool sVar) :
                       m_packetString(""), m_packetClosed(false),
                       m_manualPacketCreation(manualPacket) {
  std::string header = "<header/>";

  if (!comment.isNull() && !sVar) {
    std::string scomment = comment.toString().data();
    header = "<header><comment>" + scomment + "</comment></header>";
  }
  m_packetString = "<wddxPacket version='1.0'>" + header + "<data>";

  if (m_manualPacketCreation) {
    m_packetString = m_packetString + "<struct>";
  }
}