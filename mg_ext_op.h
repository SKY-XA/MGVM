#ifndef MG_EXT_OP_H
#define MG_EXT_OP_H
/*
 * MGVM Implementation
 * Copyright © 2026 SKY‑XA
 * Underlying runtime implementation for two self‑developed programming languages.
 * These two languages may be developed and released in the future.
 * Full license terms: see repository root LICENSE / LICENSE‑CH.
 * Note: Official full bundled interpreters are copyrighted.
 */

#include "mg_opcode.h"
#include <unordered_map>
#include <string>

// 子handler类型，与顶层InstructionHandler签名完全对齐
typedef void (*ExtHandler)(MGVM* vm, void* inst);

// 扩展子op枚举
enum ExtSubOp : int
{
    extop_nop = 0,
    extop_itof,
    extop_ftoi,
    extop_fadd,
    extop_fsub,
    extop_fmul,
    extop_fdiv,
    extop_fneg,
    extop_fabs,
    extop_fsqrt,
    extop_fsin,
    extop_fcos,
    extop_floor,
    extop_ceil,
    extop_fpush,

    extop_max // 数组大小标记
};

// 获取子op名字映射（用于反汇编美化）
const std::unordered_map<int, std::string>& GetExtOpNameMap();
// 获取子op名字→id反向映射（汇编解析器用：ext extop_fadd → 数字id）
const std::unordered_map<std::string, int>& GetExtOpNameToIdMap();

// ========== 动态注册API（可选，插件扩展用） ==========
// 注册一个扩展子handler，返回分配的sub_op id
int ExtOpRegisterHandler(ExtHandler h);
// 获取静态内置handler表指针
ExtHandler* GetBuiltinExtHandlerTable();
// 获取内置表大小
int GetBuiltinExtOpCount();

#endif // MG_EXT_OP_H
