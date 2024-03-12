// lab 6B 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
   //初始化各种管道信息以及缓存区
  int	fd[2];	
  char	buf[80], buf2[80];
  int	even, n;

  if (pipe(fd) < 0) {
     printf("Pipe creation error\n");
     exit(1);
  }
  even = 1;
  // 死循环，不停地要用户写和自己读再进行相关的操作
  while (1) {
        even = 1 - even; //小技巧: 偶数(输入的第二项)"开关" (标识符)

        if (even) //if的判断条件: 1进(偶,even在上面等于0或其他偶数)，0不进(奇，even在上面为1或其他奇数)
           printf("Please input an even line\n");
        else 
           printf("Please input an odd line\n");
           
         
        n = read(STDIN_FILENO, buf, 80); //读入文件
        if (n <= 0) break; // 读入异常
        buf[--n] = 0; // 设置"回车换行符"

        printf("%d char in input line: [%s]\n", n, buf);

        write(fd[1], buf, n); //将buf中的内容写到管道的写入口(fd[1])
        printf("Input line [%s] written to pipe\n",buf);

        //这里不同的是，在奇数的情况下管道的出口是"封死的"，就会被暂留在管道中
        //直到偶数时管道才"被打开"，先前的数据和偶数输入的数据一起被输出

        if (even) { // 偶数状态下,even的值应该为1,因此此时会进入这里的if分支
           n = read(fd[0], buf2, 80); //将"目前"管道中的所有数据(包括之前 奇数输入的值 和 当前偶数来的值)输出到buf2
           buf2[n] = 0; // 设置"回车换行符"
           printf("%d char read from pipe: [%s]\n", n, buf2);
        }
  }
  printf("bye bye\n");
  close(fd[0]);
  close(fd[1]);
  exit(0);
}
