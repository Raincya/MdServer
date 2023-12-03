#include "NoteController.h"

void NoteController::createNote(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<orm::DbClient> clientPtr = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::mdserver::Notes> db(clientPtr);
    drogon::orm::Mapper<drogon_model::mdserver::Users> _db(clientPtr);

    std::shared_ptr<Json::Value> reqJson = req->getJsonObject();

    struct { std::string username, title, content; trantor::Date update_time; } noteInfoFromReq = {
            (*reqJson)["username"].asString(),
            (*reqJson)["title"].asString(),
            (*reqJson)["content"].asString(),
            trantor::Date::date()
    };

    drogon_model::mdserver::Users userInfo;

    Json::Value result;
    
    try {
        userInfo = _db.findOne(
                drogon::orm::Criteria(drogon_model::mdserver::Users::Cols::_username,
                                      drogon::orm::CompareOperator::EQ,
                                      noteInfoFromReq.username)
        );
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "create failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    int userId = userInfo.getValueOfId();

    std::vector<drogon_model::mdserver::Notes> noteList;

    try {
         noteList = db.findBy(
                drogon::orm::Criteria(
                        drogon_model::mdserver::Notes::Cols::_user_id,
                        drogon::orm::CompareOperator::EQ,
                        userId
                )
        );
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "create failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    bool isExist = false;

    std::for_each(noteList.begin(), noteList.end(),
                  [&](const drogon_model::mdserver::Notes& item) -> void
                  {
                      if(item.getValueOfTitle() == noteInfoFromReq.title)
                      {
                          isExist = true;
                      }
                  });

    if(isExist)
    {
        result["code"] = -1;
        result["msg"] = "create failed: content with the same title already exists";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    drogon_model::mdserver::Notes noteInfo;
    noteInfo.setUserId(userId);
    noteInfo.setTitle(noteInfoFromReq.title);
    noteInfo.setContent(noteInfoFromReq.content);
    noteInfo.setUpdateDate(noteInfoFromReq.update_time);

    try {
        db.insert(noteInfo);
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "create failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    result["code"] = 1;
    result["msg"] = "create success";
    std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void NoteController::updateNote(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<orm::DbClient> clientPtr = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::mdserver::Notes> db(clientPtr);
    drogon::orm::Mapper<drogon_model::mdserver::Users> _db(clientPtr);

    std::shared_ptr<Json::Value> reqJson = req->getJsonObject();

    struct { std::string username, title, content; trantor::Date update_time; } noteInfoFromReq = {
            (*reqJson)["username"].asString(),
            (*reqJson)["title"].asString(),
            (*reqJson)["content"].asString(),
            trantor::Date::date()
    };

    Json::Value result;

    drogon_model::mdserver::Users userInfo;

    try {
        userInfo = _db.findOne(
                drogon::orm::Criteria(drogon_model::mdserver::Users::Cols::_username,
                                      drogon::orm::CompareOperator::EQ,
                                      noteInfoFromReq.username)
        );
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "update failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    int userId = userInfo.getValueOfId();

    std::vector<drogon_model::mdserver::Notes> noteList;

    try {
        noteList = db.findBy(
                drogon::orm::Criteria(drogon_model::mdserver::Notes::Cols::_user_id,
                                      drogon::orm::CompareOperator::EQ,
                                      userId)
        );
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "update failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    bool isExist = false;

    drogon_model::mdserver::Notes noteInfoFromDb;

    std::for_each(noteList.begin(), noteList.end(),
                  [&](const drogon_model::mdserver::Notes& item) -> void
                  {
                      if(item.getValueOfTitle() == noteInfoFromReq.title)
                      {
                          isExist = true;
                          noteInfoFromDb = item;
                      }
                  });

    if(!isExist)
    {
        result["code"] = -1;
        result["msg"] = "update failed: content with the same title does not exist";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    drogon_model::mdserver::Notes noteInfo;
    noteInfo.setId(noteInfoFromDb.getValueOfId());
    noteInfo.setTitle(noteInfoFromDb.getValueOfTitle());
    noteInfo.setContent(noteInfoFromReq.content);
    noteInfo.setUserId(userId);
    noteInfo.setUpdateDate(noteInfoFromReq.update_time);

    try {
        db.update(noteInfo);
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

void NoteController::queryNoteList(const HttpRequestPtr &req,
                               std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<orm::DbClient> clientPtr = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::mdserver::Notes> db(clientPtr);
    drogon::orm::Mapper<drogon_model::mdserver::Users> _db(clientPtr);

    std::shared_ptr<Json::Value> reqJson = req->getJsonObject();

    struct { std::string username, password; } userInfoFromReq = {
            (*reqJson)["username"].asString(),
            (*reqJson)["password"].asString()
    };

    Json::Value result;

    drogon_model::mdserver::Users userInfo;

    try {
        userInfo = _db.findOne(
                drogon::orm::Criteria(drogon_model::mdserver::Users::Cols::_username,
                                      drogon::orm::CompareOperator::EQ,
                                      userInfoFromReq.username)
        );
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "query failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    int userId = userInfo.getValueOfId();

    std::vector<drogon_model::mdserver::Notes> noteList;

    try {
        noteList = db.findBy(
                drogon::orm::Criteria(drogon_model::mdserver::Notes::Cols::_user_id,
                                      drogon::orm::CompareOperator::EQ,
                                      userId)
        );
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "query failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    Json::Value note;

    std::for_each(noteList.begin(), noteList.end(),
                  [&](const drogon_model::mdserver::Notes& item) -> void
                  {
                      Json::Value now;
                      now["id"] = static_cast<int>(item.getValueOfId());
                      now["title"] = item.getValueOfTitle();
                      note.append(now);
                  });

    result["code"] = 1;
    result["msg"] = "query success";
    result["data"] = note;
    std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void NoteController::queryNote(const HttpRequestPtr &req,
                               std::function<void(const HttpResponsePtr &)> &&callback,
                               int noteId)
{
    std::shared_ptr<orm::DbClient> clientPtr = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::mdserver::Notes> db(clientPtr);

    drogon_model::mdserver::Notes noteInfo;
    
    Json::Value result;

    try {
        noteInfo = db.findOne(
                drogon::orm::Criteria(drogon_model::mdserver::Notes::Cols::_id,
                                      drogon::orm::CompareOperator::EQ,
                                      noteId)
        );
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "query failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    result["code"] = 1;
    result["msg"] = "query success";
    result["data"]["title"] = noteInfo.getValueOfTitle();
    result["data"]["content"] = noteInfo.getValueOfContent();
    result["data"]["update_time"] = noteInfo.getValueOfUpdateDate().toDbString();
    std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void NoteController::deleteNote(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<orm::DbClient> clientPtr = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::mdserver::Notes> db(clientPtr);
    drogon::orm::Mapper<drogon_model::mdserver::Users> _db(clientPtr);

    std::shared_ptr<Json::Value> reqJson = req->getJsonObject();

    struct { std::string username, title; } noteInfoFromReq = {
            (*reqJson)["username"].asString(),
            (*reqJson)["title"].asString()
    };

    Json::Value result;

    drogon_model::mdserver::Users userInfo;

    try {
        userInfo = _db.findOne(
                drogon::orm::Criteria(drogon_model::mdserver::Users::Cols::_username,
                                      drogon::orm::CompareOperator::EQ,
                                      noteInfoFromReq.username)
        );
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "delete failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    int userId = userInfo.getValueOfId();

    std::vector<drogon_model::mdserver::Notes> noteList;

    try {
        noteList = db.findBy(
                drogon::orm::Criteria(drogon_model::mdserver::Notes::Cols::_user_id,
                                      drogon::orm::CompareOperator::EQ,
                                      userId)
        );
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "delete failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    bool isExist = false;

    drogon_model::mdserver::Notes noteInfoFromDb;

    std::for_each(noteList.begin(), noteList.end(),
                  [&](const drogon_model::mdserver::Notes& item) -> void
                  {
                      if(item.getValueOfTitle() == noteInfoFromReq.title)
                      {
                          isExist = true;
                          noteInfoFromDb = item;
                      }
                  });

    if(!isExist)
    {
        result["code"] = -1;
        result["msg"] = "delete failed: content with the same title does not exist";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    try {
        db.deleteBy(
                drogon::orm::Criteria(drogon_model::mdserver::Notes::Cols::_id,
                                      drogon::orm::CompareOperator::EQ,
                                      noteInfoFromDb.getValueOfId())
        );
    } catch (const drogon::orm::DrogonDbException& e)
    {
        std::cerr << e.base().what() << std::endl;
        result["code"] = -1;
        result["msg"] = "delete failed: unknown reason, please try again";
        std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    result["code"] = 1;
    result["msg"] = "delete success";
    std::shared_ptr<HttpResponse> resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}