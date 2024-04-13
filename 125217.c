void SymmetricCryptContextBase::Wipe()
{
    sodium_free(m_ctx);

    m_ctx = nullptr;
    m_cbIV = 0;
    m_cbTag = 0;
}