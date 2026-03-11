#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 安全输入函数：读取输入并处理换行符/异常，无多余依赖
char* input(const char* prompt) {
    // 静态缓冲区（精简版，兼顾安全与易用）
    static char buf[100];
    // 清空缓冲区，避免残留数据
    memset(buf, 0, sizeof(buf));

    // 打印提示（仅当提示不为空时）
    if (prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }

    // 读取输入并处理异常
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        fprintf(stderr, "输入错误！\n");
        return buf;
    }

    // 去除换行符，兼容超长输入
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        // 清空超长输入残留，避免影响后续逻辑
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
    }

    return buf;
}

int main() {
    // 核心：打印HelloWorld
    printf("HelloWorld!\n");
    
    // 核心：input函数功能，无多余提示
    char* user_input = input("请输入：");
    printf("你输入的是：%s\n", user_input);

    return 0;
}
