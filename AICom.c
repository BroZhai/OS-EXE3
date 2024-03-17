#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_PLAYERS 4
#define NUM_ROUNDS 13

void play_round(int player_id, int read_fd, int write_fd);

int main() {
    pid_t pid[NUM_PLAYERS];
    int read_fds[NUM_PLAYERS];
    int write_fds[NUM_PLAYERS];
    int player_ids[NUM_PLAYERS];

    // 创建管道
    for (int i = 0; i < NUM_PLAYERS; i++) {
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) {
            perror("pipe");
            exit(1);
        }
        read_fds[i] = pipe_fd[0];
        write_fds[i] = pipe_fd[1];
    }

    // 创建子进程
    for (int i = 0; i < NUM_PLAYERS; i++) {
        pid[i] = fork();
        if (pid[i] < 0) {
            perror("fork");
            exit(1);
        } else if (pid[i] == 0) {
            // 子进程
            close(read_fds[i]);  // 关闭读取管道
            close(write_fds[(i + 1) % NUM_PLAYERS]);  // 关闭下一个玩家的写入管道
            player_ids[i] = getpid();
            play_round(i + 1, read_fds[(i + NUM_PLAYERS - 1) % NUM_PLAYERS], write_fds[i]);
            exit(0);
        } else {
            player_ids[i] = pid[i];
        }
    }

    // 父进程
    printf("Parent pid %d: child players are", getpid());
    for (int i = 0; i < NUM_PLAYERS; i++) {
        printf(" %d", player_ids[i]);
    }
    printf("\n");

    // 发送初始牌面信息
    char initial_cards[256];
    fgets(initial_cards, sizeof(initial_cards), stdin);
    for (int i = 0; i < NUM_PLAYERS; i++) {
        write(write_fds[i], initial_cards, strlen(initial_cards) + 1);
    }

    // 等待子进程结束
    for (int i = 0; i < NUM_PLAYERS; i++) {
        waitpid(pid[i], NULL, 0);
    }

    printf("Parent pid %d: game completed\n", getpid());
    printf("Parent pid %d: score = <%d %d %d %d>\n", getpid(), scores[0], scores[1], scores[2], scores[3]);

    return 0;
}

void play_round(int player_id, int read_fd, int write_fd) {
    char cards[256];
    read(read_fd, cards, sizeof(cards));

    // 处理牌面信息
    // ...

    // 出牌
    char played_card[256];
    // 根据规则决定出牌

    // 发送出牌信息给父进程
    write(write_fd, played_card, strlen(played_card) + 1);
}