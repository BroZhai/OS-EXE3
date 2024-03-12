// lab 6A
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
  int	fd[2];	// for the pipe 创建 "读管道" 和 "写管道"
  char	buf[80], buf2[80]; //设定两个"输入字符缓存区"
  int	n;

  if (pipe(fd) < 0) { //使用pipe()函数创建管道，会返回一个int整数值
     printf("Pipe creation error\n");
     exit(1);
  }
  // 写个死循环，在管道中不停的读和写，直到遇到ctrl-d中断while循环
  while (1) {
        printf("Please input a line\n");
        // 当前整数n即为读入的文件(长度)
        n = read(STDIN_FILENO, buf, 80); // STDIN_FILENO --> (描述"输入文件"的一个值)

        //read(输入文件-->程序内部的某个变量或缓存, 读取输入文件的长度)
	if (n <= 0) break; 

	buf[--n] = 0; // 将换行符之前的字符 设置为 字符串结束符 (实现的逻辑就是：回车结束文件输入)
        printf("%d char in input line: [%s]\n", n, buf);

        write(fd[1], buf, n); // 将buf中的数据"写"(fd[1])到管道里面，n为写入长度(n即为上面的读入read长度)
        //write(pipe口<---源数据，写入长度)
        //输出写入到pipe中的提示
	printf("Input line [%s] written to pipe\n",buf);

        n = read(fd[0], buf2, 80);  //将管道的输出口（fd[0]）"对准了"buf2，并取得 管道输出"实际"的长度，并存到n里面
        //Tips: 这个输出的"实际长度"也即为buf2的大小，管道吐出了多少，buf2就接收多少，直到我们规定的上限(80)为止

	buf2[n] = 0; //将读出的文件 换行前的字符 设置为 字符串结束符(同上，遇到"回车符"换行)

        printf("%d char read from pipe: [%s]\n", n, buf2);
  }
  printf("bye bye\n");

  //打开管道后，请记得分别关闭打开的管道( 节 约 用 水 )
  close(fd[0]);
  close(fd[1]);
  exit(0);
}
