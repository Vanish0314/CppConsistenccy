/*
 * --------------------------------------------------------------------------------
 * Copyright (c) 2025 Vanishing Games. All Rights Reserved.
 * @Author: VanishXiao
 * @Date: 2026-03-17 11:43:30
 * @LastEditTime: 2026-03-17 10:47:15
 * --------------------------------------------------------------------------------
 */
#include <iostream>

int main() {
    int a = 1;
    int b = 2;

    int c = a + b;   // (1)
    int d = a * b;   // (2)

    std::cout << c << " " << d << std::endl;
}

/* Release 下生成的汇编代码:
Line 10: std::cout << c << " " << d << std::endl;
  mov    edx, 3    ; 直接把计算结果 3 (a+b) 放入寄存器
  lea    rcx, OFFSET FLAT:?cout@std@@3V?$basic_ostream@DU?$char_traits@D@std@@@1@A
  call   ??6...    ; 输出 3

  lea    rdx, OFFSET FLAT:??_C@_01CLKCMJKC@?5@ ; 加载空格 " "
  mov    rcx, rax
  call   ??$...    ; 输出空格

  mov    edx, 2    ; 直接把计算结果 2 (a*b) 放入寄存器
  mov    rcx, rax
  call   ??6...    ; 输出 2
*/