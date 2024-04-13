CConfigEntry& CConfigEntry::operator=(const CConfigEntry& other) {
    delete m_pSubConfig;
    if (other.m_pSubConfig)
        m_pSubConfig = new CConfig(*other.m_pSubConfig);
    else
        m_pSubConfig = nullptr;
    return *this;
}