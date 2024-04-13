    void CmdAuthenticate::disableAuthMechanism(std::string authMechanism) {
        if (authMechanism == "MONGODB-CR") {
            _isCRAuthDisabled = true;
        }
        if (authMechanism == "MONGODB-X509") {
            _isX509AuthDisabled = true;
        }
    }