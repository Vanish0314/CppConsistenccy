int A = 0, B = 0;
void foo()
{
    A = B + 1;  //(1)
    B = 1;      //(2) 
}

int main()
{
    foo();
    return 0;
}

/* Release 下生成的汇编代码:
 ?foo@@YAXXZ PROC    ; foo 函数开始
 ; Line 12: A = B + 1;
     mov    eax, DWORD PTR ?B@@3HA    ; 将 B 的值读入 eax
     inc    eax                       ; eax = eax + 1 (即 B + 1)
 
 ; Line 13: B = 1;
     mov    DWORD PTR ?B@@3HA, 1      ; 把 1 写入 B (!!! 注意这里)
     mov    DWORD PTR ?A@@3HA, eax    ; 把 eax 的值写入 A
     ret    0
 ?foo@@YAXXZ ENDP

逻辑顺序：
    * 在 C++ 源码中，先给 A 赋值，再给 B 赋值
    * 但在汇编中，编译器执行了 mov DWORD PTR ?B@@3HA, 1（给 B 赋值），然后才执行 mov DWORD PTR ?A@@3HA, eax（给 A 赋值）
 */