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


#define XBOX_BUTTON_A   0x1000  // Botão A
#define XBOX_BUTTON_B   0x2000  // Botão B
#define XBOX_BUTTON_X   0x4000  // Botão X
#define XBOX_BUTTON_Y   0x8000  // Botão Y

#define AXIS_CENTER 0  // Centro do analógico (ponto neutro)
#define AXIS_MIN    -32767  // Posição máxima negativa (extremo esquerdo ou baixo)
#define AXIS_MAX    32767   // Posição máxima positiva (extremo direito ou alto)

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

            // Verifica se o dispositivo é o controle (comparando o HANDLE)
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
                        printf("Tecla detectada mas não mapeada.\n");
                    }
                }
            }
            else {
                // Ignora a entrada se o dispositivo não for o controle
                printf("Entrada ignorada (não é o controle). HANDLE: %p\n", deviceHandle);
            }
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    // Inicializa o ViGEm
    printf("Inicializando ViGEm...\n");
    client = vigem_alloc();
    if (client == NULL) {
        printf("Erro ao alocar cliente ViGEm\n");
        return -1;
    }

    if (vigem_connect(client) != VIGEM_ERROR_NONE) {
        printf("Erro ao conectar ao ViGEm\n");
        return -1;
    }

    printf("Cliente ViGEm alocado e conectado com sucesso.\n");

    // Aloca o alvo X360
    target = vigem_target_x360_alloc();
    if (target == NULL) {
        printf("Erro ao alocar o alvo X360\n");
        return -1;
    }

    printf("Alvo X360 alocado com sucesso.\n");

    // Adiciona o alvo ao cliente
    if (vigem_target_add(client, target) != VIGEM_ERROR_NONE) {
        printf("Erro ao adicionar alvo ao cliente\n");
        return -1;
    }

    printf("Alvo X360 adicionado com sucesso.\n");

    // Inicializa o relatório XUSB_REPORT
    ZeroMemory(&report, sizeof(XUSB_REPORT));

    /*/ Envia um update neutro (botões soltos, analógicos no centro)
    VIGEM_ERROR update_result = vigem_target_x360_update(client, target, report);
    if (update_result != VIGEM_ERROR_NONE) {
        printf("Erro ao enviar estado inicial: 0x%X\n", update_result);
    }*/

    // Configura a janela invisível para capturar entradas RAW
    printf("Registrando janela para Raw Input...\n");

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

    if (hwnd == NULL) {
        printf("Erro ao criar a janela para Raw Input\n");
        return -1;
    }

    printf("Janela criada com sucesso.\n");

    // Registra o Raw Input para o teclado
    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = 0x01;  // Página de dispositivos genéricos
    rid[0].usUsage = 0x06;      // Uso do teclado
    rid[0].dwFlags = RIDEV_INPUTSINK; // Recebe input enquanto a janela não estiver em foco
    rid[0].hwndTarget = hwnd;
    if (RegisterRawInputDevices(rid, 1, sizeof(rid[0])) == FALSE) {
        printf("Erro ao registrar o Raw Input\n");
        return -1;
    }

    printf("Raw Input registrado com sucesso.\n");

    // Loop de mensagens para processar as entradas
    printf("Entrando no loop de mensagens...\n");
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Limpeza e desconexão do ViGEm
    vigem_target_remove(client, target);
    vigem_target_free(target);
    vigem_disconnect(client);
    vigem_free(client);

    return 0;
}