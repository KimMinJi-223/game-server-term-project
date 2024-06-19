#include "OVER_EXP.h"
#include "DB.h"
#include <thread>

void DB::printErr(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

void DB::Init(HANDLE hiocp)
{
	_hiocp = hiocp;
	connect();
}


void DB::connect()
{
	setlocale(LC_ALL, "korean");

	// Allocate environment handle  
	SQLRETURN retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, _henv, &_hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				retcode = SQLConnect(_hdbc, (SQLWCHAR*)L"2019184002_GameServerProj_ODBC", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &_hstmt);
				}
			}
		}
	}
}

void DB::ExecDirect(DB_EXEC_TYPE exec, const wchar_t* sqlArr, Session* player)
{
	bool ret = false;

	SQLRETURN retcode = SQLExecDirect(_hstmt, (SQLWCHAR*)sqlArr, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		switch (exec)
		{
		case DB_EXEC_TYPE::LOGINT: {
			ret = fetchUserInfo(sqlArr, player);
		}
		break;
		case DB_EXEC_TYPE::LOGOUT: {
			printf("로그아웃 : %d", player->GetId());
		}
		break;
		default:
			break;
		}
	}
	else {
		printErr(_hstmt, SQL_HANDLE_STMT, retcode);
	}
	// Process data  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		SQLCancel(_hstmt);
	}
}

bool DB::fetchUserInfo(const wchar_t* sqlArr, Session* player)
{
	SQLRETURN retcode;

	SQLWCHAR szName[NAME_LEN];
	SQLINTEGER x;
	SQLINTEGER y;
	SQLINTEGER lv;
	SQLINTEGER exp;
	SQLINTEGER hp;
	SQLINTEGER power;
	SQLINTEGER success;

	SQLLEN cbName = 0, cbX = 0, cbY = 0, cbLv = 0, cbExp = 0, cbHp = 0, cbPower = 0, cbSuccess = 0;

	retcode = SQLBindCol(_hstmt, 1, SQL_C_WCHAR, szName, NAME_LEN + 2, &cbName);
	retcode = SQLBindCol(_hstmt, 2, SQL_C_LONG, &x, 10, &cbX);
	retcode = SQLBindCol(_hstmt, 3, SQL_C_LONG, &y, 10, &cbY);
	retcode = SQLBindCol(_hstmt, 4, SQL_C_LONG, &lv, 10, &cbLv);
	retcode = SQLBindCol(_hstmt, 5, SQL_C_LONG, &exp, 10, &cbExp);
	retcode = SQLBindCol(_hstmt, 6, SQL_C_LONG, &hp, 10, &cbHp);
	retcode = SQLBindCol(_hstmt, 7, SQL_C_LONG, &power, 10, &cbPower);
	retcode = SQLBindCol(_hstmt, 8, SQL_C_LONG, &success, 10, &cbSuccess);

	retcode = SQLFetch(_hstmt);
	if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
	{
		printErr(_hstmt, SQL_HANDLE_STMT, retcode);
		return false;
	}
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		if (success == true) {
			//printf("유저가 존재합니다.\n");
		}
		else {
			//printf("유저가 없어 유저를 추가했습니다.\n");
		}

		//printf("x : %3d, y : %3d, Lv : %3d, Exp : %3d, Hp : %3d, Power : %3d\n", x, y, lv, exp, hp, power);

		player->Login(x, y, hp, lv, exp, power);
		return true;
	}
	else {
		wprintf(L"User not found.\n");
		return false;
	}
}

void DB::startDbThread()
{
	while (true) {
		DB_EVENT ev;
		if (_dbTaskQueue.try_pop(ev)) {
			OVER_EXP* ov = new OVER_EXP;
			switch (ev.event_type) {
			case EV_LOGIN:
				ov->_comp_type = OP_LOGIN;
				ExecDirect(LOGINT, ev.query, ev.player);
				PostQueuedCompletionStatus(_hiocp, 1, ev.player->GetId(), &ov->_over);
				break;
			case EV_LOGOUT:
				ExecDirect(LOGOUT, ev.query, ev.player);
				break;
			default:
				delete ov;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void DB::addTaskExecDirect(Session* player, const wchar_t* sql, EVENT_TYPE et)
{
	DB_EVENT ev;
	ev.player = player;
	wcsncpy_s(ev.query, sql, 100);
	ev.event_type = et;
	_dbTaskQueue.push(ev);
}