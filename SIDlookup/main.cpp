#include <Windows.h>
#include <tchar.h>
#include "resource.h"
#include <vector>
#include <string>

using tstring = std::basic_string<TCHAR>;
constexpr size_t MAX_LOADSTRING = 100;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
tstring GetFromStringTable(UINT resourceId);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    std::vector<TCHAR> szTitle(MAX_LOADSTRING);
    LoadString(hInstance, IDS_APP_TITLE, szTitle.data(), MAX_LOADSTRING);

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szTitle.data();
    wcex.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

    RegisterClassExW(&wcex);

    HWND hWnd = CreateWindow(szTitle.data(), szTitle.data(), WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 550, 200, nullptr, nullptr, hInstance, nullptr);
    if (hWnd == nullptr)
    {
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hEditSidToName = nullptr;
    static HWND hEditNameToSid = nullptr;

    static HWND hBtnSidToName = nullptr;
    static HWND hBtnNameToSid = nullptr;

    switch (message)
    {
    case WM_CREATE:
    {
        hEditSidToName = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            _T("EDIT"),
            _T(""),
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 30, 30, 200, 30,
            hWnd,
            (HMENU)IDC_EDIT_SIDTONAME, ((LPCREATESTRUCT)lParam)->hInstance, nullptr);

        hEditNameToSid = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            _T("EDIT"),
            _T(""),
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP, 300, 30, 200, 30,
            hWnd,
            (HMENU)IDC_EDIT_NAMETOSID, ((LPCREATESTRUCT)lParam)->hInstance, nullptr);

        hBtnSidToName = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            _T("BUTTON"),
            GetFromStringTable(IDS_BTN_SIDTONAME_LABEL).c_str(),
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, 30, 80, 200, 30, hWnd,
            (HMENU)IDC_BTN_SIDTONAME, ((LPCREATESTRUCT)lParam)->hInstance, nullptr);

        hBtnNameToSid = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            _T("BUTTON"),
            GetFromStringTable(IDS_BTN_NAMETOSID).c_str(),
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, 300, 80, 200, 30, hWnd,
            (HMENU)IDC_BTN_NAMETOSID, ((LPCREATESTRUCT)lParam)->hInstance, nullptr);

        SetFocus(hEditSidToName);
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_BTN_SIDTONAME:
        {
            OutputDebugString(_T("SID to ユーザー名\n"));
            break;
        }
        case IDC_BTN_NAMETOSID:
        {
            OutputDebugString(_T("ユーザー名 to SID\n"));
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }//switch (message)
    return 0;
}

tstring GetFromStringTable(UINT resourceId)
{
    constexpr auto size = 256;
    std::vector<TCHAR> szResource(size);
    LoadString(GetModuleHandle(nullptr), resourceId, szResource.data(), szResource.size());
    return tstring(szResource.data());
}