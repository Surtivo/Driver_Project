#include <windows.h>
#include <stdio.h>
#include <ViGEmClient.h>

#define VK_KEY_1 0x31  // Mapeia tecla "1"
#define VK_KEY_2 0x32  // Mapeia tecla "2"
#define VK_KEY_3 0x33  // Mapeia tecla "3"
#define VK_KEY_4 0x34  // Mapeia tecla "4"

#define VKEY_UP      0x26  // Tecla seta para cima
#define VKEY_DOWN    0x28  // Tecla seta para baixo
#define VKEY_LEFT    0x25  // Tecla seta para esquerda
#define VKEY_RIGHT   0x27  // Tecla seta para direita


#define XBOX_BUTTON_A   0x1000  // Bot�o A
#define XBOX_BUTTON_B   0x2000  // Bot�o B
#define XBOX_BUTTON_X   0x4000  // Bot�o X
#define XBOX_BUTTON_Y   0x8000  // Bot�o Y

#define AXIS_CENTER 0  // Centro do anal�gico (ponto neutro)
#define AXIS_MIN    -32767  // Posi��o m�xima negativa (extremo esquerdo ou baixo)
#define AXIS_MAX    32767   // Posi��o m�xima positiva (extremo direito ou alto)

PVIGEM_CLIENT client;
PVIGEM_TARGET target;
XUSB_REPORT report;
HANDLE MEU_DISPOSITIVO = (HANDLE)0x0000000000020776;

#define VIGEM_SUCCESS(val) (val == VIGEM_ERROR_NONE)

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INPUT) {
        RAWINPUT raw;
        UINT dwSize = sizeof(RAWINPUT);
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));

        if (raw.header.dwType == RIM_TYPEKEYBOARD) {
            HANDLE deviceHandle = raw.header.hDevice;
            WORD key = raw.data.keyboard.VKey;

            // Verifica se o dispositivo � o controle (comparando o HANDLE)
            if (deviceHandle == MEU_DISPOSITIVO) {
                printf("Tecla detectada do controle (HANDLE: %p): VK %04X\n", deviceHandle, key);

                if (raw.data.keyboard.Flags == 0) { // KEYDOWN
                    WORD buttonState = 0;
                    BOOL mapped = FALSE;

                    switch (key) {
                    case 0x31: buttonState |= XBOX_BUTTON_X; mapped = TRUE; break;  // Tecla '1' -> X
                    case 0x32: buttonState |= XBOX_BUTTON_A; mapped = TRUE; break;  // Tecla '2' -> A
                    case 0x33: buttonState |= XBOX_BUTTON_B; mapped = TRUE; break;  // Tecla '3' -> B
                    case 0x34: buttonState |= XBOX_BUTTON_Y; mapped = TRUE; break;  // Tecla '4' -> Y

                    case VKEY_UP: report.sThumbLY = AXIS_MIN; mapped = TRUE; break;
                    case VKEY_DOWN: report.sThumbLY = AXIS_MAX; mapped = TRUE; break;
                    case VKEY_LEFT: report.sThumbLX = AXIS_MIN; mapped = TRUE; break;
                    case VKEY_RIGHT: report.sThumbLX = AXIS_MAX; mapped = TRUE; break;
                    }

                    if (mapped) {
                        report.wButtons = buttonState;
                        vigem_target_x360_update(client, target, report);
                        printf("Tecla mapeada e simulada no controle: VK %04X\n", key);
                    }
                    else {
                        printf("Tecla detectada mas n�o mapeada.\n");
                    }
                }
            }
            else {
                // Ignora a entrada se o dispositivo n�o for o controle
                printf("Entrada ignorada (n�o � o controle). HANDLE: %p\n", deviceHandle);
            }
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int winmain() {

    // Configura a janela invisivel para capturar entradas RAW
    printf("Registrando janela para Raw Input...\n");

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"RawInputWindowClass";
    RegisterClass(&wc);

    //Cria a janela;
    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, L"RawInputWindow", //Estilos de janela opcionais, classe, texto na janela;
        0, 0, 0, 0, 0,                          //Estilo da janela, tamanho e posições;
        HWND_MESSAGE, NULL, wc.hInstance, NULL  //Janela pai, menu, instance handle, dados de aplicação adicional;
    );

    if (hwnd == NULL) {
        printf("Erro ao criar a janela para Raw Input\n");
        return -1;
    }

    printf("Janela criada com sucesso.\n");

    printf("Raw Input registrado com sucesso.\n");

    // Loop de mensagens para processar as entradas
    printf("Entrando no loop de mensagens...\n");
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);          //Responsável por evocar a função WindowProc indiretamente;
    }

    return 0;
}