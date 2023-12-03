# MdServer

## 介绍
markdown编辑器的服务端项目，基于drogon框架开发

## 接口文档
> **/user/login** (POST)

请求格式：
```
{"username" : "aaa", "password" : "assa"}
```

响应格式：
```
{"code" : 1, "msg" : "login succcess"}
{"code" : -1, "msg" : "login failed: ..."}
```

> **/user/reg** (POST)

请求格式：
```
{"username" : "aaa", "password" : "assa"}
```

响应格式：
```
{"code" : 1, "msg" : "register succcess"}
{"code" : -1, "msg" : "register failed: ..."}
```


> **/user/reg** (POST)

请求格式：
```
{"username" : "aaa", "password" : "assa"}
```

响应格式：
```
{"code" : 1, "msg" : "register succcess"}
{"code" : -1, "msg" : "register failed: ..."}
```


> **/user/changePwd** (POST)

请求格式：
```
{"username" : "aaa", "oldPassword" : "assa", "newPassword" : "as"}
```

响应格式：
```
{"code" : 1, "msg" : "update succcess"}
{"code" : -1, "msg" : "update failed: ..."}
```

