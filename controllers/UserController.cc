#include "UserController.h"

using namespace Controller;

void UserController::login(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<orm::DbClient> clientPtr = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::mdserver::Users> db(clientPtr);

    std::shared_ptr<Json::Value> reqJson = req->getJsonObject();

    std::pair<std::string, std::string> userInfoFromReq = {
            (*reqJson)["username"].asString(),
            (*reqJson)["password"].asString()
    };

    std::vector<drogon_model::mdserver::Users> userInfoFromDb = db.findBy(
            drogon::orm::Criteria(drogon_model::mdserver::Users::Cols::_username,
                                  drogon::orm::CompareOperator::EQ,
                                  userInfoFromReq.first)
                                  );

    Json::Value result;

    if(userInfoFromDb.empty())
    {
        result["code"] = -1;
        result["msg"] = "login failed: user does not exist";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    if(userInfoFromReq.first != userInfoFromDb.front().getValueOfUsername() or userInfoFromReq.second != userInfoFromDb.front().getValueOfPassword())
    {
        result["code"] = -1;
        result["msg"] = "login failed: username and password do not match";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    result["code"] = 1;
    result["msg"] = "login success";
    std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void UserController::reg(const HttpRequestPtr &req,
                         std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<orm::DbClient> clientPtr = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::mdserver::Users> db(clientPtr);

    std::shared_ptr<Json::Value> reqJson = req->getJsonObject();

    std::pair<std::string, std::string> userInfoFromReq = {
            (*reqJson)["username"].asString(),
            (*reqJson)["password"].asString()
    };

    std::vector<drogon_model::mdserver::Users> userInfoFromDb = db.findBy(
            drogon::orm::Criteria(drogon_model::mdserver::Users::Cols::_username,
                                  drogon::orm::CompareOperator::EQ,
                                  userInfoFromReq.first)
    );

    Json::Value result;

    if(!userInfoFromDb.empty())
    {
        result["code"] = -1;
        result["msg"] = "register failed: user already exist";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    drogon_model::mdserver::Users userInfo;

    userInfo.setUsername(userInfoFromReq.first);
    userInfo.setPassword(userInfoFromReq.second);

    try {
        db.insert(userInfo);
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "register failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    result["code"] = 1;
    result["msg"] = "register success";
    std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void UserController::changePassword(const HttpRequestPtr &req,
                                    std::function<void (const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<orm::DbClient> clientPtr = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::mdserver::Users> db(clientPtr);

    std::shared_ptr<Json::Value> reqJson = req->getJsonObject();

    struct { std::string username, oldPassword, newPassword; } userInfoFromReq = {
            (*reqJson)["username"].asString(),
            (*reqJson)["oldPassword"].asString(),
            (*reqJson)["newPassword"].asString()
    };

    std::vector<drogon_model::mdserver::Users> userInfoFromDb = db.findBy(
            drogon::orm::Criteria(drogon_model::mdserver::Users::Cols::_username,
                                  drogon::orm::CompareOperator::EQ,
                                  userInfoFromReq.username)
    );

    Json::Value result;

    if(userInfoFromDb.empty())
    {
        result["code"] = -1;
        result["msg"] = "update failed: user does not exist";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    if(userInfoFromReq.username != userInfoFromDb.front().getValueOfPassword())
    {
        result["code"] = -1;
        result["msg"] = "update failed: the old password is incorrect";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    drogon_model::mdserver::Users userInfo;
    userInfo.setId(userInfoFromDb.front().getValueOfId());
    userInfo.setUsername(userInfoFromReq.username);
    userInfo.setPassword(userInfoFromReq.newPassword);

    try {
        db.update(userInfo);
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "update failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    result["code"] = 1;
    result["msg"] = "update success";
    std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}
