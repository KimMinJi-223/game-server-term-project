﻿// Client_GameServerTermProject.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "pch.h"
#include "framework.h"
#include "Client_GameServerTermProject.h"
#include "Game.h"
#include "NetworkManager.h"
#include <iostream>
#include <locale>
#include <codecvt>

#define MAX_LOADSTRING 100

// 전역 변수:
bool idInput = false;

Game game;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
HWND g_hWnd;
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	std::wcout.imbue(std::locale("korean"));


	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CLIENTGAMESERVERTERMPROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg = {};
	uint64 prevTick = 0;
	

	// 기본 메시지 루프입니다:
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			if (GET_SINGLE(NetworkManager)->GetInstance()->connectOk) {
				GET_SINGLE(NetworkManager)->Update();

				if (GET_SINGLE(NetworkManager)->loginOk == true) {
					game.Update();
					game.Render();
				}
			}
		}
	}

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENTGAMESERVERTERMPROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENTGAMESERVERTERMPROJECT);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT windowRect = { 0, 0, GWinSizeX, GWinSizeY };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);

	g_hWnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	wchar_t word[CHAT_SIZE / 2];
	char multiText[CHAT_SIZE];

	switch (message)
	{
	case WM_CREATE: {
		HWND listBox = CreateWindow(TEXT("LISTBOX"), TEXT("LISTBOX"),
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL, 0, 400, 300, 180, hWnd, (HMENU)1000,
			hInst, NULL);
		HWND edit = CreateWindow(TEXT("EDIT"), TEXT(""),
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 580, 200, 20, hWnd, (HMENU)2000,
			hInst, NULL);
		HWND button = CreateWindow(TEXT("BUTTON"), TEXT("BUTTON"),
			WS_CHILD | WS_VISIBLE | WS_BORDER, 200, 580, 100, 20, hWnd, (HMENU)3000,
			hInst, NULL);

		ShowWindow(listBox, SW_HIDE);
		ShowWindow(edit, SW_HIDE);
		ShowWindow(button, SW_HIDE);

		CreateWindow(TEXT("EDIT"), TEXT("127.0.0.1"),
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 200, 20, hWnd, (HMENU)4000,
			hInst, NULL);
		CreateWindow(TEXT("BUTTON"), TEXT("<- 서버 IP 입력"),
			WS_CHILD | WS_VISIBLE | WS_BORDER, 200, 0, 110, 20, hWnd, (HMENU)5000,
			hInst, NULL);
		HWND idEdit = CreateWindow(TEXT("EDIT"), TEXT(""),
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 40, 200, 20, hWnd, (HMENU)6000,
			hInst, NULL);
		HWND idButton = CreateWindow(TEXT("BUTTON"), TEXT("<- ID입력"),
			WS_CHILD | WS_VISIBLE | WS_BORDER, 200, 40, 110, 20, hWnd, (HMENU)7000,
			hInst, NULL);

		ShowWindow(idEdit, SW_HIDE);
		ShowWindow(idButton, SW_HIDE);
	}
				  break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case 1000: // 리스트 박스
			break;
		case 2000: // 에디트 박스
			break;
		case 3000: // 버튼
		{
			GetDlgItemText(hWnd, 2000, word, CHAT_SIZE / 2);
			if (word[0] == '\0')
				break;
			WideCharToMultiByte(CP_ACP, 0, word, -1, multiText, sizeof(multiText), NULL, NULL);
			GET_SINGLE(NetworkManager)->GetInstance()->SendChat(multiText);
		}
		break;
		case 4000:
		{

		}
		break;
		case 5000:
		{
			GetDlgItemText(hWnd, 4000, word, CHAT_SIZE / 2);
			if (word[0] == '\0')
				break;


			WideCharToMultiByte(CP_ACP, 0, word, -1, multiText, sizeof(multiText), NULL, NULL);
			game.Init(g_hWnd, hInst, multiText);

			HWND ipEdit = GetDlgItem(hWnd, 4000);
			HWND ipbutton = GetDlgItem(hWnd, 5000);
			DestroyWindow(ipEdit);
			DestroyWindow(ipbutton);

			HWND ideEdit= GetDlgItem(hWnd, 6000);
			HWND idbutton = GetDlgItem(hWnd, 7000);
			ShowWindow(ideEdit, SW_SHOW);
			ShowWindow(idbutton, SW_SHOW);

			//GET_SINGLE(NetworkManager)->GetInstance()->SendChat(multiText);
		}
		break;
		case 7000:
		{
			GetDlgItemText(hWnd, 6000, word, CHAT_SIZE / 2);
			if (word[0] == '\0')
				break;
			WideCharToMultiByte(CP_ACP, 0, word, -1, multiText, sizeof(multiText), NULL, NULL);

			GET_SINGLE(NetworkManager)->GetInstance()->SendLogin(multiText);
		}
		break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//// 정보 대화 상자의 메시지 처리기입니다.
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(lParam);
//    switch (message)
//    {
//    case WM_INITDIALOG:
//        return (INT_PTR)TRUE;
//
//    case WM_COMMAND:
//        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//        {
//            EndDialog(hDlg, LOWORD(wParam));
//            return (INT_PTR)TRUE;
//        }
//        break;
//    }
//    return (INT_PTR)FALSE;
//}
