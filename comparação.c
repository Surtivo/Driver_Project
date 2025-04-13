// Mapeia todos os botões
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INPUT) {
        // Processa a entrada de teclado RAW
        RAWINPUT raw;
        UINT dwSize = sizeof(RAWINPUT);
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));

        // Verifica se o dispositivo é o teclado
        if (raw.header.dwType == RIM_TYPEKEYBOARD) {
            WORD key = raw.data.keyboard.VKey;
            printf("Tecla pressionada: 0x%X\n", key); // Adicionei um printf para depuração
            WORD buttonState = 0;

            // Mapeamento das teclas para os botões do controle Xbox
            switch (key) {
            case 0x31: buttonState |= XBOX_BUTTON_A; break;  // Tecla '1' -> A
            case 0x32: buttonState |= XBOX_BUTTON_B; break;  // Tecla '2' -> B
            case 0x33: buttonState |= XBOX_BUTTON_X; break;  // Tecla '3' -> X
            case 0x34: buttonState |= XBOX_BUTTON_Y; break;  // Tecla '4' -> Y

            case VKEY_UP: report.sThumbLY = AXIS_MIN; break; // Tecla seta para cima
            case VKEY_DOWN: report.sThumbLY = AXIS_MAX; break; // Tecla seta para baixo
            case VKEY_LEFT: report.sThumbLX = AXIS_MIN; break; // Tecla seta para esquerda
            case VKEY_RIGHT: report.sThumbLX = AXIS_MAX; break; // Tecla seta para direita
            }

            // Atualiza o estado do controle
            if (buttonState != 0) {
                report.wButtons = buttonState;
                vigem_target_x360_update(client, target, report);  // Envia o estado dos botões
            }
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Mapeia todos os botões e funciona como controle (também mapeia o teclado)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INPUT) {
        // Processa a entrada de teclado RAW
        RAWINPUT raw;
        UINT dwSize = sizeof(RAWINPUT);
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));

        // Verifica se o dispositivo é o teclado
        if (raw.header.dwType == RIM_TYPEKEYBOARD) {
            WORD key = raw.data.keyboard.VKey;
            printf("Tecla pressionada: 0x%X\n", key); // Adicionei um printf para depuração
            WORD buttonState = 0;

            // Mapeamento das teclas para os botões do controle Xbox
            switch (key) {
            case 0x31: buttonState |= XBOX_BUTTON_A; break;  // Tecla '1' -> A
            case 0x32: buttonState |= XBOX_BUTTON_B; break;  // Tecla '2' -> B
            case 0x33: buttonState |= XBOX_BUTTON_X; break;  // Tecla '3' -> X
            case 0x34: buttonState |= XBOX_BUTTON_Y; break;  // Tecla '4' -> Y

            case VKEY_UP: report.sThumbLY = AXIS_MIN; break; // Tecla seta para cima
            case VKEY_DOWN: report.sThumbLY = AXIS_MAX; break; // Tecla seta para baixo
            case VKEY_LEFT: report.sThumbLX = AXIS_MIN; break; // Tecla seta para esquerda
            case VKEY_RIGHT: report.sThumbLX = AXIS_MAX; break; // Tecla seta para direita
            }

            // Atualiza o estado do controle
            if (buttonState != 0) {
                report.wButtons = buttonState;
                vigem_target_x360_update(client, target, report);  // Envia o estado dos botões
            }
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//Funciona apenas as teclas A, B, Y, X
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INPUT) {
        // Processa a entrada de teclado RAW
        RAWINPUT raw;
        UINT dwSize = sizeof(RAWINPUT);
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));

        // Verifica se o dispositivo é o teclado
        if (raw.header.dwType == RIM_TYPEKEYBOARD) {
            if (raw.data.keyboard.Flags == 0) {
                // Mapeia a tecla para o botão do controle
                WORD key = raw.data.keyboard.VKey;
                WORD buttonState = 0;

                // Mapeamento das teclas para os botões do controle Xbox
                switch (key) {
                    case 0x31: buttonState |= XBOX_BUTTON_X; break;  // Tecla '1' -> A
                    case 0x32: buttonState |= XBOX_BUTTON_A; break;  // Tecla '2' -> B
                    case 0x33: buttonState |= XBOX_BUTTON_B; break;  // Tecla '3' -> X
                    case 0x34: buttonState |= XBOX_BUTTON_Y; break;  // Tecla '4' -> Y
                    
                    case VKEY_UP: report.sThumbLY = AXIS_MIN; break; // Tecla 'W' move para cima no eixo Y
                    case VKEY_DOWN: report.sThumbLY = AXIS_MAX; break; // Tecla 'S' move para baixo no eixo Y
                    case VKEY_LEFT: report.sThumbLX = AXIS_MIN; break; // Tecla 'A' move para a esquerda no eixo X
                    case VKEY_RIGHT: report.sThumbLX = AXIS_MAX; break; // Tecla 'D' move para a direita no eixo X
                }

                // Atualiza o estado do controle
                if (buttonState != 0) {
                    report.wButtons = buttonState;
                    printf("Botao pressionado: %d\n", key);
                    vigem_target_x360_update(client, target, report);  // Envia o estado dos botões
                }
            }
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}






// Oq estava;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INPUT) {
        // Processa a entrada de teclado RAW
        RAWINPUT raw;
        UINT dwSize = sizeof(RAWINPUT);
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));

        // Verifica se o dispositivo é o teclado
        if (raw.header.dwType == RIM_TYPEKEYBOARD) {
            if (raw.data.keyboard.Flags == 0) {
                // Mapeia a tecla para o botão do controle
                WORD key = raw.data.keyboard.VKey;
                WORD buttonState = 0;

                // Mapeamento das tesclas para os botões do controle Xbox
                switch (key) {
                case 0x31: buttonState |= XBOX_BUTTON_X; break;  // Tecla '1' -> A
                case 0x32: buttonState |= XBOX_BUTTON_A; break;  // Tecla '2' -> B
                case 0x33: buttonState |= XBOX_BUTTON_B; break;  // Tecla '3' -> X
                case 0x34: buttonState |= XBOX_BUTTON_Y; break;  // Tecla '4' -> Y

                case VKEY_UP: report.sThumbLY = AXIS_MIN; break; // Tecla 'W' move para cima no eixo Y
                case VKEY_DOWN: report.sThumbLY = AXIS_MAX; break; // Tecla 'S' move para baixo no eixo Y
                case VKEY_LEFT: report.sThumbLX = AXIS_MIN; break; // Tecla 'A' move para a esquerda no eixo X
                case VKEY_RIGHT: report.sThumbLX = AXIS_MAX; break; // Tecla 'D' move para a direita no eixo X
                }

                // Atualiza o estado do controle
                if (buttonState != 0) {
                    report.wButtons = buttonState;
                    printf("Botao pressionado: %d\n", key);
                    vigem_target_x360_update(client, target, report);  // Envia o estado dos botões
                }
            }
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
