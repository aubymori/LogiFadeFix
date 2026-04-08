#include <windows.h>
#include "resource.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define WM_TRAYCB (WM_USER + 0x223)

LRESULT CALLBACK WndProc(
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            NOTIFYICONDATAW nid;
            nid.cbSize           = sizeof(nid);
            nid.hWnd             = hwnd;
            nid.uID              = 0;
            nid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
            nid.uCallbackMessage = WM_TRAYCB;
            nid.hIcon            = (HICON)LoadImageW(
                            HINST_THISCOMPONENT, MAKEINTRESOURCEW(IDI_LOGIFADEFIX), IMAGE_ICON,
                            GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
                            LR_DEFAULTCOLOR);
            lstrcpyW(nid.szTip, L"LogiFadeFix");
            Shell_NotifyIconW(NIM_ADD, &nid);

            PlaySoundW(MAKEINTRESOURCEW(IDR_BLANKWAVE), HINST_THISCOMPONENT, SND_RESOURCE | SND_LOOP | SND_ASYNC);

            return 0;
        }
        case WM_TRAYCB:
            if (LOWORD(lParam) == WM_RBUTTONDOWN)
            {
                static HMENU s_hmenu = NULL;
                if (!s_hmenu)
                {
                    HMENU hmenuParent = LoadMenu(HINST_THISCOMPONENT, IDR_TRAYMENU);
                    if (hmenuParent)
                    {
                        s_hmenu = GetSubMenu(hmenuParent, 0);
                        RemoveMenu(hmenuParent, 0, MF_BYPOSITION);
                        DestroyMenu(hmenuParent);
                    }
                }

                POINT pt;
                GetCursorPos(&pt);
                switch (TrackPopupMenuEx(s_hmenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, hwnd, NULL))
                {
                    case IDM_ABOUT:
                    {
                        MSGBOXPARAMSW mbp;
                        mbp.cbSize             = sizeof(mbp);
                        mbp.hwndOwner          = hwnd;
                        mbp.hInstance          = HINST_THISCOMPONENT;
                        mbp.lpszText           = L"LogiFadeFix, version " VER_STRING "\n"
                                                  "A daemon that fixes sounds fading in on the Logitech G733\n"
                                                  "\n"
                                                  "Licensed under GPL-3.0.\n"
                                                  "GitHub repo: https://github.com/aubymori/LogiFadeFix";
                        mbp.lpszCaption        = L"About LogiFadeFix";
                        mbp.dwStyle            = MB_USERICON;
                        mbp.lpszIcon           = MAKEINTRESOURCEW(IDI_LOGIFADEFIX);
                        mbp.dwContextHelpId    = 0;
                        mbp.lpfnMsgBoxCallback = NULL;
                        mbp.dwLanguageId       = 0;
                        MessageBoxIndirectW(&mbp);
                        break;
                    }
                    case IDM_EXIT:
                    {
                        NOTIFYICONDATAW nid;
                        nid.cbSize = sizeof(nid);
                        nid.hWnd   = hwnd;
                        nid.uID    = 0;
                        nid.uFlags = 0;
                        Shell_NotifyIconW(NIM_DELETE, &nid);
                        PostQuitMessage(0);
                        break;
                    }
                }
            }
            return 0;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

void main(void)
{
    WNDCLASSW wc;
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = HINST_THISCOMPONENT;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"LogiFadeFix";
    RegisterClassW(&wc);

    CreateWindowExW(
        0, L"LogiFadeFix", NULL,
        0, 0, 0, 0, 0, NULL, NULL, HINST_THISCOMPONENT, NULL);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    PlaySoundW(NULL, NULL, 0);
    ExitProcess(0);
}