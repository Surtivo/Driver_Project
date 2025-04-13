#include <Windows.h>
#include <ViGEmClient.h>
#include <stdio.h>

int main() {
    PVIGEM_CLIENT client = vigem_alloc();
    if (vigem_connect(client) != VIGEM_ERROR_NONE) {
        printf("Erro ao conectar com ViGEmBus.\n");
        return 1;
    }

    PVIGEM_TARGET target = vigem_target_x360_alloc();
    if (vigem_target_add(client, target) != VIGEM_ERROR_NONE) {
        printf("Erro ao criar controle virtual.\n");
        return 1;
    }

    printf("Controle Xbox virtual iniciado! Pressione ESC para sair.\n");

    while (1) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            break;

        XUSB_REPORT report = { 0 };

        if (GetAsyncKeyState('2') & 0x8000)
            report.wButtons |= XUSB_GAMEPAD_A;
        if (GetAsyncKeyState('3') & 0x8000)
            report.wButtons |= XUSB_GAMEPAD_B;
        if (GetAsyncKeyState('1') & 0x8000)
            report.wButtons |= XUSB_GAMEPAD_X;
        if (GetAsyncKeyState('4') & 0x8000)
            report.wButtons |= XUSB_GAMEPAD_Y;

        vigem_target_x360_update(client, target, report);
        Sleep(10);
    }

    vigem_target_remove(client, target);
    vigem_target_free(target);
    vigem_free(client);

    printf("Finalizado.\n");
    return 0;
}
