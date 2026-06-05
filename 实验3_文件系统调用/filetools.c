#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

typedef struct {
    int id;
    char name[50];
    int age;
    float score;
} Record;

void create_file() {
    char filename[100];
    printf("请输入要创建的文件名: ");
    scanf("%s", filename);
    
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("创建文件失败");
        return;
    }
    printf("文件 %s 创建成功！\n", filename);
    close(fd);
}

void write_file() {
    char filename[100];
    printf("请输入要写入的文件名: ");
    scanf("%s", filename);
    
    int fd = open(filename, O_WRONLY | O_APPEND);
    if (fd < 0) {
        perror("打开文件失败");
        return;
    }
    
    Record r;
    printf("请输入记录ID: ");
    scanf("%d", &r.id);
    printf("请输入姓名: ");
    scanf("%s", r.name);
    printf("请输入年龄: ");
    scanf("%d", &r.age);
    printf("请输入成绩: ");
    scanf("%f", &r.score);
    
    ssize_t bytes_written = write(fd, &r, sizeof(Record));
    if (bytes_written < 0) {
        perror("写入文件失败");
    } else {
        printf("写入成功！写入了 %zd 字节\n", bytes_written);
    }
    close(fd);
}

void read_file() {
    char filename[100];
    printf("请输入要读取的文件名: ");
    scanf("%s", filename);
    
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("打开文件失败");
        return;
    }
    
    Record r;
    int count = 0;
    printf("\n文件内容:\n");
    printf("ID\t姓名\t年龄\t成绩\n");
    printf("--------------------------------\n");
    
    ssize_t bytes_read;
    while ((bytes_read = read(fd, &r, sizeof(Record))) > 0) {
        printf("%d\t%s\t%d\t%.2f\n", r.id, r.name, r.age, r.score);
        count++;
    }
    
    if (bytes_read < 0) {
        perror("读取文件失败");
    } else if (count == 0) {
        printf("文件为空\n");
    } else {
        printf("\n共读取 %d 条记录\n", count);
    }
    close(fd);
}

void change_permission() {
    char filename[100];
    int mode;
    printf("请输入文件名: ");
    scanf("%s", filename);
    printf("请输入权限模式（八进制，如644、755）: ");
    scanf("%o", &mode);
    
    if (chmod(filename, mode) < 0) {
        perror("修改权限失败");
    } else {
        printf("权限修改成功！\n");
    }
}

void show_permission() {
    char filename[100];
    printf("请输入文件名: ");
    scanf("%s", filename);
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork失败");
    } else if (pid == 0) {
        char *argv[] = {"/bin/ls", "-l", filename, NULL};
        execv("/bin/ls", argv);
        perror("execv失败");
        exit(1);
    } else {
        wait(NULL);
    }
}

int main() {
    int choice;
    do {
        printf("\n===== 文件工具 =====\n");
        printf("0. 退出\n");
        printf("1. 创建新文件\n");
        printf("2. 写文件\n");
        printf("3. 读文件\n");
        printf("4. 修改文件权限\n");
        printf("5. 查看当前文件权限\n");
        printf("请选择操作: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 0:
                printf("退出程序\n");
                break;
            case 1:
                create_file();
                break;
            case 2:
                write_file();
                break;
            case 3:
                read_file();
                break;
            case 4:
                change_permission();
                break;
            case 5:
                show_permission();
                break;
            default:
                printf("无效选项！\n");
        }
    } while (choice != 0);
    
    return 0;
}