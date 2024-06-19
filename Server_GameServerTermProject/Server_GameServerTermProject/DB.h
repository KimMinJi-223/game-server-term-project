#pragma once

#include "Defines.h"
#include <windows.h>
#include <stdio.h>  
#include <locale.h>
#include <sqlext.h>  
#include <concurrent_queue.h>
#include "Session.h"

#define NAME_LEN 40

struct DB_EVENT {
	Session*	player;
	wchar_t query[100]; // 명령문
	EVENT_TYPE event_type; // 스위치 문에서호출할 함수 정하기
};

class DB
{
private:
	HANDLE _hiocp;

	SQLHSTMT _hstmt;
	SQLHENV _henv;
	SQLHDBC _hdbc;
	
	concurrency::concurrent_queue<DB_EVENT> _dbTaskQueue;

private:
	void printErr(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

public:
	void Init(HANDLE hiocp);

private:
	void connect();
	
private:
	void ExecDirect(DB_EXEC_TYPE exec, const wchar_t* sqlArr, Session* player);
	bool fetchUserInfo(const wchar_t* sqlArr, Session* player);

public:
	void startDbThread();
	void addTaskExecDirect(Session* player, const wchar_t* sql, EVENT_TYPE et);
};

