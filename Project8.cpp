           #define UNICODE
          #define _UNICODE
          #define TRAY_ID 1
          #define WM_TRAY (WM_USER + 1)
         #include <iostream>
          #include <limits>
          #include <windows.h>
          #include <thread>
          #include <chrono>
          #include <atomic>
          #include <csignal>
          #include <shellapi.h>

          LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
          NOTIFYICONDATA nid{};
          HMENU trayMenu = nullptr;
          std::atomic<int> interval{60}; // default 
          std::atomic<bool> running(true);
          std::atomic<bool> paused(false);

                      void sendkey() {
            INPUT in[2] = {};

            in [0] .type = INPUT_KEYBOARD;
            in [0] .ki.wVk = VK_F12;  // key code 

            in [1] .type = INPUT_KEYBOARD;
            in [1] .ki.wVk = VK_F12;
            in[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, in, sizeof(INPUT));
            }
          

              int WINAPI WinMain (
              HINSTANCE hInstance,
              HINSTANCE,
              LPSTR,
              int nCmdShow
          ) {
            WNDCLASS wc = {};
            wc.lpfnWndProc = WindowProc;
            wc.hInstance = hInstance;
            wc.lpszClassName = L"MyWindowClass";
            RegisterClass(&wc);
            HWND hwnd = CreateWindowEx(
              0,
              L"MyWindowClass",
              L"project8",
              0,
              0, 0, 0, 0,
              nullptr,
              nullptr,
              hInstance,
              nullptr
            );
            MSG msg = {};
          nid.cbSize = sizeof(NOTIFYICONDATA);
          nid.hWnd = hwnd;
          nid.uID = TRAY_ID;
          nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
          nid.uCallbackMessage = WM_TRAY;
          nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
          wcscpy_s(nid.szTip, L"Project8 running");

          Shell_NotifyIcon(NIM_ADD, &nid);

            trayMenu = CreatePopupMenu();
                  AppendMenu(trayMenu, MF_STRING, 1, L"Pause / Resume");
                  AppendMenu(trayMenu, MF_STRING, 2, L"Interval 30s");
                  AppendMenu(trayMenu, MF_STRING, 3, L"Interval 60s");
                  AppendMenu(trayMenu, MF_SEPARATOR, 0, nullptr);
                  AppendMenu(trayMenu, MF_STRING, 4, L"Exit");
                  AppendMenu(trayMenu, MF_SEPARATOR, 0, nullptr);
AppendMenu(trayMenu, MF_STRING, 5, L"Enable autostart");
AppendMenu(trayMenu, MF_STRING, 6, L"Disable autostart");
            std::thread worker([] {
      while (running) {
          if (!paused) {
              sendkey();
          }
          std::this_thread::sleep_for(std::chrono::seconds(interval.load()));
      }
   
  });
  worker.detach();

            while (GetMessage(&msg, nullptr, 0, 0)) {
              TranslateMessage(&msg);
              DispatchMessage(&msg);
            }
            running = false;
            return 0;

          }


                    void Enableautostart () {
            HKEY hKey;
            const wchar_t* path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";


            wchar_t exePath[MAX_PATH];
            GetModuleFileNameW(nullptr, exePath, MAX_PATH); 

            if (RegOpenKeyExW(HKEY_CURRENT_USER, path, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
                RegSetValueExW(hKey, L"Project8", 0, REG_SZ, (BYTE*)exePath, (wcslen(exePath) + 1) * sizeof(wchar_t));
                RegCloseKey(hKey);
            }
            }
          void Disableautostart () {
            HKEY hKey;
            const wchar_t* path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
            if (RegOpenKeyExW(HKEY_CURRENT_USER, path, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
              RegDeleteValueW(hKey, L"Project8");
              RegCloseKey(hKey);
          }
          }

          void OnExit (int) {
            running = false;
          }
              
            LRESULT CALLBACK WindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
            switch (msg) {
              case WM_DESTROY:
                running = false;
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
                return 0;
            

                case WM_TRAY:
      if (lParam == WM_RBUTTONUP) {
          POINT p;
          GetCursorPos(&p);
          SetForegroundWindow(hwnd);
          TrackPopupMenu(trayMenu , TPM_RIGHTBUTTON, p.x, p.y, 0, hwnd, nullptr);
      }
      return 0;
                case WM_COMMAND:
      switch (LOWORD(wParam)) {
          case 1: paused = !paused; break;
          case 2: interval = 30; break;
          case 3: interval = 60; break;
          case 4:
              Shell_NotifyIcon(NIM_DELETE, &nid);
              PostQuitMessage(0);
              break;
               case 5: Enableautostart(); break;
        case 6: Disableautostart(); break;
      }
      return 0;     
        default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }
      }
  
