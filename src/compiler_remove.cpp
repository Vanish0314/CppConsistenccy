int flag = 0;

// 示例 1: 冗余写入删除
void redundant_store()
{
    flag = 1; // 编译器认为这个写入是“冗余”的，因为紧接着就被覆盖了
    flag = 2; // 最终生效的值
}

/* Release 下生成的汇编代码:
 ?redundant_store@@YAXXZ PROC
     mov    DWORD PTR ?flag@@3HA, 2  ; 直接把 2 写入 flag，完全跳过了写入 1 的指令
     ret    0
 ?redundant_store@@YAXXZ ENDP

编译器发现 flag = 1 之后没有任何读取操作就直接被 flag = 2 覆盖了，因此认为第一步是徒劳的，直接将其删除。
*/

// 示例 2: 死代码删除
void dead_code()
{
    int local_var = 100;
    local_var = local_var * 2; // 这个计算虽然发生了，但结果从未被使用
    // 函数结束，计算结果被直接丢弃
}

/* Release 下生成的汇编代码:
 ?dead_code@@YAXXZ PROC
 ; Line 22
     ret    0    ; 什么都没做，直接返回
 ?dead_code@@YAXXZ ENDP

local_var 是一个局部变量，它的计算结果没有通过返回值、全局变量或函数调用传递出去。编译器判定这段代码对外部世界“毫无贡献”，因此整段逻辑被移除。
*/

int main()
{
    redundant_store();
    dead_code();
    return 0;
}

/*
这种优化在单线程中是完美的，但在多线程中可能导致严重的并发问题：

    假设一个线程（生产者）正在执行 redundant_store，而另一个线程（消费者）正在轮询 flag 的值：

        1 // 消费者线程
        2 while (flag != 1) {
        3     // 等待状态 1 的出现，以执行某些中间初始化逻辑
        4 }
    * 由于编译器的 Remove 优化，flag = 1 这个中间状态在机器码层面根本不存在
    * 消费者线程将永远阻塞在 while 循环中，或者直接跳过状态 1 看到状态 2。这种“状态丢失”会导致多线程状态机的逻辑崩溃

    可以查看 src/compiler_remove_concurrency.cpp 以了解这个问题的实际演示。
*/
