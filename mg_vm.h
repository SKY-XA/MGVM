#ifndef MG_VM_H
#define MG_VM_H
/*
 * MGVM — Stack‑based Virtual Machine Assembler Interpreter
 * Copyright © 2026 SKY-XA
 * Version: V1.2.6
 * This is the underlying runtime for self‑developed ML (MemLock) and Lava languages.
 * Full license terms are located in repository root: LICENSE / LICENSE‑CH.
 * Note: Official full‑version interpreters bundled with ML and Lava may retain separate copyright.
 */

#include "mg_opcode.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

// 符号表项：标签名 -> 指令PC索引
struct SymbolEntry {
    std::string name;
    int32_t pc;
};

#define MG_VM_HAS_VERSION_NUMBER 1 // 是否有版本号字符串标记（不得删除）
class MGVM {
public:
    MGVM();
    void reset();
    bool parseASM(const std::string& asmCode);
    void run();
    void stop();

    // 【极速优化】数组栈，完全替代std::stack
    std::vector<long long> stack;
    std::vector<std::string> str_stack;
    int str_sp=0;
    int sp=0;

    inline void pushStr(const std::string& s) {
        if (str_sp >= (int)str_stack.size()) {
            str_stack.push_back(s);
        } else {
            str_stack[str_sp] = s;
        }
        str_sp++;
    }
    inline std::string popStr() {
        if (str_sp <= 0) return "";
        return str_stack[--str_sp];
    }
    inline bool strStackEmpty() {
        return str_sp == 0;
    }
    inline int getStrStackSize() {
        return str_sp;
    }
    inline void pushStack(long long val) {
        stack[sp++] = val;
    }
    inline long long popStack() {
        return (sp > 0) ? stack[--sp] : 0;
    }
    inline bool isStackEmpty() {
        return sp == 0;
    }
    inline int getStackSize() {
        return sp;
    }

    // 【极速优化】变量：名字→索引 + 数组存储，替代unordered_map
    std::vector<long long> vars;
    std::unordered_map<std::string, int> varIndex;
    int nextVarIndex = 0;

    inline int getVarId(const std::string& name) {
        auto it = varIndex.find(name);
        if(it != varIndex.end()) return it->second;
        int idx = nextVarIndex++;
        varIndex[name] = idx;
        vars.push_back(0);
        return idx;
    }
    inline void setVar(const std::string& name, long long val) {
        int idx = getVarId(name);
        vars[idx] = val;
    }
    inline long long getVar(const std::string& name) {
        int idx = getVarId(name);
        return vars[idx];
    }

    // PC操作
    inline void setPC(int newPC) {
        pc = newPC;
    }
    inline int getPC() {
        return pc;
    }

    // 字节码相关
    bool compileToBytecode(std::vector<uint8_t>& outBuf);
    bool saveBytecode(const std::string& path, const std::vector<uint8_t>& buf);
    bool loadBytecode(const std::string& path, std::vector<uint8_t>& outBuf);
    void runBytecode(const std::vector<uint8_t>& bytecode,bool run_directly=true);

    // 新增：符号表、字符串常量池（编译/加载后填充）
    std::vector<SymbolEntry> symbolTable;
    std::vector<std::string> stringPool;

    // 反汇编工具
    // 反汇编：输出汇编文本到std::string
    std::string disasm();
    // 反汇编并直接打印到控制台
    void disasmPrint();


    // 版本号
	std::string Version = "ML-ASM VM free V1.2.6"; // 当前版本号
	std::string version = "ML-ASM VM free V1.2.5"; // 上一个版本号
	std::string previous_version = version;
	std::string next_version = "ML-ASM VM free V1.2.7"; // 下一个版本号
	std::string VERSION = "ML Vt0.x.x-X 0\nLava Vt0.x.x-X 0"; // 所属ML/Lava主版本号
	int version_code = 27;
	std::string update_log = R"(
V1.2.5: add symbol table + string pool {
这个版本对字节码的格式进行了修改，
增添了标签字符段（符号表段）和字符串段
This version modified the format of the bytecode,
adding a label character field (symbol table field) and a string field.
	
}
V1.2.6: Add disassembly tool function added {
	这个版本添加了反汇编工具函数，
	可以根据.mgc字节码，将其还原成.mgasm汇编
	This version adds disassembly tool functions,
	which can reverse the .mgc bytecode into .mgasm assembly according to .mgc bytecodes.
}

)"; // 更新日志 "

private:
    std::string trim(const std::string& s);
    std::string toLower(const std::string& s);
    std::vector<Instruction> code;
    std::unordered_map<std::string, int> labelMap;
    int pc = 0;
    bool isRunning = false;

public:
    InstructionHandler handlers[OP_COUNT] = {nullptr};
};

MGVM _asm_vm__;
#endif // MG_VM_H
// >_ mg_vm.h

#include "mg_instructions_impl.h"
#include "mg_vm_impl.h"
// V1.2.6
// V1.2.5 add symbol table + string pool
