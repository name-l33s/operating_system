# 操作系统课程设计实验

## 项目概述

本项目包含7个操作系统实验，全部使用C语言实现，兼容macOS环境。

## 实验列表

### 实验1：作业调度
- **目录**: `实验1_作业调度/`
- **程序**: `job_scheduling.c`
- **功能**:
  - 先来先服务(FCFS)
  - 短作业优先(SJF)
  - 响应比高者优先(HRRN)
  - 双作业并发模拟
- **编译**: `gcc -o job_scheduling job_scheduling.c`
- **运行**: `./job_scheduling`

### 实验2：磁盘调度
- **目录**: `实验2_磁盘调度/`
- **程序**: `disk_scheduling.c`
- **功能**:
  - 先来先服务(FCFS)
  - 最短寻道优先(SSTF)
  - 电梯调度(SCAN)
- **编译**: `gcc -o disk_scheduling disk_scheduling.c`
- **运行**: `./disk_scheduling`

### 实验3：文件系统调用
- **目录**: `实验3_文件系统调用/`
- **程序**: `filetools.c`
- **功能**:
  - 创建文件
  - 写入文件(可变记录)
  - 读取文件
  - 修改文件权限
  - 查看文件权限
- **系统调用**: open, close, read, write, chmod, execv
- **编译**: `gcc -o filetools filetools.c`
- **运行**: `./filetools`

### 实验4：进程管理
- **目录**: `实验4_进程管理/`
- **程序**: `process_management.c`
- **功能**:
  - 父子进程创建(fork)
  - 管道通信(pipe)
  - 信号处理(signal/kill)
- **编译**: `gcc -o process_management process_management.c`
- **运行**: `./process_management`
- **测试**: Ctrl+C 触发信号

### 实验5：进程通信
- **目录**: `实验5_进程通信/`
- **程序**: `process_communication.c`
- **功能**:
  - 管道通信
  - 消息队列通信(演示)
  - 共享内存通信(使用POSIX共享内存)
- **说明**: macOS上System V IPC与Linux有差异
- **编译**: `gcc -o process_communication process_communication.c`
- **运行**: `./process_communication`

### 实验6：多级反馈队列调度
- **目录**: `实验6_多级反馈队列调度/`
- **程序**: `mlfq.c`
- **功能**:
  - 3级队列(Q1/Q2/Q3)
  - 时间片4/8/16
  - 进程降级机制
  - 紧急进程抢占
  - 优先级提升(32时间单位)
- **编译**: `gcc -o mlfq mlfq.c`
- **运行**: `./mlfq`

### 实验7：读者-写者问题
- **目录**: `实验7_读者写者问题/`
- **程序**: `reader_writer.c`
- **功能**:
  - 信号量同步(sem_wait/sem_post)
  - 读-读并发
  - 读-写/写-写互斥
  - 避免写者饥饿(3次延迟后写优先)
  - 平均等待时间统计
- **编译**: `gcc -o reader_writer reader_writer.c -lpthread`
- **运行**: `./reader_writer`

## 目录结构

```
操作系统课设/
├── README.md
├── 实验1_作业调度/
│   ├── job_scheduling.c
│   ├── job_scheduling
│   └── 实验1_实验报告.md
├── 实验2_磁盘调度/
│   ├── disk_scheduling.c
│   ├── disk_scheduling
│   └── 实验2_实验报告.md
├── 实验3_文件系统调用/
│   ├── filetools.c
│   ├── filetools
│   └── 实验3_实验报告.md
├── 实验4_进程管理/
│   ├── process_management.c
│   ├── process_management
│   └── 实验4_实验报告.md
├── 实验5_进程通信/
│   ├── process_communication.c
│   ├── process_communication
│   └── 实验5_实验报告.md
├── 实验6_多级反馈队列调度/
│   ├── mlfq.c
│   ├── mlfq
│   └── 实验6_实验报告.md
└── 实验7_读者写者问题/
    ├── reader_writer.c
    ├── reader_writer
    └── 实验7_实验报告.md
```

## 环境要求

- 操作系统: macOS/Linux
- 编译器: gcc
- 线程库: pthread (实验7)

## 编译所有实验

在每个实验目录下运行相应的编译命令，或参考上面的实验列表。

## 注意事项

1. **macOS差异**: 
   - System V IPC (msgget, semget, shmget) 与Linux有差异
   - sem_init在macOS上标记为deprecated但仍可使用
   
2. **实验7**:
   - 需要链接pthread库: `-lpthread`

3. **实验3**:
   - 使用系统调用而非C库I/O函数

4. **实验报告**:
   - 每个实验目录下都有详细的实验报告(Markdown格式)

## 作者

操作系统课程设计
