        bool run(const string&, BSONObj& cmdObj, int, string& errmsg, BSONObjBuilder& result, bool fromRepl) {
            nonce64 n = getNextNonce();
            stringstream ss;
            ss << hex << n;
            result.append("nonce", ss.str() );
            ClientBasic::getCurrent()->resetAuthenticationSession(
                    new MongoAuthenticationSession(n));
            return true;
        }