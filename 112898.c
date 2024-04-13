std::string WddxPacket::packet_end() {
  if (!m_packetClosed) {
    if (m_manualPacketCreation) {
      m_packetString += "</struct>";
    }
    m_packetString += "</data></wddxPacket>";
  }
  m_packetClosed = true;
  return m_packetString;
}