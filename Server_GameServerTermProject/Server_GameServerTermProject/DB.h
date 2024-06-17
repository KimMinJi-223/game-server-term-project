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
	wchar_t sqlArr[100]; // 명령문
	EVENT_TYPE event_type; // 스위치 문에서호출할 함수 정하기
};

class DB
{
private:
	HANDLE _hiocp;

	SQLHSTMT hstmt;
	SQLHENV henv;
	SQLHDBC hdbc;
	
	concurrency::concurrent_queue<DB_EVENT> _db_queue;

	void print_err(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

public:
	void Init(HANDLE hiocp);
	void do_db();
	void add_exec(Session* player, const wchar_t* sql, EVENT_TYPE et);

public:
	bool ExecDirect(EXEC_TYPE exec, const wchar_t* sqlArr, Session* player);
	void connect();
	bool send_login(const wchar_t* sqlArr, Session* player);
	void send_logout(const wchar_t* sqlArr, Session* player);
	//void send_update();
};

