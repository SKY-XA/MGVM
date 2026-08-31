#pragma once
#include "mg_vm.h"
#include "mg_opcode.h"
#include <iostream>
/*
 * MGVM Implementation
 * Copyright © 2026 SKY-XA
 * Underlying runtime implementation for self‑developed ML (MemLock) & Lava languages.
 * Full license terms: see repository root LICENSE / LICENSE‑CH.
 * Note: Official full bundled interpreters of ML & Lava are copyrighted.
 */


#define VM MGVM* vm
#define INS Instruction* inst = (Instruction*)param

// ==================== 基础指令 ====================
inline void handler_push(VM, void* param) {
    INS;
    vm->stack[++vm->sp] = inst->num;
}



inline void handler_load_var(VM, void* param) {
    INS;
    vm->pushStack(vm->getVar(inst->str1));
}
inline void handler_store_var(VM, void* param) {
    INS;

    // 空栈 直接彻底跳过，什么都不干
    if(vm->sp <= 0) {
        return;
    }

    // 只有真有数据才pop+赋值
    long long val = vm->popStack();
    vm->setVar(inst->str1, val);
}
inline void handler_print(VM, void* param) {
    INS;
    int poolIdx = static_cast<int>(inst->num);
    // poolIdx == -1：打印栈上数字
    if(poolIdx == -1)
    {
        long long val=0;
        if(vm->sp > 0)
        {
            val = vm->popStack(); // 注意：这里要pop！原版bug只读取没有弹出栈
        }
        printf("%lld\n", val);
    }
    else
    {
        // 从虚拟机的字符串常量池取出字符串输出
        if(poolIdx >= 0 && poolIdx < (int)vm->stringPool.size())
        {
            const std::string& s = vm->stringPool[poolIdx];
            printf("%s\n", s.c_str());
        }
        else
        {
            printf("[print: invalid string pool index %d]\n", poolIdx);
        }
    }
}

inline void handler_input(VM, void* param) {
    INS;
    long long value = 0;
    scanf("%lld", &value);    // 从控制台读整数
    vm->pushStack(value);     // 压入栈顶
}


// ==================== MG专属指令 ====================
inline void handler_inc_to(VM, void* param) {
    INS;
    long long target = vm->popStack();
    long long val = vm->getVar(inst->str1);
    // 循环自增
    while(val < target) val++;
    vm->setVar(inst->str1, val);
    vm->pushStack(val);
}

inline void handler_dec_to(VM, void* param) {
    INS;
    long long target = vm->popStack();
    long long val = vm->getVar(inst->str1);
    // 循环自减
    while(val > target) val--;
    vm->setVar(inst->str1, val);
    vm->pushStack(val);
}

// ==================== 核心运算指令 ====================
inline void handler_mov(VM, void* param) {
    INS;
    long long val = (inst->str2.empty()) ? inst->num : vm->getVar(inst->str2);
    vm->setVar(inst->str1, val);
}

inline void handler_pop(VM, void* param) {
    INS;
    long long val = vm->popStack();
    if (!inst->str1.empty()) vm->setVar(inst->str1, val);
}

inline void handler_add(VM, void* param) {
    INS;
    long long b=vm->popStack();
    vm->pushStack(vm->popStack()+b);
}
inline void handler_sub(VM, void* param) {
    INS;
    long long b=vm->popStack();
    long long a=vm->popStack();
    vm->pushStack(a-b);
}
inline void handler_mul(VM, void* param) {
    INS;
    long long b=vm->popStack();
    long long a=vm->popStack();
    vm->pushStack(a*b);
}
inline void handler_div(VM, void* param) {
    INS;
    long long b=vm->popStack();
    vm->pushStack(vm->popStack()/b);
}
inline void handler_mod(VM, void* param) {
    INS;
    long long b=vm->popStack();
    vm->pushStack(vm->popStack()%b);
}

inline void handler_inc(VM, void* param) {
    INS;
    long long v=vm->getVar(inst->str1);
    vm->setVar(inst->str1, v+1);
}
inline void handler_dec(VM, void* param) {
    INS;
    long long v=vm->getVar(inst->str1);
    vm->setVar(inst->str1, v-1);
}

// ==================== 位运算指令 ====================
inline void handler_and(VM, void* param) {
    INS;
    long long b=vm->popStack();
    vm->pushStack(vm->popStack()&b);
}
inline void handler_or(VM, void* param)  {
    INS;
    long long b=vm->popStack();
    vm->pushStack(vm->popStack()|b);
}
inline void handler_xor(VM, void* param) {
    INS;
    long long b=vm->popStack();
    vm->pushStack(vm->popStack()^b);
}
inline void handler_not(VM, void* param) {
    INS;
    vm->pushStack(~vm->popStack());
}

inline void handler_shl(VM, void* param) {
    INS;
    long long b=vm->popStack();
    vm->pushStack(vm->popStack()<<b);
}
inline void handler_shr(VM, void* param) {
    INS;
    long long b=vm->popStack();
    vm->pushStack(vm->popStack()>>b);
}

// ==================== 跳转指令 ====================
inline void handler_jmp(VM, void* param)  {
    INS;
    vm->setPC(inst->num);
}
inline void handler_jz(VM, void* param)   {
    INS;
    if (vm->popStack() == 0) vm->setPC(inst->num);
}
inline void handler_jnz(VM, void* param)  {
    INS;
    if (vm->popStack() != 0) vm->setPC(inst->num);
}
inline void handler_jg(VM, void* param) {
    INS;
    long long right = vm->popStack();
    long long left = vm->popStack();
    if (left > right) {
        vm->setPC(inst->num);
    }
}
inline void handler_jl(VM, void* param)   {
    INS;
    long long right = vm->popStack();
    long long left = vm->popStack();
    if (left < right) {
        vm->setPC(inst->num);
    }
}
inline void handler_jle(VM, void* param)  {
    INS;
    long long right = vm->popStack();
    long long left = vm->popStack();
    if (left <= right) {
        vm->setPC(inst->num);
    }
}
inline void handler_jge(VM, void* param)  {
    INS;
    long long right = vm->popStack();
    long long left = vm->popStack();
    if (left >= right) {
        vm->setPC(inst->num);
    }
}


inline void handler_call(VM, void* param) {
    INS;
    vm->pushStack(vm->getPC());
    vm->setPC(inst->num);
}
inline void handler_ret(VM, void* param)  {
    INS;
    vm->setPC(vm->popStack());
}

// ==================== 辅助指令 ====================
inline void handler_neg(VM, void* param)  {
    INS;
    vm->pushStack(-vm->popStack());
}
inline void handler_abs(VM, void* param)  {
    INS;
    long long v=vm->popStack();
    vm->pushStack(v<0?-v:v);
}
inline void handler_dup(VM, void* param)  {
    INS;
    long long v=vm->popStack();
    vm->pushStack(v);
    vm->pushStack(v);
}
inline void handler_swap(VM, void* param) {
    INS;
    long long a=vm->popStack(), b=vm->popStack();
    vm->pushStack(a);
    vm->pushStack(b);
}
inline void handler_nop(VM, void* param)  {}
inline void handler_halt(VM, void* param) {
    vm->stop();
}

// 别名指令
inline void handler_je(VM, void* param)   {
    handler_jz(vm, param);
}
inline void handler_jne(VM, void* param)  {
    handler_jnz(vm, param);
}

// ==================== 指令注册 ====================
inline void registerAllInstructions(InstructionHandler handlers[]) {
    handlers[op_push]      = handler_push;
    handlers[op_load_var]  = handler_load_var;
    handlers[op_store_var] = handler_store_var;
    handlers[op_print]     = handler_print;
    handlers[op_inc_to]    = handler_inc_to;
    handlers[op_dec_to]    = handler_dec_to;
    handlers[op_mov]       = handler_mov;
    handlers[op_pop]       = handler_pop;
    handlers[op_add]       = handler_add;
    handlers[op_sub]       = handler_sub;
    handlers[op_mul]       = handler_mul;
    handlers[op_div]       = handler_div;
    handlers[op_mod]       = handler_mod;
    handlers[op_inc]       = handler_inc;
    handlers[op_dec]       = handler_dec;
    handlers[op_and]       = handler_and;
    handlers[op_or]        = handler_or;
    handlers[op_xor]       = handler_xor;
    handlers[op_not]       = handler_not;
    handlers[op_shl]       = handler_shl;
    handlers[op_shr]       = handler_shr;
    handlers[op_jmp]       = handler_jmp;
    handlers[op_jz]        = handler_jz;
    handlers[op_jnz]       = handler_jnz;
    handlers[op_jg]        = handler_jg;
    handlers[op_jl]        = handler_jl;
    handlers[op_call]      = handler_call;
    handlers[op_ret]       = handler_ret;
    handlers[op_neg]       = handler_neg;
    handlers[op_abs]       = handler_abs;
    handlers[op_dup]       = handler_dup;
    handlers[op_swap]      = handler_swap;
    handlers[op_nop]       = handler_nop;
    handlers[op_halt]      = handler_halt;
    handlers[op_je]        = handler_je;
    handlers[op_jne]       = handler_jne;
    handlers[op_jle]       = handler_jle;
    handlers[op_jge]       = handler_jge;
    handlers[op_input]	 = handler_input;
}

#undef VM
#undef INS
// </> mg_instructions_impl.h
