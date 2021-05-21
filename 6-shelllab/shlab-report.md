<h1 style="text-align: center; font-weight: 700">shlab-report</h1>

[toc]

刘慕梵 19307130248

## 一、结果截图：

左边是tsh，右边是tshref。

![image-20210425093857144](C:\Users\lalala\AppData\Roaming\Typora\typora-user-images\image-20210425093857144.png)

![image-20210425093922490](C:\Users\lalala\AppData\Roaming\Typora\typora-user-images\image-20210425093922490.png)

![image-20210425093945308](C:\Users\lalala\AppData\Roaming\Typora\typora-user-images\image-20210425093945308.png)

![image-20210425104255809](C:\Users\lalala\AppData\Roaming\Typora\typora-user-images\image-20210425104255809.png)

![image-20210425094027596](C:\Users\lalala\AppData\Roaming\Typora\typora-user-images\image-20210425094027596.png)

![image-20210425094046901](C:\Users\lalala\AppData\Roaming\Typora\typora-user-images\image-20210425094046901.png)

![image-20210425094114661](C:\Users\lalala\AppData\Roaming\Typora\typora-user-images\image-20210425094114661.png)

## 二、代码及注释：

### 1、eval

```c
void eval(char *cmdline) 
{
    char* argv[MAXLINE];
    char buf[MAXLINE];
    int bg;
    pid_t pid;

    sigset_t mask_all, mask_one, prev;
    if(sigfillset(&mask_all) < 0)//设置阻塞集合,mask_all阻塞所有信号
        unix_error("sigset error");
    if(sigemptyset(&mask_one) < 0)
        unix_error("sigset error");
    if(sigaddset(&mask_one, SIGCHLD) < 0)//mask_one阻塞SIGCHLD信号
        unix_error("sigset error");

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);//得到argv，并判断是否是后台作业，得到bg
    if(argv[0] == NULL)//忽略空命令
        return;

    if(!builtin_cmd(argv))//执行内部命令，如果非内部命令执行以下代码
    {
        if(sigprocmask(SIG_BLOCK, &mask_one, &prev) < 0)//阻塞SIGCHLD信号，防止addjob时出现不一致
            unix_error("sigset error");
        if((pid = fork()) < 0)
            unix_error("fork error");
        if(pid == 0)//子进程
        {
            if(sigprocmask(SIG_SETMASK, &prev, NULL) < 0)//解除子进程对SIGCHLD的阻塞
                unix_error("sigset error");
            if(execve(argv[0], argv, environ) < 0)//调用可执行程序
            {
                printf("%s: Command not found\n",argv[0]);//出错
                exit(0);//子进程退出结束
            }
        }
        //主进程
        if(sigprocmask(SIG_BLOCK, &mask_all, NULL) < 0)//阻塞所有信号
            unix_error("sigset error");
        if(setpgid(pid, 0) < 0)//将该进程单独添加到一个进程组
            unix_error("setpid group error");
        addjob(jobs, pid, bg + 1, buf);//添加到工作集
        if(sigprocmask(SIG_SETMASK, &prev, NULL) < 0)//解除阻塞
            unix_error("sigset error");

        if(!bg)//前台作业，显式等待作业结束
            waitfg(pid);
        else
            printf("[%d] (%d) %s",pid2jid(pid), pid, buf);//后台作业，打印相关信息
    }
    return;
}
```

### 2、builtin cmd

```c
int builtin_cmd(char **argv) 
{
    sigset_t mask, prev;
    if(sigemptyset(&mask) < 0)//设置阻塞集合
        unix_error("sigset error");
    if(sigaddset(&mask, SIGCHLD) < 0)//设置阻塞集合
        unix_error("sigset error");
    if(!strcmp(argv[0], "quit"))//quit command
    {
        for(int i = 0; i < MAXJOBS; i++)
            if(jobs[i].pid)
                kill(-jobs[i].pid, SIGKILL);//结束所有子进程
        exit(0);//退出shell
    }
    if(!strcmp(argv[0], "jobs"))//jobs command
    {
        if(sigprocmask(SIG_BLOCK, &mask, &prev) < 0)//在调用listjobs时阻塞SIGCHLD信号,防止调用过程中接收到SIGCHLD信号而发生不一致
            unix_error("sigset error");
        listjobs(jobs);
        if(sigprocmask(SIG_SETMASK, &prev, NULL) < 0)
            unix_error("sigset error");
        return 1;
    }
    if(!strcmp(argv[0], "fg") || !strcmp(argv[0], "bg"))//fg bg command
    {
        do_bgfg(argv);//执行处理bg、fg的函数
        return 1;
    }
    return 0;     /* not a builtin command */
}
```

### 3、do_bgfg

```c
void do_bgfg(char **argv) 
{
    pid_t pid = 0;//fg或bg作用的pid
    struct job_t *job;//相应pid的job
    sigset_t mask, prev;
    if(argv[1] == NULL)//如果第二个参数为空，输出提示
    {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }
    if(argv[1][0] == '%')//如果是jid
    {
        int jid = atoi(argv[1] + 1);//得到jid
        if(jid == 0)//如果参数有错误，打印信息
        {
            printf("%s: argument must be a PID or %%jobid\n",argv[0]);
            return;
        }
        job = getjobjid(jobs, jid);//得到相应的job
        if(job == NULL)//jid不存在
        {
            printf("%s: No such job\n", argv[1]);
            return;
        }
        pid = job->pid;
    }
    else
    {
        pid = atoi(argv[1]);//得到pid
        if(pid == 0)//如果参数有错误，打印信息
        {
            printf("%s: argument must be a PID or %%jobid\n",argv[0]);
            return;
        }
        job = getjobpid(jobs, pid);//得到相应的job
        if(job == NULL)//pid不存在
        {
            printf("(%s): No such process\n", argv[1]);
            return;
        }
    }
    if(sigemptyset(&mask) < 0)//设置阻塞集合
        unix_error("sigset error");
    if(sigaddset(&mask, SIGCHLD) < 0)//设置阻塞集合
        unix_error("sigset error");
    if(sigprocmask(SIG_BLOCK, &mask, &prev) < 0)//在发送SIGCONT时阻塞SIGCHLD信号,防止调用过程中接收到SIGCHLD信号而发生不一致
        unix_error("sigset error");
    if(kill(-pid, SIGCONT) < 0)//发送SIGCONT信号
        unix_error("kill error");
    if(sigprocmask(SIG_SETMASK, &prev, NULL) < 0)
        unix_error("sigset error");
    if(!strcmp(argv[0], "bg"))//如果是bg命令
    {
        job->state = BG;//修改状态为BG
        printf("[%d] (%d) %s",job->jid, pid, job->cmdline);//打印相应信息
    }
    else//fg命令
    {
        job->state = FG;//修改状态为fg
        waitfg(pid);//等待运行结束
    }
    return;
}
```

### 4、waitfg

```c
void waitfg(pid_t pid)
{
    sigset_t mask, prev;

    if(sigemptyset(&mask) < 0)//设置mask
        unix_error("sigset error");
    if(sigaddset(&mask, SIGCHLD) < 0)
        unix_error("sigset error");
    if(sigprocmask(SIG_BLOCK, &mask, &prev) < 0)//阻塞SIGCHLD信号
        unix_error("sigset error");

    while(fgpid(jobs) == pid)//循环等待前台进程结束
        sigsuspend(&prev);

    if(sigprocmask(SIG_SETMASK, &prev, NULL) < 0)//还原调用前的阻塞集合
        unix_error("sigset error");

    return;
}
```

### 5、sigchld_handler

```c
void sigchld_handler(int sig) 
{
    int olderrno = errno;//保存之前的错误信息
    sigset_t mask, prev;
    int status;
    pid_t pid;

    if(sigfillset(&mask) < 0)//设置阻塞集合
        unix_error("sigset error");
    
    while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)//循环检查所有终止或停止的子进程
    {
        if(sigprocmask(SIG_BLOCK, &mask, &prev) < 0)//阻塞所有信号
            unix_error("sigset error");
        if(WIFEXITED(status))//子进程正常结束退出，删除其job
            deletejob(jobs, pid);
        else if(WIFSIGNALED(status))//因为未捕获的信号结束，打印信息并删除job
        {
            printf("Job [%d] (%d) terminated by signal %d\n",pid2jid(pid), pid, WTERMSIG(status));
            deletejob(jobs, pid);
        }
        else if(WIFSTOPPED(status))//子进程停止
        {
            printf("Job [%d] (%d) stopped by signal %d\n",pid2jid(pid), pid, WSTOPSIG(status));
            struct job_t* job = getjobpid(jobs, pid);
            job->state = ST;
        }
        if(sigprocmask(SIG_SETMASK, &prev, NULL) < 0)//恢复阻塞集合
            unix_error("sigset error");
    }
    if(errno != olderrno && errno != ECHILD)//检查waitpid返回负值的原因是不是因为没有终止或停止子进程了
        unix_error("waitpid error");

    errno = olderrno;//恢复之前的错误信息
    return;
}
```

### 6、sigint_handler

```c
void sigint_handler(int sig) 
{
    int olderrno = errno;//保存之前的错误信息
    sigset_t mask, prev;
    pid_t pid = fgpid(jobs);//找到前台进程

    if(pid > 0)//如果存在前台作业
    {
        if(sigemptyset(&mask) < 0)//设置mask
            unix_error("sigset error");
        if(sigaddset(&mask, SIGCHLD) < 0)
            unix_error("sigset error");
        if(sigprocmask(SIG_BLOCK, &mask, &prev) < 0)//阻塞SIGCHLD信号
            unix_error("sigset error");
        if(kill(-pid, SIGINT) < 0)//发送SIGINT信号
            unix_error("kill error");
        if(sigprocmask(SIG_SETMASK, &prev, NULL) < 0)//恢复阻塞集合
            unix_error("sigset error");
    }
    
    errno = olderrno;//恢复之前的错误信息
    return;
}
```

### 7、sigtstp_handler

```c
void sigtstp_handler(int sig) 
{
    int olderrno = errno;//保存之前的错误信息
    sigset_t mask, prev;
    pid_t pid = fgpid(jobs);//找到前台进程

    if(pid > 0)//如果存在前台作业
    {
        if(sigemptyset(&mask) < 0)//设置mask
            unix_error("sigset error");
        if(sigaddset(&mask, SIGCHLD) < 0)
            unix_error("sigset error");
        if(sigprocmask(SIG_BLOCK, &mask, &prev) < 0)//阻塞SIGCHLD信号
            unix_error("sigset error");
        if(kill(-pid, SIGTSTP) < 0)//发送SIGTSTP信号
            unix_error("kill error");
        if(sigprocmask(SIG_SETMASK, &prev, NULL) < 0)//恢复阻塞集合
            unix_error("sigset error");
    }

    errno = olderrno;//恢复之前的错误信息
    return;
}
```