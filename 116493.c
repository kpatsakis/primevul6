CConfigEntry::CConfigEntry(const CConfigEntry& other) : m_pSubConfig(nullptr) {
    if (other.m_pSubConfig) m_pSubConfig = new CConfig(*other.m_pSubConfig);
}