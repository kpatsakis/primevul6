void CCrypto::Init()
{
    // sodium_init is safe to call multiple times from multiple threads
    // so no need to do anything clever here.
    if(sodium_init() < 0)
    {
        AssertMsg( false, "libsodium didn't init" );
    }
}