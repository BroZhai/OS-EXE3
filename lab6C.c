// lab 6C
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main()
{
   //《进程间的加密通信XD》，除了两个"关死"的读写口外，剩下的两个要"选择性的开和关"
  char	mapl[] = "qwertyuiopasdfghjklzxcvbnm"; // for encoding letter
  char	mapd[] = "1357924680"; // for encoding digit
  int	fd[2];	// for the pipe
  char	buf[80];
  int	i, n, childid;

  if (pipe(fd) < 0) {
     printf("Pipe creation error\n");
     exit(1);
  }
  childid = fork(); //创建子进程

  if (childid < 0) {
     printf("Fork failed\n");
     exit(1);
  } else if (childid == 0) { //子进程的操作
     close(fd[1]); // 关掉子进程的"写口"
     while ((n = read(fd[0],buf,80)) > 0) { // 从管道中读取数据，此时子进程"读口"还是打开的状态
           buf[n] = 0; 
           printf("<child> message [%s] of size %d bytes received\n",buf,n);
     }
     close(fd[0]);//子进程读取管道数据已完成，关掉子进程的"读口"
     printf("<child> I have completed!\n");

  } else { //父进程操作
     close(fd[0]); //关闭父进程的"读口"，只剩下他的"写口"(与之对应的，管道中剩下子进程的"读口")
     //这俩进程"建立了通信"
     while (1) {
           printf("<parent> please enter a message\n");
           n = read(STDIN_FILENO,buf,80); // 和前面一样的读文件操作
           if (n <= 0) break; 
           buf[--n] = 0;
           printf("<parent> message [%s] is of length %d\n",buf,n);
           for (i = 0; i < n; i++) // 加密，可略过
               if (buf[i] >= 'a' && buf[i] <= 'z')
                  buf[i] = mapl[buf[i]-'a'];
               else if (buf[i] >= 'A' && buf[i] <= 'Z')
                  buf[i] = mapl[buf[i]-'A']-('a'-'A');
               else if (buf[i] >= '0' && buf[i] <= '9')
                  buf[i] = mapd[buf[i]-'0'];
           printf("<parent> sending encrypted message [%s] to child\n",buf);
           write(fd[1],buf,n); // 将缓存buf中的处理过的"加密字符"推到"写口"里面去
     }
     close(fd[1]);//关闭父亲的写口
     wait(NULL);
     printf("<parent> I have completed!\n");
  }
  exit(0);
}
