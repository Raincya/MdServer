#pragma once

#include <drogon/HttpController.h>
#include "Notes.h"
#include "Users.h"

using namespace drogon;

class NoteController : public drogon::HttpController<NoteController>
{
  public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(NoteController::createNote, "/note/create", Post);
        ADD_METHOD_TO(NoteController::updateNote, "/note/update", Post);
        ADD_METHOD_TO(NoteController::queryNoteList, "note/list", Post);
        ADD_METHOD_TO(NoteController::queryNote, "/note/query/{1}", Get);
        ADD_METHOD_TO(NoteController::deleteNote, "/note/delete", Post);
    METHOD_LIST_END

    void createNote(const HttpRequestPtr &req,
                     std::function<void (const HttpResponsePtr &)> &&callback);

    void updateNote(const HttpRequestPtr &req,
                    std::function<void (const HttpResponsePtr &)> &&callback);

    void queryNoteList(const HttpRequestPtr &req,
                   std::function<void (const HttpResponsePtr &)> &&callback);

    void queryNote(const HttpRequestPtr &req,
                 std::function<void (const HttpResponsePtr &)> &&callback,
                 int noteId);

    void deleteNote(const HttpRequestPtr &req,
                    std::function<void (const HttpResponsePtr &)> &&callback);
};
