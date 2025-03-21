#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

#include <iostream>
#include <vector>
using namespace std;

// 提供离线消息表的操作
class OfflineMsgModel
{
public:
    // 存储用户的离线消息
    void insert(int userid, string msg);

    // 删除用户的离线消息
    void remve(int userid);

    // 查询用户的离线消息
    vector<string> query(int userid);
};

#endif