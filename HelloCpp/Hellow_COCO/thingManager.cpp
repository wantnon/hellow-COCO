

#include "thingManager.h"
//类静态成员初始化
//注意，1， 类静态成员一定要初始化，否则会产生无法解析外部符号的连接错误
// 2，类静态成员初始化一定要放在cpp中，否则会产生符号多重定义的连接错误
CthingManager*CthingManager::m_pInstance=NULL;