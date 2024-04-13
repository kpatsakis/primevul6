bool AES_GCM_CipherContext::InitCipher( const void *pKey, size_t cbKey, size_t cbIV, size_t cbTag, bool bEncrypt )
{
    // Libsodium requires AES and CLMUL instructions for AES-GCM, available in
    // Intel "Westmere" and up. 90.41% of Steam users have this as of the
    // November 2019 survey.
    // Libsodium recommends ChaCha20-Poly1305 in software if you've not got AES support
    // in hardware.
    AssertMsg( crypto_aead_aes256gcm_is_available() == 1, "No hardware AES support on this CPU." );
    AssertMsg( cbKey == crypto_aead_aes256gcm_KEYBYTES, "AES key sizes other than 256 are unsupported." );
    AssertMsg( cbIV == crypto_aead_aes256gcm_NPUBBYTES, "Nonce size is unsupported" );

    if(m_ctx == nullptr)
    {
        m_ctx = sodium_malloc( sizeof(crypto_aead_aes256gcm_state) );
    }

    crypto_aead_aes256gcm_beforenm( static_cast<crypto_aead_aes256gcm_state*>( m_ctx ), static_cast<const unsigned char*>( pKey ) );

    return true;
}