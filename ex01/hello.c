#include <stdio.h>
#include <string.h>

// 提示用户输入并返回处理后的字符串
char* input(const char* prompt) {
    static char input_buf[100];
    size_t newline_pos;
    
    printf("%s", prompt);
    
    // 处理输入异常
    if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
        printf("输入出错！\n");
        input_buf[0] = '\0';
        return input_buf;
    }
    
    // 去除输入字符串末尾的换行符
    newline_pos = strcspn(input_buf, "\n");
    input_buf[newline_pos] = '\0';
    
    return input_buf;
}

int main() {
    printf("HelloWorld!\n");
    
    char* user_input = input("请输入内容：");
    printf("你输入的内容是：%s\n", user_input);
    
    return 0;
}
