#include <Windows.h>
#include <tchar.h>
#include <sddl.h>
#include "resource.h"
#include <vector>
#include <string>
#include <exception>
#include <memory>

using tstring = std::basic_string<TCHAR>;
constexpr size_t MAX_LOADSTRING = 100;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
tstring GetFromStringTable(UINT resourceId);
tstring NameToSid(const tstring& sName);

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
            OutputDebugString(_T("SID to ÉÜÅ[ÉUÅ[ñº\n"));
            break;
        }
        case IDC_BTN_NAMETOSID:
        {
            tstring sSid;
            try
            {
                constexpr size_t BUFFER_LENGTH = 256;
                std::vector<TCHAR> szInputName(BUFFER_LENGTH);
                GetWindowText(hEditNameToSid, szInputName.data(), BUFFER_LENGTH);
                sSid = NameToSid(szInputName.data());
            }
            catch (...)
            {
                //not found
                return 0;
            }
            SetWindowText(hEditSidToName, sSid.c_str());
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

tstring NameToSid(const tstring& sName)
{
    constexpr size_t DOMAIN_BUFFER_SIZE = 256;
    std::vector<TCHAR> szDomainName(DOMAIN_BUFFER_SIZE);
    DWORD dwSizeDomain = szDomainName.size();
    DWORD dwSizeSid = 0;
    SID_NAME_USE sidName;

    BOOL bRet;
    LookupAccountName(nullptr, sName.c_str(), nullptr, &dwSizeSid, szDomainName.data(), &dwSizeDomain, &sidName);

    std::shared_ptr<std::remove_pointer<PSID>::type> pSid((PSID)LocalAlloc(LPTR, dwSizeSid), LocalFree);
    bRet = LookupAccountName(nullptr, sName.c_str(), pSid.get(), &dwSizeSid, szDomainName.data(), &dwSizeDomain, &sidName);
    if (!bRet)
    {
        std::string sErrorMsg = std::string("LookupAccountName failed with error: " + std::to_string(GetLastError()));
        throw std::exception(sErrorMsg.c_str());
    }

    LPTSTR szSid;
    bRet = ConvertSidToStringSid(pSid.get(), &szSid);
    if (!bRet)
    {
        std::string sErrorMsg = std::string("ConvertSidToStringSid failed with error: " + std::to_string(GetLastError()));
        throw std::exception(sErrorMsg.c_str());
    }
    
    tstring sSid(szSid);
    LocalFree(szSid);
    return sSid;
}