    Status CmdAuthenticate::_authenticate(const std::string& mechanism,
                                          const UserName& user,
                                          const BSONObj& cmdObj) {

        if (mechanism == "MONGODB-CR") {
            return _authenticateCR(user, cmdObj);
        }
#ifdef MONGO_SSL
        if (mechanism == "MONGODB-X509") {
            return _authenticateX509(user, cmdObj);
        }
#endif
        return Status(ErrorCodes::BadValue, "Unsupported mechanism: " + mechanism);
    }