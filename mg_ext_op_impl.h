#ifndef MG_EXT_OP_IMPL_H
#define MG_EXT_OP_IMPL_H
#include "mg_ext_op.h"
#include "mg_vm.h"
#include <cstring>
#include <cmath>
/*
 * MGVM Implementation
 * Copyright © 2026 SKY‑XA
 * Underlying runtime implementation for two self‑developed programming languages.
 * These two languages may be developed and released in the future.
 * Full license terms: see repository root LICENSE / LICENSE‑CH.
 * Note: Official full bundled interpreters are copyrighted.
 */

#define VM MGVM* vm
#define INS Instruction* inst = (Instruction*)param

// -------------------------- 全部内置ext子handler --------------------------
inline void ext_handler_nop(VM, void* param)
{
    (void)vm; (void)param;
}

// pop整数栈 → push浮点栈 int→double
inline void ext_handler_itof(VM, void* param)
{
    INS;
    long long iv = vm->popStack();
    vm->pushF(static_cast<double>(iv));
}

// pop浮点栈 → push整数栈 double→long long
inline void ext_handler_ftoi(VM, void* param)
{
    INS;
    double fv = vm->popF();
    vm->pushStack(static_cast<long long>(fv));
}

inline void ext_handler_fadd(VM, void* param)
{
    INS;
    double b = vm->popF();
    double a = vm->popF();
    vm->pushF(a + b);
}

inline void ext_handler_fsub(VM, void* param)
{
    INS;
    double b = vm->popF();
    double a = vm->popF();
    vm->pushF(a - b);
}

inline void ext_handler_fmul(VM, void* param)
{
    INS;
    double b = vm->popF();
    double a = vm->popF();
    vm->pushF(a * b);
}

inline void ext_handler_fdiv(VM, void* param)
{
    INS;
    double b = vm->popF();
    double a = vm->popF();
    vm->pushF(a / b);
}

inline void ext_handler_fneg(VM, void* param)
{
    INS;
    double f = vm->popF();
    vm->pushF(-f);
}

inline void ext_handler_fabs(VM, void* param)
{
    INS;
    double f = vm->popF();
    vm->pushF(std::fabs(f));
}

inline void ext_handler_fsqrt(VM, void* param)
{
    INS;
    double f = vm->popF();
    vm->pushF(std::sqrt(f));
}

inline void ext_handler_fsin(VM, void* param)
{
    INS;
    double f = vm->popF();
    vm->pushF(std::sin(f));
}

inline void ext_handler_fcos(VM, void* param)
{
    INS;
    double f = vm->popF();
    vm->pushF(std::cos(f));
}

inline void ext_handler_floor(VM, void* param)
{
    INS;
    double f = vm->popF();
    vm->pushF(std::floor(f));
}

inline void ext_handler_ceil(VM, void* param)
{
    INS;
    double f = vm->popF();
    vm->pushF(std::ceil(f));
}

// extop_fpush：将inst.fnum压入浮点栈
inline void ext_handler_fpush(VM, void* param)
{
    INS;
    vm->pushF(inst->fnum);
}

inline void ext_handler_invalid(VM, void* param)
{
    // 无效子op兜底
}


// -------------------------- 静态内置handler表 --------------------------
static ExtHandler g_builtin_ext_handlers[extop_max] = {
    ext_handler_nop,
    ext_handler_itof,
    ext_handler_ftoi,
    ext_handler_fadd,
    ext_handler_fsub,
    ext_handler_fmul,
    ext_handler_fdiv,
    ext_handler_fneg,
    ext_handler_fabs,
    ext_handler_fsqrt,
    ext_handler_fsin,
    ext_handler_fcos,
    ext_handler_floor,
    ext_handler_ceil,
    ext_handler_fpush,
};

// 子op名字映射，用于反汇编美化
static const std::unordered_map<int, std::string> g_extop_name_map = {
    {extop_nop,   "extop_nop"},
    {extop_itof,  "extop_itof"},
    {extop_ftoi,  "extop_ftoi"},
    {extop_fadd,  "extop_fadd"},
    {extop_fsub,  "extop_fsub"},
    {extop_fmul,  "extop_fmul"},
    {extop_fdiv,  "extop_fdiv"},
    {extop_fneg,  "extop_fneg"},
    {extop_fabs,  "extop_fabs"},
    {extop_fsqrt, "extop_fsqrt"},
    {extop_fsin,  "extop_fsin"},
    {extop_fcos,  "extop_fcos"},
    {extop_floor, "extop_floor"},
    {extop_ceil,  "extop_ceil"},
    {extop_fpush, "extop_fpush"},    
};
static const std::unordered_map<std::string, int> g_extop_name_to_id_map = {
    {"extop_nop", extop_nop},
    {"extop_itof", extop_itof},
    {"extop_ftoi", extop_ftoi},
    {"extop_fadd", extop_fadd},
    {"extop_fsub", extop_fsub},
    {"extop_fmul", extop_fmul},
    {"extop_fdiv", extop_fdiv},
    {"extop_fneg", extop_fneg},
    {"extop_fabs", extop_fabs},
    {"extop_fsqrt", extop_fsqrt},
    {"extop_fsin", extop_fsin},
    {"extop_fcos", extop_fcos},
    {"extop_floor", extop_floor},
    {"extop_ceil", extop_ceil},
    {"extop_fpush", extop_fpush},
};

// 动态注册表（运行时插件扩展）
static std::vector<ExtHandler> g_dynamic_ext_handlers;

// -------------------------- API实现 --------------------------
inline const std::unordered_map<int, std::string>& GetExtOpNameMap()
{
    return g_extop_name_map;
}

inline const std::unordered_map<std::string, int>& GetExtOpNameToIdMap()
{
    return g_extop_name_to_id_map;
}

inline ExtHandler* GetBuiltinExtHandlerTable()
{
    return g_builtin_ext_handlers;
}

inline int GetBuiltinExtOpCount()
{
    return extop_max;
}

inline int ExtOpRegisterHandler(ExtHandler h)
{
    int id = static_cast<int>(g_dynamic_ext_handlers.size()) + extop_max;
    g_dynamic_ext_handlers.push_back(h);
    return id;
}

#undef VM
#undef INS
#endif // MG_EXT_OP_IMPL_H