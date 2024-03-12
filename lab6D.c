// lab 6D
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main()
{
  char	mapl[] = "qwertyuiopasdfghjklzxcvbnm"; // for encoding letter
  char	mapd[] = "1357924680"; // for encoding digit
  int	fd[2];	// for the pipe
  char	buf[80], buf2[80];
  int	i, j, n, childid;

  if (pipe(fd) < 0) {
     printf("Pipe creation error\n");
     exit(1);
  }
  childid = fork();
  if (childid < 0) {
     printf("Fork failed\n");
     exit(1);
  } else if (childid == 0) { // child
     // close(fd[1]); child forgets to close out
     while ((n = read(fd[0],buf,80)) > 0) { // read from pipe
           buf[n] = 0;
           printf("<child> message [%s] of size %d bytes received\n",buf,n);
           sleep(3); // add this line to delay child
           for (i = 0, j = 0; i < n; i = i+2, j++) // skip the odd characters
	       buf2[j] = buf[i];
           write(fd[1],buf2,j); // accidentally echo back new string via fd[1]
     }
     close(fd[0]);
     printf("<child> I have completed!\n");
  } else { // parent
     close(fd[0]); // close parent in
     while (1) {
           printf("<parent> please enter a message\n");
           n = read(STDIN_FILENO,buf,80); // read a line
           if (n <= 0) break; // EOF or error
           buf[--n] = 0;
           printf("<parent> message [%s] is of length %d\n",buf,n);
           for (i = 0; i < n; i++) // encrypt
               if (buf[i] >= 'a' && buf[i] <= 'z')
                  buf[i] = mapl[buf[i]-'a'];
               else if (buf[i] >= 'A' && buf[i] <= 'Z')
                  buf[i] = mapl[buf[i]-'A']-('a'-'A');
               else if (buf[i] >= '0' && buf[i] <= '9')
                  buf[i] = mapd[buf[i]-'0'];
           printf("<parent> sending encrypted message [%s] to child\n",buf);
           write(fd[1],buf,n); // send the encrypted string
     }
     close(fd[1]);
     wait(NULL);
     printf("<parent> I have completed!\n");
  }
  exit(0);
}
