#pragma once

#include <drogon/HttpController.h>
#include "Users.h"

using namespace drogon;

namespace Controller
{
    class UserController : public drogon::HttpController<UserController>
    {
    public:
        METHOD_LIST_BEGIN
            ADD_METHOD_TO(UserController::login, "user/login", Post);
            ADD_METHOD_TO(UserController::reg, "user/reg", Post);
            ADD_METHOD_TO(UserController::changePassword, "user/changePwd", Post);
        METHOD_LIST_END

        void login(const HttpRequestPtr &req,
                   std::function<void (const HttpResponsePtr &)> &&callback);

        void reg(const HttpRequestPtr &req,
                 std::function<void (const HttpResponsePtr &)> &&callback);

        void changePassword(const HttpRequestPtr &req,
                            std::function<void (const HttpResponsePtr &)> &&callback);

    };
}
