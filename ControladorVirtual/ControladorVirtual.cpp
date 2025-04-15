#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tchar.h>      //Para usar TCHAR e funções relacionadas (ex: _tcsstr, _tcscpy, etc.) deixando o código modular para ANSI ou UNICODE;
#include <ViGEmClient.h>

#define VK_KEY_1 0x31  // Mapeia tecla "1"
#define VK_KEY_2 0x32  // Mapeia tecla "2"
#define VK_KEY_3 0x33  // Mapeia tecla "3"
#define VK_KEY_4 0x34  // Mapeia tecla "4"

#define XBOX_BUTTON_A   0x1000  // Botao A
#define XBOX_BUTTON_B   0x2000  // Botao B
#define XBOX_BUTTON_X   0x4000  // Botao X
#define XBOX_BUTTON_Y   0x8000  // Botao Y

PVIGEM_CLIENT client;
PVIGEM_TARGET target;
XUSB_REPORT report;
#define VIGEM_SUCCESS(val) (val == VIGEM_ERROR_NONE)

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INPUT) {     //Verifica se o código da mensagem foi um WM_INPUT (entrada pelo raw input);

        UINT dwSize = 0;        /*Aloca uma variável unsigned int para onde serão enviados os tamanhos em bytes usados para alocar o buffer que conterá os reais dados
de entrda (lbp). É feito desta forma para que possamos ter uma forma dinâmica para se detectar o tamnho que virá dependendo do dispositivo (teclado, mouse, hid).
Como da pData (3° intem) foi definida como null, o tamanho necessário será retornando para o enderço de pcbSize (dwSize);*/

        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));    /*Passa lParam como um tipo (HRAWINPUT) onde ele contém a estrutura bruta
do dispositivo (tipo, id, etc), o tipo de comando, NULL foi falado acima, &dwSize também, e informa o tamanho da estrutura de cabeçalho esperada. Parâmetros da função; */

        LPBYTE lpb = (LPBYTE)malloc(dwSize);    //Agora podemos alocar memória para o tamanho certo. LPBYTE = BYTE*;
        if (lpb == NULL)    return -1;          //Falhou em alocar;

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) { //Comparação feita para evitar que ocorram erros.
            free(lpb);                          //Caso os valores sejam diferentes significa que houve algo errado, pois agora a função GetRawInputData deve retornar o
            return -1;                          //número de bytes que foram copiados para pData (lpb)
        }

        RAWINPUT* raw = (RAWINPUT*)lpb;         //lbp é um ponteiro genérico que pode apontar qualquer dado. Ele aponta para um tipo RAWINPUT, por isso a conversão;
        //Parte para identificação do controle correto. Está adpatado para ser compilado em UNICODE com adptação dinâmica usando TCHAR (que verifica se é UNICODE ou ANSI).
        //Se for ANSI assumirá TCHAR = CHAR e se for UNICODE TCHAR = WCHAR;
        TCHAR DeviceName[256];
        UINT size = sizeof(DeviceName);
        GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICENAME, DeviceName, &size);
        _tprintf(_T("Nome do dispositivo: %s\n"), DeviceName);

        if (_tcsstr(DeviceName, _T("_VID&00021949")) && _tcsstr(DeviceName, _T("_PID&0402")) != NULL) { //Verifica se o dispositivo é o que queremos;
            if (raw->data.keyboard.Flags == 0) {   //Processa o KEYDOWN (apertar o botão). Isso filtra para não computar o KEYUP (soltar o botão);
                printf("Tecla reconhecida pelo dispositivo certo!\n");
                switch (raw->data.keyboard.VKey) {
                case VK_KEY_1: report.wButtons = XBOX_BUTTON_X; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
                case VK_KEY_2: report.wButtons = XBOX_BUTTON_A; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
                case VK_KEY_3: report.wButtons = XBOX_BUTTON_B; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
                case VK_KEY_4: report.wButtons = XBOX_BUTTON_Y; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
                }
            }
            else if (raw->data.keyboard.Flags == 1) {
                report.wButtons = 0;
                vigem_target_x360_update(client, target, report);
                printf("Tecla foi solta\n");
            }
        }
        else {
            printf("Tecla ignorada. Nao e o dispositivo procurado!\n");
        }

        printf("\n");

        free(lpb);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);       //Trata e processa mensagens que não foram tratadas pela minha função;
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

    // Inicializa o relatorio XUSB_REPORT
    ZeroMemory(&report, sizeof(XUSB_REPORT));

    //Envia um update neutro (botoes soltos, analogicos no centro)
    VIGEM_ERROR update_result = vigem_target_x360_update(client, target, report);
    if (update_result != VIGEM_ERROR_NONE) {
        printf("Erro ao enviar estado inicial: 0x%X\n", update_result);
    }

    //Baseado na documentação de criação de janelas (MicrosoftLearn);
    //Registrar classe de janela;
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"RawInputWindowClass";
    RegisterClass(&wc);

    //Cria janela;
    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, L"RawInputWindow",             //Estilos de janela opcionais, classe, texto na janela;
        0, 0, 0, 0, 0,                                      //Estilo da janela, tamanho e posições;
        HWND_MESSAGE, NULL, wc.hInstance, NULL              //Janela pai, menu, instance handle, dados de aplicação adicional;
    );

    if (hwnd == NULL) {
        printf("Erro ao criar a janela para Raw Input\n");
        return -1;
    }
    printf("Janela criada com sucesso.\n");

    //Registrar dispositivo teclado;
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;             // Generic desktop controls;
    rid.usUsage = 0x06;                 //Keyboard;
    rid.dwFlags = RIDEV_INPUTSINK;      //Recebe input mesmo sem foco;
    rid.hwndTarget = hwnd;

    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        printf("Erro ao registrar dispositivos RAWINPUT\n");
        return 1;
    }
    printf("Raw Input registrado com sucesso.\n");

    //Loop de mensagens;
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);          //Responsável por evocar a função WindowProc indiretamente; 
    }

    return 0;

}

/*
Controle;
\\?\HID#{00001124-0000-1000-8000-00805f9b34fb}_VID&00021949_PID&0402&Col01#a&16dd6de9&0&0000#{884b96c3-56ef-11d1-bc8c-00a0c91405dd}

Teclado;
\\?\HID#VID_048D&PID_C966&Col02#7&1ccb74b1&0&0001#{884b96c3-56ef-11d1-bc8c-00a0c91405dd}
*/