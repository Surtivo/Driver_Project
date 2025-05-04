#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tchar.h>      //Para usar TCHAR e funções relacionadas (ex: _tcsstr, _tcscpy, etc.) deixando o código modular para ANSI ou UNICODE;
#include <ViGEmClient.h>

//------------------CONTROLE GENÉRICO------------------//

//Definições de teclas do Controle Genérico;
#define VK_KEY_1 0x31  //Mapeia tecla "1";
#define VK_KEY_2 0x32  //Mapeia tecla "2";
#define VK_KEY_3 0x33  //Mapeia tecla "3";
#define VK_KEY_4 0x34  //Mapeia tecla "4";

//Botões de ombro, gatilhos e analógicos do Controle Genérico;
#define VK_KEY_5 0x35  //Mapeia tecla "5" "L1";
#define VK_KEY_7 0x37  //Mapeia tecla "7" "L2";
#define VK_KEY_9 0x39  //Mapeia tecla "9" "L3";
#define VK_KEY_6 0x36  //Mapeia tecla "6" "R1";
#define VK_KEY_8 0x38  //Mapeia tecla "8" "R2";
#define VK_KEY_0 0x30  //Mapeia tecla "9" "R3";

//Botões DPAD do Controle Genérico;
#define VK_KEY_W 0x57  //Mapeia tecla "W" que é o "Cima" do controle;
#define VK_KEY_A 0x41  //Mapeia tecla "A" que é o "Esquerda" do controle;
#define VK_KEY_S 0x53  //Mapeia tecla "S" que é o "Baixo" do controle;
#define VK_KEY_D 0x44  //Mapeia tecla "D" que é o "Direita" do controle;

//Analógicos do Controle Genérico;
//Esquerdo;
#define VK_KEY_UP 0x26      //Mapeia tecla "Seta para Cima" que é o "Cima" do controle;
#define VK_KEY_DOWN 0x28    //Mapeia tecla "Seta para Baixo" que é o "Baixo" do controle;
#define VK_KEY_LEFT 0x25    //Mapeia tecla "Seta para Esquerda" que é o "Esquerda" do controle;
#define VK_KEY_RIGHT 0x27   //Mapeia tecla "Seta para Direita" que é o "Direita" do controle;
//Direito;
#define VK_KEY_UPRIGHT 0x49     //Mapeia tecla "I" que é o "Cima" do controle;
#define VK_KEY_DOWNRIGHT 0x4B   //Mapeia tecla "K" que é o "Baixo" do controle;
#define VK_KEY_UPLEFT 0x4A      //Mapeia tecla "J" que é o "Esquerda" do controle;
#define VK_KEY_DOWNLEFT 0x4C    //Mapeia tecla "L" que é o "Direita" do controle;

//Botões START e SELECT do Controle Genérico;
#define VK_KEY_BACKSPASCE 0x08  //Mapeia tecla "Backspace" que é o "Select" do controle;
#define VK_KEY_ENTER 0x0D       //Mapeia tecla "Enter" que é o "Start" do controle;


//------------------CONTROLE XBOX 360------------------//

//Botões A, B, X e Y do controle Xbox 360;
#define XBOX_BUTTON_A   0x1000  //Botao A;
#define XBOX_BUTTON_B   0x2000  //Botao B;
#define XBOX_BUTTON_X   0x4000  //Botao X;
#define XBOX_BUTTON_Y   0x8000  //Botao Y;

//Botões de ombro e gatilhos do controle Xbox 360;
#define XBOX_BUTTON_RIGHSHOULDER 0x0200 //Botão R1;
#define XBOX_BUTTON_LEFTSHOULDER 0x0100 //Botão L1;
#define XBOX_BUTTON_RIGHTTRIGGER_FULLY_PRESSED 255; //Botão R2;
#define XBOX_BUTTON_LEFTTRIGGER_FULLY_PRESSED 255; //Botão L2;

//"Start" e "Select" do controle Xbox 360;
#define XBOX_BUTTON_START 0x0010 //Botão "Start";
#define XBOX_BUTTON_SELECT 0x0020 //Botão "Select";

//Botões analógicos do controle Xbox 360;
#define XBOX_BUTTON_LEFTTHUMB 0x0040 //Botão "L3";
#define XBOX_BUTTON_RIGHTTHUMB 0x0080 //Botão "R3";

//Analógicos do controle Xbox 360;
#define AXIS_CENTER  0 //Centro do analógico;
#define AXIS_MIN  -32767 //Valor mínimo do analógico (extrema esquerda ou baixo);
#define AXIS_MAX  32767 //Valor máximo do analógico (extrema direita ou cima);

//Estrutura para o controle Xbox 360;
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
            //system("cls");
            if (raw->data.keyboard.Flags == 0 || raw->data.keyboard.Flags == 2) {   //Processa o KEYDOWN (apertar o botão). Isso filtra para não computar o KEYUP (soltar o botão);
                printf("Tecla reconhecida pelo dispositivo certo!\n");
                switch (raw->data.keyboard.VKey) {  //Traduzir um botão do controle para um controle de xbox;

                    //Esses casos não permitem os botões que dependem das flags serem apertados juntos!!!;

					//Botões de ação;
                    case VK_KEY_1: report.wButtons = XBOX_BUTTON_X; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
                    case VK_KEY_2: report.wButtons = XBOX_BUTTON_A; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
                    case VK_KEY_3: report.wButtons = XBOX_BUTTON_B; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
                    case VK_KEY_4: report.wButtons = XBOX_BUTTON_Y; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					
                    //Botões de ombro;
                    case VK_KEY_5: report.wButtons = XBOX_BUTTON_LEFTSHOULDER; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_6: report.wButtons = XBOX_BUTTON_RIGHSHOULDER; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					
                    //Gatilhos;
                    case VK_KEY_7: report.bLeftTrigger = XBOX_BUTTON_LEFTTRIGGER_FULLY_PRESSED; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_8: report.bRightTrigger = XBOX_BUTTON_RIGHTTRIGGER_FULLY_PRESSED; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;

					//Botões de menu;
					case VK_KEY_BACKSPASCE: report.wButtons = XBOX_BUTTON_SELECT; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_ENTER: report.wButtons = XBOX_BUTTON_START; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;

                    //Botões Analogicos;
					case VK_KEY_0: report.wButtons = XBOX_BUTTON_RIGHTTHUMB; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_9: report.wButtons = XBOX_BUTTON_LEFTTHUMB; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;

                    //Botões DPAD;
					case VK_KEY_W: report.wButtons = XUSB_GAMEPAD_DPAD_UP; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_A: report.wButtons = XUSB_GAMEPAD_DPAD_LEFT; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_S: report.wButtons = XUSB_GAMEPAD_DPAD_DOWN; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_D: report.wButtons = XUSB_GAMEPAD_DPAD_RIGHT; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;

					//Analógico Esquerdo;
					case VK_KEY_UP: report.sThumbLY = AXIS_MAX; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_DOWN: report.sThumbLY = AXIS_MIN; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_LEFT: report.sThumbLX = AXIS_MIN; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_RIGHT: report.sThumbLX = AXIS_MAX; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;

					//Analógico Direito;
					case VK_KEY_UPRIGHT: report.sThumbRY = AXIS_MAX; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_DOWNRIGHT: report.sThumbRY = AXIS_MIN; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_UPLEFT: report.sThumbRX = AXIS_MIN; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
					case VK_KEY_DOWNLEFT: report.sThumbRX = AXIS_MAX; vigem_target_x360_update(client, target, report); printf("Tecla mapeada e simulada no controle: VK %04X\n", raw->data.keyboard.VKey); break;
                    
                }
            }
            else if (raw->data.keyboard.Flags == 1) {
				report.wButtons = 0;	        //Solta os botões normais (A, B, R1, etc); - PROBLEMA: APENAS APERTO UM BOTÃO POR VEZ;
                report.bLeftTrigger = 0;        //Solta os gatilhos, pois eles são manipulados diretamente; - PROBLEMA: SOLTA O GATILHO E O ANALÓGICO SEMPRE QUE QUALQUER OUTRO BOTÃO
				report.bRightTrigger = 0;       //Solta os gatilhos, pois eles são manipulados diretamente;             QUE DEPENDA DA FLAG FOR SOLTO! PARA OS ANALÓGICOS A TROCA DE DIREÇÕES NÃO É MUITO BEM FEITA. 
                report.sThumbRX = AXIS_CENTER;  //Solta o analógico direito;                                            DELAY PRA TROCAR ALGUMAS DIREÇÕES;
                report.sThumbRY = AXIS_CENTER;  //Solta o analógico direito;
                vigem_target_x360_update(client, target, report);                                                
                printf("Tecla foi solta\n");
            }
            else if (raw->data.keyboard.Flags == 3) {
                report.sThumbLX = AXIS_CENTER;  //Solta o analógico esquerdo;
                report.sThumbLY = AXIS_CENTER;  //Solta o analógico esquerdo;
                vigem_target_x360_update(client, target, report);
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