#ifndef MG_MACRO_H
#define MG_MACRO_H
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include "mg_opcode.h"
/*
 * MGVM Implementation
 * Copyright © 2026 SKY_XA
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
    out.push_back("print \"" + args[0] + "\\n\"");
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

// ==================== 【新增：循环辅助】 ====================
inline MacroExpandResult macro_do_inc(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto var = args[0], target = args[1], jmp_label = args[2];
    out.push_back("push " + target);
    out.push_back("inc_to " + var);
    out.push_back("jnz " + jmp_label);
    return out;
}

// ==================== 【新增：交换扩展】 ====================
inline MacroExpandResult macro_swap3(const std::vector<std::string>& args) {
    MacroExpandResult out;
    auto a=args[0],b=args[1],c=args[2];
    out.push_back("load_var " + a);
    out.push_back("load_var " + b);
    out.push_back("load_var " + c);
    out.push_back("push " + a);
    out.push_back("push " + c);
    out.push_back("push " + b);
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

    {"loop", macro_loop},
    {"print_str", macro_print_str},
    {"swap_var", macro_swap_var}
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
