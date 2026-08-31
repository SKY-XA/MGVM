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

inline const std::unordered_map<std::string, MacroHandler> MACRO_MAP = {
    {"mov_add", macro_mov_add},
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
