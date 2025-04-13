#include <windows.h>
#include <stdio.h>

#define MAX_DEVICES 128

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INPUT) {
        UINT dwSize = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

        BYTE lpb[1024];
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
            return 0;

        RAWINPUT* raw = (RAWINPUT*)lpb;

        if (raw->header.dwType == RIM_TYPEKEYBOARD) {
            RAWKEYBOARD keyboard = raw->data.keyboard;

            // Aqui você pode pegar o código da tecla e o ID do dispositivo
            printf("Dispositivo: %p, Tecla: 0x%X (%d)\n",
                raw->header.hDevice,
                keyboard.VKey,
                keyboard.VKey
            );
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    // Criar uma janela invisível apenas para receber mensagens
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"RawInputWindowClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, L"RawInputWindow",
        0, 0, 0, 0, 0,
        HWND_MESSAGE, NULL, wc.hInstance, NULL
    );

    // Registrar dispositivos de entrada do tipo teclado
    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x06;  // Teclado
    rid[0].dwFlags = RIDEV_INPUTSINK;
    rid[0].hwndTarget = hwnd;
    RegisterRawInputDevices(rid, 1, sizeof(rid[0]));

    // Loop de mensagens
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
