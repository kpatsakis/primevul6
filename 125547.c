        bool run(const string& dbname,
                 BSONObj& cmdObj,
                 int options,
                 string& errmsg,
                 BSONObjBuilder& result,
                 bool fromRepl) {
            AuthorizationSession* authSession =
                    ClientBasic::getCurrent()->getAuthorizationSession();
            authSession->logoutDatabase(dbname);
            return true;
        }