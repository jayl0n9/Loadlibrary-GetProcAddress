#include<windows.h>
#include<stdio.h>
extern "C" PVOID64 _cdecl GetInit();
extern "C" PVOID64 _cdecl GetExportTable(void* baseAddress);

typedef struct _EXPORT_DIRECTORY {
    DWORD   Characteristics;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;
    WORD    MinorVersion;
    DWORD   Name;
    DWORD   Base;
    DWORD   NumberOfFunctions;
    DWORD   NumberOfNames;
    DWORD   AddressOfFunctions; 
    DWORD   AddressOfNames;    
    DWORD   AddressOfNameOrdinals;
} EXPORT_DIRECTORY;
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
}UNICODE_STRING, * PUNICODE_STRING;

HMODULE myLoadlibrary(char* str) {
    UNICODE_STRING* FullName;
    LIST_ENTRY* pNode = NULL;
    pNode = (LIST_ENTRY*)GetInit();
    while (true)
    {
        FullName = (UNICODE_STRING*)((BYTE*)pNode + 0x38);

        int requiredSize = WideCharToMultiByte(CP_UTF8, 0, FullName->Buffer, FullName->Length / 2, NULL, 0, NULL, NULL);
        char* dllName = (char*)malloc(requiredSize + 1);
        WideCharToMultiByte(CP_UTF8, 0, FullName->Buffer, FullName->Length / 2, dllName, requiredSize, NULL, NULL);
        dllName[requiredSize] = '\0';

        if (strcmp(dllName, str)==0)
        {
            return (HMODULE)(*((ULONG64*)((BYTE*)pNode + 0x10)));
            break;
        }
        pNode = pNode->Flink;
    }
}

void* myGetProcAddress(void* baseAddress, char* str) {
    void* retFunctionAddress = NULL;
    IMAGE_EXPORT_DIRECTORY* exportDirectory = (IMAGE_EXPORT_DIRECTORY*)GetExportTable(baseAddress);
    DWORD* functionAddresses = (DWORD*)((BYTE*)baseAddress + exportDirectory->AddressOfFunctions);

    DWORD* nameAddresses = (DWORD*)((BYTE*)baseAddress + exportDirectory->AddressOfNames);

    WORD* nameOrdinals = (WORD*)((BYTE*)baseAddress + exportDirectory->AddressOfNameOrdinals);
    int countzwfunc = 0;
    for (DWORD i = 0; i < exportDirectory->NumberOfNames; i++) {
        char* functionName = (char*)((BYTE*)baseAddress + nameAddresses[i]);
        
        if (strcmp(functionName, str) == 0) {
            DWORD ordinal = nameOrdinals[i];
            retFunctionAddress = (void*)((BYTE*)baseAddress + functionAddresses[ordinal]);
        }
    }
    if (retFunctionAddress == NULL ) {
        return NULL;
    }
    
    return retFunctionAddress;
}


int main(int argc, TCHAR* argv[], TCHAR* envp[])
{

    HMODULE kernel32 = NULL;
    kernel32 = myLoadlibrary("KERNEL32.dll");
    void* ZwAllocateVirtualMemorySSN = myGetProcAddress(kernel32, ZwAllocateVirtualMemory);
};
