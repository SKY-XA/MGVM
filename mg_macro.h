#ifndef MG_MACRO_H
#define MG_MACRO_H
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include "mg_opcode.h"
/*
 * MGVM Implementation
 * Copyright © 2026 SKY-XA
 * Underlying runtime implementation for self‑developed ML (MemLock) & Lava languages.
 * Full license terms: see repository root LICENSE / LICENSE‑CH.
 * Note: Official full bundled interpreters of ML & Lava are copyrighted.
 */
namespace mg_macro {
using MacroExpandResult = std::vector<std::string>;
using MacroHandler = MacroExpandResult (*)(const std::vector<std::string>& args);

// ==================== 复合指令实现 ====================
inline MacroExpandResult macro_mov_add(const std::vector<std::string>& args) {
    MacroExpandResult out;
    std::string a = args[0], b = args[1], c = args[2];
    out.push_back("mov " + a + ", " + b);
    out.push_back("load_var " + a);
    out.push_back("push " + c);
    out.push_back("add");
    out.push_back("pop " + a);
    return out;
}

inline MacroExpandResult macro_loop(const std::vector<std::string>& args) {
    MacroExpandResult out;
    std::string var = args[0], target = args[1];
    out.push_back("push " + target);
    out.push_back("inc_to " + var);
    return out;
}

inline MacroExpandResult macro_print_str(const std::vector<std::string>& args) {
    MacroExpandResult out;
    // 注意，这套虚拟机目前对于字符串的解析方式与正常的有些区别
    // 当虚拟机匹配到print后面的第一个双引号后，一直到行尾都会被认定成字符串
    // 不需要配一对引号
    out.push_back("print \"" + args[0] /*+ "\""*/);
    return out;
}

inline MacroExpandResult macro_swap_var(const std::vector<std::string>& args) {
    MacroExpandResult out;
    std::string x = args[0], y = args[1];
    out.push_back("load_var " + x);
    out.push_back("load_var " + y);
    out.push_back("swap");
    out.push_back("pop " + x);
    out.push_back("pop " + y);
    return out;
}
// ==================== 【新增：算术运算系列】 ====================
inline MacroExpandResult macro_mov_sub(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto dst = args[0], a = args[1], b = args[2];
    out.push_back("mov " + dst + ", " + a);
    out.push_back("load_var " + dst);
    out.push_back("push " + b);
    out.push_back("sub");
    out.push_back("pop " + dst);
    return out;
}

inline MacroExpandResult macro_mov_mul(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto dst = args[0], a = args[1], b = args[2];
    out.push_back("mov " + dst + ", " + a);
    out.push_back("load_var " + dst);
    out.push_back("push " + b);
    out.push_back("mul");
    out.push_back("pop " + dst);
    return out;
}

inline MacroExpandResult macro_mov_div(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto dst = args[0], a = args[1], b = args[2];
    out.push_back("mov " + dst + ", " + a);
    out.push_back("load_var " + dst);
    out.push_back("push " + b);
    out.push_back("div");
    out.push_back("pop " + dst);
    return out;
}

inline MacroExpandResult macro_mov_mod(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto dst = args[0], a = args[1], b = args[2];
    out.push_back("mov " + dst + ", " + a);
    out.push_back("load_var " + dst);
    out.push_back("push " + b);
    out.push_back("mod");
    out.push_back("pop " + dst);
    return out;
}

// ==================== 【新增：变量拷贝 & 批量操作】 ====================
inline MacroExpandResult macro_copy(const std::vector<std::string>& args) {
    MacroExpandResult out;
    out.push_back("mov " + args[0] + ", " + args[1]);
    return out;
}

inline MacroExpandResult macro_mov_zero(const std::vector<std::string>& args) {
    MacroExpandResult out;
    out.push_back("mov " + args[0] + ", 0");
    return out;
}

inline MacroExpandResult macro_clr_vars(const std::vector<std::string>& args) {
    MacroExpandResult out;
    for(auto &v : args){
        out.push_back("mov " + v + ", 0");
    }
    return out;
}

// ==================== 【新增：栈便捷宏】 ====================
inline MacroExpandResult macro_push_vars(const std::vector<std::string>& args) {
    MacroExpandResult out;
    for(auto &v : args){
        out.push_back("load_var " + v);
    }
    return out;
}

inline MacroExpandResult macro_pop_vars(const std::vector<std::string>& args) {
    MacroExpandResult out;
    for(auto &v : args){
        out.push_back("pop " + v);
    }
    return out;
}

// ==================== 【新增：IO打印增强】 ====================
inline MacroExpandResult macro_print_var(const std::vector<std::string>& args) {
    MacroExpandResult out;
    out.push_back("load_var " + args[0]);
    out.push_back("print -1");
    return out;
}

inline MacroExpandResult macro_println_str(const std::vector<std::string>& args) {
    MacroExpandResult out;
    out.push_back("print \"" + args[0] + "\\n");
    return out;
}

// ==================== 【新增：条件跳转宏；注意：会消耗栈上两个元素，VM无cmp指令】 ====================
inline MacroExpandResult macro_if_lt(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto a = args[0], b = args[1], label = args[2];
    out.push_back("load_var " + a);
    out.push_back("load_var " + b);
    out.push_back("jl " + label);
    return out;
}

inline MacroExpandResult macro_if_gt(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto a = args[0], b = args[1], label = args[2];
    out.push_back("load_var " + a);
    out.push_back("load_var " + b);
    out.push_back("jg " + label);
    return out;
}

inline MacroExpandResult macro_if_le(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto a = args[0], b = args[1], label = args[2];
    out.push_back("load_var " + a);
    out.push_back("load_var " + b);
    out.push_back("jle " + label);
    return out;
}

inline MacroExpandResult macro_if_ge(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto a = args[0], b = args[1], label = args[2];
    out.push_back("load_var " + a);
    out.push_back("load_var " + b);
    out.push_back("jge " + label);
    return out;
}

inline MacroExpandResult macro_if_eq(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto a = args[0], b = args[1], label = args[2];
    out.push_back("load_var " + a);
    out.push_back("load_var " + b);
    out.push_back("je " + label);
    return out;
}

inline MacroExpandResult macro_if_ne(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto a = args[0], b = args[1], label = args[2];
    out.push_back("load_var " + a);
    out.push_back("load_var " + b);
    out.push_back("jne " + label);
    return out;
}

inline MacroExpandResult macro_swap3(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto a = args[0], b = args[1], c = args[2];
    // 栈： a b c
    out.push_back("load_var " + a);
    out.push_back("load_var " + b);
    out.push_back("load_var " + c);
    // 栈布局 [a, b, c]
    // pop顺序：c b a
    out.push_back("pop " + a); // a = c
    out.push_back("pop " + c); // c = b
    out.push_back("pop " + b); // b = a(old)
    // 完成轮换 a←c, c←b, b←原来a；栈回归平衡
    return out;
}

inline MacroExpandResult macro_var_inc_to(const std::vector<std::string>& args) {
    MacroExpandResult out;
    std::string var = args[0], target = args[1];
    out.push_back("push " + target);
    out.push_back("inc_to " + var);
    // inc_to会push结果，这里弹出丢弃，保证栈平衡，避免栈垃圾堆积
    out.push_back("pop");
    return out;
}

inline MacroExpandResult macro_do_inc(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto var = args[0], target = args[1], jmp_body = args[2];
    // do‑while：先执行循环体，再判断条件
    // 1. 变量单步自增
    out.push_back("inc " + var);
    // 2. 压入 var, target，比较 var < target，如果成立跳回循环头部继续
    out.push_back("load_var " + var);
    out.push_back("push " + target);
    out.push_back("jl " + jmp_body);
    // jl会pop掉两个栈元素，栈恢复平衡
    return out;
}
// arr_set(ptr, offset, value) → heap[ptr+offset]=value
inline MacroExpandResult macro_arr_set(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto ptr = args[0], off = args[1], val = args[2];
    out.push_back("load_var " + ptr);
    out.push_back("push " + off);
    out.push_back("add");
    out.push_back("load_var " + val);
    out.push_back("store_mem");
    return out;
}
// arr_get(ptr,offset) → push heap[ptr+offset]
inline MacroExpandResult macro_arr_get(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto ptr = args[0], off = args[1];
    out.push_back("load_var " + ptr);
    out.push_back("push " + off);
    out.push_back("add");
    out.push_back("load_mem");
    return out;
}


inline const std::unordered_map<std::string, MacroHandler> MACRO_MAP = {
    {"mov_add", macro_mov_add},
    {"mov_sub", macro_mov_sub},
    {"mov_mul", macro_mov_mul},
    {"mov_div", macro_mov_div},
    {"mov_mod", macro_mov_mod},
    {"copy", macro_copy},
    {"mov_zero", macro_mov_zero},
    {"clr_vars", macro_clr_vars},
    {"push_vars", macro_push_vars},
    {"pop_vars", macro_pop_vars},
    {"print_var", macro_print_var},
    {"println_str", macro_println_str},

    {"if_lt", macro_if_lt},
    {"if_gt", macro_if_gt},
    {"if_le", macro_if_le},
    {"if_ge", macro_if_ge},
    {"if_eq", macro_if_eq},
    {"if_ne", macro_if_ne},

    {"do_inc", macro_do_inc},
    {"swap3", macro_swap3},

    {"var_inc_to", macro_var_inc_to},
    {"print_str", macro_print_str},
    {"swap_var", macro_swap_var},
        
    {"arr_set", macro_arr_set},
    {"arr_get", macro_arr_get}
};


// C++11 兼容，不用 contains
inline bool IsMacroOpcode(const std::string& op) {
    return MACRO_MAP.find(op) != MACRO_MAP.end();
}

inline MacroExpandResult ExpandSingleMacro(const std::string& op, const std::vector<std::string>& args) {
    auto it = MACRO_MAP.find(op);
    if (it == MACRO_MAP.end()) return {};
    return it->second(args);
}
}
#endif
