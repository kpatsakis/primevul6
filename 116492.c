CConfigEntry::CConfigEntry(const CConfig& Config)
    : m_pSubConfig(new CConfig(Config)) {}