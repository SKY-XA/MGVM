#ifndef MG_OPCODE_H
#define MG_OPCODE_H
/*
 * MGVM Implementation
 * Copyright © 2026 SKY-XA
 * Underlying runtime implementation for self‑developed ML (MemLock) & Lava languages.
 * Full license terms: see repository root LICENSE / LICENSE‑CH.
 * Note: Official full bundled interpreters of ML & Lava are copyrighted.
 */

#include <string>
#include <unordered_map>

// 前向声明
class MGVM;

// 指令处理函数指针
typedef void (*InstructionHandler)(MGVM* vm, void* param);

// 指令枚举
enum OpCode {
    op_invalid = 0,
    // 原有兼容指令
    op_push,
    op_load_var,
    op_store_var,
    op_print,
    // MG专属指令
    op_inc_to,
    op_dec_to,
    // 通用汇编核心指令
    op_mov,
    op_pop,
    op_add,
    op_sub,
    op_mul,
    op_div,
    op_mod,
    op_inc,
    op_dec,
    op_and,
    op_or,
    op_xor,
    op_not,
    op_shl,
    op_shr,
    op_jmp,
    op_jz,
    op_jnz,
    op_jg,
    op_jl,
    op_call,
    op_ret,
    // 新增常用汇编指令
    op_neg,
    op_abs,
    op_dup,
    op_swap,
    op_nop,
    op_halt,
    op_je,
    op_jne,
    op_jle,
    op_jge,
    op_input,

    // 【极限优化必需】数组大小标记，不影响原有逻辑
    OP_COUNT
};

// 指令字符串映射表
extern const std::unordered_map<std::string, OpCode> OPCODE_MAP;

// 汇编指令结构体
struct Instruction {
    OpCode op = op_invalid;
    long long num = 0;       // 数字参数
    std::string str1 = "";   // 第一个字符串参数
    std::string str2 = "";   // 第二个字符串参数
    int line = 0;            // 行号（报错用）
};

#endif // MG_OPCODE_H
//>_ mg_opcode.h

