#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>// 定义fork、kill、alarm、pause、sleep等系统调用


void signal_handler(int signal_num) {// 父进程的信号处理函数：接收到SIGINT/Ctrl+C或SIGQUIT/Ctrl+\时执行
  // 注释掉的打印语句：用于调试，显示收到的信号编号
  // printf("Received interrupt signal: %d\n", signal_num);
}


void child_signal_handler(int signal_num) {// 子进程的信号处理函数：接收到16/17号自定义信号时执行终止逻辑
 
  if (signal_num == 16) { // 如果子进程收到16号信号
    printf("Child process 1 is killed by parent !!\n");
  }
  
  if (signal_num == 17) {// 如果子进程收到17号信号
    printf("Child process 2 is killed by parent !!\n");
  }
  exit(0);// 调用exit(0)终止当前子进程，0表示正常退出
}


int main() {
  pid_t child1, child2;// 子进程1和子进程2的PID
  signal(SIGINT, signal_handler);   // 父进程收到SIGINT（Ctrl+C）信号时，执行signal_handler函数
  signal(SIGQUIT, signal_handler);  // 父进程收到SIGQUIT（Ctrl+\）信号时，执行signal_handler函数

 
  // fork()返回值：父进程中返回子进程的PID（大于0），子进程中返回0，失败返回-1
  child1 = fork(); // 创建子进程1
  
  if (child1 == 0) {// 子进程1成功
    // 子进程1注册信号处理规则：收到16号信号时，执行child_signal_handler函数
    signal(16, child_signal_handler); 
    // 子进程1进入无限循环，一直等待信号（不做任何工作，直到收到终止信号）
    while (1) {
    }
  }

  
  child2 = fork();// 创建子进程2
  if (child2 == 0) {
    signal(17, child_signal_handler); 
    while (1) {
    }
  }

  // 父进程的执行逻辑：只有父进程中child1和child2都大于0（两个子进程都创建成功）
  if (child1 > 0 && child2 > 0) {
    alarm(5);// 调用alarm(5)设置5秒超时：5秒后父进程会收到SIGALRM（闹钟）信号，（ 14 号闹钟信号）

    pause();// 调用pause()让父进程暂停执行：直到收到任意一个信号后，才会继续执行后续代码
    // 触发条件：1.5秒超时收到SIGALRM；2.手动按Ctrl+C收到SIGINT；3.手动按Ctrl+\收到SIGQUIT
    kill(child1, 16);
    kill(child2, 17);

    wait(NULL); // 调用wait(NULL)等待任意一个子进程退出，防止子进程变成“僵尸进程” 
    wait(NULL); // 再次调用wait(NULL)等待另一个子进程退出 

    printf("Parent process is killed!!\n");// 两个子进程都退出
  }

  return 0;
}