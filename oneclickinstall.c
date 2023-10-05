#include <stdio.h>
#include <windows.h>

int main() {
    const char* destinationFolder = "C:\\Program Files\\device";
    const char* executableName = "device.exe";
    const char* sourcePath = "device.exe"; // 设定为 device.exe 的路径

    // 创建目标文件夹
    if (!CreateDirectoryA(destinationFolder, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        printf("无法创建目标文件夹。\n");
        return 1;
    }

    // 构建目标文件的完整路径
    char destinationPath[MAX_PATH];
    snprintf(destinationPath, sizeof(destinationPath), "%s\\%s", destinationFolder, executableName);

    // 复制文件
    if (!CopyFileA(sourcePath, destinationPath, FALSE)) {
        printf("复制文件失败。\n");
        return 1;
    }
    printf("%s 已成功复制到 %s。\n", executableName, destinationFolder);

    // 打开或创建注册表键
    HKEY hKey;
    const char* keyPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, keyPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
        printf("无法打开或创建注册表键。\n");
        return 1;
    }

    // 设置启动项
    if (RegSetValueExA(hKey, executableName, 0, REG_SZ, (BYTE*)destinationPath, strlen(destinationPath) + 1) != ERROR_SUCCESS) {
        printf("无法设置启动项。\n");
        RegCloseKey(hKey);
        return 1;
    }

    printf("%s 已成功添加到自动启动项。\n", executableName);

    // 关闭注册表键
    RegCloseKey(hKey);

    return 0;
}
