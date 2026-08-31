#pragma once
#include "mg_vm.h"
#include "mg_opcode.h"
#include "mg_instructions.h"
#include "mg_macro_expand.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <vector>
#include <cstdint>
#include <unordered_map>
/*
 * MGVM Implementation
 * Copyright © 2026 SKY-XA
 * Underlying runtime implementation for self‑developed ML (MemLock) & Lava languages.
 * Full license terms: see repository root LICENSE / LICENSE‑CH.
 * Note: Official full bundled interpreters of ML & Lava are copyrighted.
 */


inline const std::unordered_map<std::string, OpCode> OPCODE_MAP = {
    {"push",      op_push},
    {"load_var",  op_load_var},
    {"store_var", op_store_var},
    {"print",     op_print},
    {"inc_to",    op_inc_to},
    {"dec_to",    op_dec_to},
    {"mov",       op_mov},
    {"pop",       op_pop},
    {"add",       op_add},
    {"sub",       op_sub},
    {"mul",       op_mul},
    {"div",       op_div},
    {"mod",       op_mod},
    {"inc",       op_inc},
    {"dec",       op_dec},
    {"and",       op_and},
    {"or",        op_or},
    {"xor",       op_xor},
    {"not",       op_not},
    {"shl",       op_shl},
    {"shr",       op_shr},
    {"jmp",       op_jmp},
    {"jz",        op_jz},
    {"jnz",       op_jnz},
    {"jg",        op_jg},
    {"jl",        op_jl},
    {"call",      op_call},
    {"ret",       op_ret},
    {"neg",       op_neg},
    {"abs",       op_abs},
    {"dup",       op_dup},
    {"swap",      op_swap},
    {"nop",       op_nop},
    {"halt",      op_halt},
    {"je",        op_je},
    {"jne",       op_jne},
    {"jle",       op_jle},
    {"jge",       op_jge},
    {"input",     op_input}
};

inline std::string MGVM::trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) start++;
    auto end = s.end();
    do {
        end--;
    }
    while (end > start && std::isspace(static_cast<unsigned char>(*end)));
    return std::string(start, end + 1);
}

inline std::string MGVM::toLower(const std::string& s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(),
    [](unsigned char c) {
        return std::tolower(c);
    });
    return res;
}

inline void MGVM::reset() {
    code.clear();
    labelMap.clear();
    varIndex.clear();
    nextVarIndex = 0;
    sp = 0;
    str_sp = 0;
    pc = 0;
    isRunning = false;
    vars.clear();
    str_stack.clear();
    str_stack.reserve(4096);

    // 新增：重置符号表、字符串池
    symbolTable.clear();
    stringPool.clear();
}

inline MGVM::MGVM() {
    varIndex.reserve(256);
    vars.reserve(256);
    stack.reserve(4096);
    str_stack.reserve(4096);
    registerAllInstructions(handlers);
}

inline bool MGVM::parseASM(const std::string& asmCode) {
    std::string expandedAsm = mg_macro::ExpandAllMacro(asmCode);
    std::istringstream stream(expandedAsm);
    reset();
    std::string line;
    int lineNum = 0;
    std::vector<std::pair<std::string, int>> labelWaitList;

    // 临时：字符串去重池，汇编解析阶段收集print字符串
    std::unordered_map<std::string, uint32_t> strTempMap;

    while (std::getline(stream, line)) {
        lineNum++;
        std::string trimedLine = trim(line);
        if (trimedLine.empty()) continue;

        size_t commentPos = trimedLine.find(';');
        if (commentPos != std::string::npos) {
            trimedLine = trim(trimedLine.substr(0, commentPos));
            if (trimedLine.empty()) continue;
        }

        size_t colonPos = trimedLine.find(':');
        if (colonPos != std::string::npos) {
            std::string labelName = trim(trimedLine.substr(0, colonPos));
            labelMap[labelName] = static_cast<int>(code.size());
            trimedLine = trim(trimedLine.substr(colonPos + 1));
            if (trimedLine.empty()) continue;
        }

        std::istringstream lineStream(trimedLine);
        std::string opStr;
        lineStream >> opStr;
        opStr = toLower(opStr);
        if (OPCODE_MAP.find(opStr) == OPCODE_MAP.end()) {
            std::cerr << "[解析错误] 第" << lineNum << "行：未知指令 '" << opStr << "'\n";
            return false;
        }

        Instruction inst;
        inst.op = OPCODE_MAP.at(opStr);
        inst.line = lineNum;
        inst.str1.clear();
        inst.str2.clear();
        inst.num = 0;

        auto regVar = [&](const std::string& name) {
            if(!name.empty() && varIndex.find(name) == varIndex.end()) {
                varIndex[name] = nextVarIndex++;
            }
        };

        switch (inst.op) {
        case op_push:
            lineStream >> inst.num;
            break;
        case op_load_var:
        case op_store_var:
        case op_inc_to:
        case op_dec_to:
        case op_inc:
        case op_dec:
        case op_pop:
        case op_neg:
        case op_abs:
        case op_dup:
        case op_swap:
        case op_nop:
        case op_halt:
        case op_not:
            lineStream >> inst.str1;
            regVar(inst.str1);
            break;
        case op_mov: {
            std::string dest_part;
            lineStream >> dest_part;
            size_t commaPos = dest_part.find(',');
            if (commaPos != std::string::npos) {
                inst.str1 = trim(dest_part.substr(0, commaPos));
                regVar(inst.str1);
                std::string src_part;
                lineStream >> src_part;
                if (!src_part.empty() && (std::isdigit(static_cast<unsigned char>(src_part[0])) || src_part[0] == '-')) {
                    inst.num = std::stoll(src_part);
                } else {
                    inst.str2 = src_part;
                    regVar(inst.str2);
                }
            }
            break;
        }
        case op_jmp:
        case op_jz:
        case op_jnz:
        case op_jg:
        case op_jl:
        case op_call:
        case op_je:
        case op_jne:
        case op_jle:
        case op_jge: {
            std::string param;
            lineStream >> param;
            labelWaitList.emplace_back(param, static_cast<int>(code.size()));
            break;
        }
        case op_print: {
            std::string rest;
            std::string allRest;
            while (lineStream >> rest)
            {
                if (!allRest.empty()) allRest += " ";
                allRest += rest;
            }
            size_t qPos = allRest.find('"');
            if (qPos != std::string::npos)
            {
                std::string s = trim(allRest.substr(qPos + 1));
                // 存入字符串常量池，指令num存池索引
                uint32_t idx;
                auto it = strTempMap.find(s);
                if(it != strTempMap.end()) {
                    idx = it->second;
                } else {
                    idx = static_cast<uint32_t>(stringPool.size());
                    strTempMap[s] = idx;
                    stringPool.push_back(s);
                }
                inst.num = static_cast<int64_t>(idx);
            } else {
                // 无引号：打印栈，索引填‑1标记
                inst.num = -1;
            }
            break;
        }
        case op_input:
            break;
        default:
            break;
        }
        code.push_back(inst);
    }

    // 填充符号表：从labelMap拷贝
    symbolTable.clear();
    for(auto& kv : labelMap) {
        SymbolEntry e;
        e.name = kv.first;
        e.pc = kv.second;
        symbolTable.push_back(e);
    }

    // 解析标签地址
    for (auto& item : labelWaitList) {
        std::string labelName = item.first;
        int codeIndex = item.second;
        if (labelMap.find(labelName) == labelMap.end()) {
            std::cerr << "[解析错误] 未知标签 '" << labelName << "'\n";
            return false;
        }
        code[codeIndex].num = labelMap[labelName];
    }
    return true;
}

inline void MGVM::run()
{
    if (code.empty())
    {
        std::cerr << "没有可执行的代码" << std::endl;
        return;
    }
    isRunning = true;
    pc = 0;
    while (isRunning && pc < static_cast<int>(code.size()))
    {
        Instruction& inst = code[pc];
        pc++;
        InstructionHandler handler = handlers[inst.op];
        if (handler != nullptr)
        {
            handler(this, &inst);
        }
        else
        {
            std::cerr << "无效指令" << std::endl;
            isRunning = false;
        }
    }
}

inline void MGVM::stop() {
    isRunning = false;
}

inline bool MGVM::compileToBytecode(std::vector<uint8_t>& outBuf)
{
    outBuf.clear();
    // 内部二进制写工具
    auto writeU8  = [&](uint8_t v) {
        outBuf.push_back(v);
    };
    auto writeU32 = [&](uint32_t v) {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&v);
        outBuf.insert(outBuf.end(), p, p+4);
    };
    auto writeI32 = [&](int32_t v) {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&v);
        outBuf.insert(outBuf.end(), p, p+4);
    };
    auto writeStr = [&](const std::string& s) {
        writeU32(static_cast<uint32_t>(s.size()));
        outBuf.insert(outBuf.end(), s.begin(), s.end());
    };

    // 1.魔数 MGC
    writeU8('M');
    writeU8('G');
    writeU8('C');
    // 字节码格式版本
    const int32_t BC_VERSION = 2;
    writeI32(BC_VERSION);

    // 2.写入符号表段
    writeU32(static_cast<uint32_t>(symbolTable.size()));
    for(auto& se : symbolTable) {
        writeStr(se.name);
        writeI32(se.pc);
    }

    //3.写入字符串常量池段
    writeU32(static_cast<uint32_t>(stringPool.size()));
    for(auto& s : stringPool) {
        writeStr(s);
    }

    // 先临时构建指令流
    std::vector<uint8_t> codeBuf;
    auto cbWriteI32 = [&](int32_t val) {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&val);
        codeBuf.insert(codeBuf.end(), p, p + 4);
    };

    for(const auto& inst : code)
    {
        uint8_t opByte = static_cast<uint8_t>(inst.op);
        codeBuf.push_back(opByte);
        switch(inst.op)
        {
        case op_push:
        case op_jmp:
        case op_jz:
        case op_jnz:
        case op_jg:
        case op_jl:
        case op_call:
        case op_je:
        case op_jne:
        case op_jle:
        case op_jge:
            cbWriteI32(static_cast<int32_t>(inst.num));
            break;
        case op_print:
            cbWriteI32(static_cast<int32_t>(inst.num));
            break;
        case op_load_var:
        case op_store_var:
        case op_inc_to:
        case op_dec_to:
        case op_inc:
        case op_dec:
        case op_pop:
        case op_neg:
        case op_abs:
        case op_dup:
        case op_swap:
        case op_not:
        {
            int32_t vid = 0;
            if(varIndex.find(inst.str1) != varIndex.end())
                vid = static_cast<int32_t>(varIndex[inst.str1]);
            cbWriteI32(vid);
            break;
        }
        case op_mov:
        {
            int32_t id1 = 0, id2 = 0;
            if(varIndex.count(inst.str1)) id1 = (int32_t)varIndex[inst.str1];
            if(varIndex.count(inst.str2)) id2 = (int32_t)varIndex[inst.str2];
            cbWriteI32(id1);
            cbWriteI32(id2);
            cbWriteI32(static_cast<int32_t>(inst.num));
            break;
        }
        case op_add:
        case op_sub:
        case op_mul:
        case op_div:
        case op_mod:
        case op_and:
        case op_or:
        case op_xor:
        case op_shl:
        case op_shr:
        case op_ret:
        case op_nop:
        case op_halt:
        case op_input:
            break;
        default:
            break;
        }
    }
    //4.指令流长度 + 指令流数据
    writeU32(static_cast<uint32_t>(codeBuf.size()));
    outBuf.insert(outBuf.end(), codeBuf.begin(), codeBuf.end());
    return true;
}

inline bool MGVM::saveBytecode(const std::string& path, const std::vector<uint8_t>& buf)
{
    std::ofstream f(path, std::ios::binary);
    if(!f) return false;
    f.write(reinterpret_cast<const char*>(buf.data()), buf.size());
    return true;
}

inline bool MGVM::loadBytecode(const std::string& path, std::vector<uint8_t>& outBuf)
{
    std::ifstream f(path, std::ios::binary);
    if(!f) return false;
    f.seekg(0, std::ios::end);
    size_t sz = f.tellg();
    f.seekg(0, std::ios::beg);
    outBuf.resize(sz);
    f.read(reinterpret_cast<char*>(outBuf.data()), sz);
    return true;
}

inline void MGVM::runBytecode(const std::vector<uint8_t>& bytecode,bool run_directly)
{
    reset();
    size_t offset = 0;
    size_t total = bytecode.size();
    if(total < 12) return;

    auto readU8  = [&]() -> uint8_t { return bytecode[offset++]; };
    auto readU32 = [&]() -> uint32_t {
        uint32_t v = *(reinterpret_cast<const uint32_t*>(bytecode.data()+offset));
        offset +=4;
        return v;
    };
    auto readI32 = [&]() -> int32_t {
        int32_t v = *(reinterpret_cast<const int32_t*>(bytecode.data()+offset));
        offset +=4;
        return v;
    };
    auto readStr = [&]() -> std::string {
        uint32_t len = readU32();
        std::string s(bytecode.data()+offset, bytecode.data()+offset+len);
        offset += len;
        return s;
    };

    // 校验魔数
    char m0 = static_cast<char>(readU8());
    char m1 = static_cast<char>(readU8());
    char m2 = static_cast<char>(readU8());
    if(!(m0=='M'&&m1=='G'&&m2=='C')) {
        std::cerr<<"[runBytecode] 无效字节码魔数\n";
        return;
    }
    int32_t bcVer = readI32();
    const int32_t EXPECT_VER = 2;
    if(bcVer != EXPECT_VER) {
        std::cerr<<"[runBytecode] 字节码版本不匹配，需要v"<<EXPECT_VER<<"，文件是v"<<bcVer<<"\n";
        return;
    }

    // 读符号表
    uint32_t symCount = readU32();
    symbolTable.reserve(symCount);
    for(uint32_t i=0; i<symCount; i++) {
        SymbolEntry e;
        e.name = readStr();
        e.pc = readI32();
        symbolTable.push_back(e);
    }

    // 读字符串常量池
    uint32_t strCount = readU32();
    stringPool.reserve(strCount);
    for(uint32_t i=0; i<strCount; i++) {
        stringPool.push_back(readStr());
    }

    // 读指令流
    uint32_t codeLen = readU32();
    size_t codeStartOff = offset;

    // 解析指令流重建code数组
    while (offset < codeStartOff + codeLen)
    {
        Instruction inst;
        inst.op  = op_invalid;
        inst.num = 0;
        inst.str1.clear();
        inst.str2.clear();
        inst.line = 0;
        uint8_t rawOp = bytecode[offset++];
        inst.op = static_cast<OpCode>(rawOp);
        switch (inst.op)
        {
        case op_push:
        case op_jmp:
        case op_jz:
        case op_jnz:
        case op_jg:
        case op_jl:
        case op_je:
        case op_jne:
        case op_jle:
        case op_jge:
        case op_call:
        case op_print:
            inst.num = readI32();
            break;
        case op_load_var:
        case op_store_var:
        case op_inc_to:
        case op_dec_to:
        case op_inc:
        case op_dec:
        case op_pop:
        case op_neg:
        case op_abs:
        case op_dup:
        case op_swap:
        case op_not:
            inst.num = readI32();
            break;
        case op_mov:
            readI32();
            readI32();
            inst.num = readI32();
            break;
        case op_add:
        case op_sub:
        case op_mul:
        case op_div:
        case op_mod:
        case op_and:
        case op_or:
        case op_xor:
        case op_shl:
        case op_shr:
        case op_ret:
        case op_nop:
        case op_halt:
        case op_input:
            break;
        default:
            break;
        }
        code.push_back(inst);
    }
    if(run_directly){
        run();
    }
}
inline std::string MGVM::disasm()
{
    std::ostringstream oss;
    // 构建反向映射：pc → 标签名（一个pc可能多个标签）
    std::unordered_map<int, std::vector<std::string>> pcToLabels;
    for(auto& entry : symbolTable)
    {
        pcToLabels[entry.pc].push_back(entry.name);
    }

    for(int pc = 0; pc < (int)code.size(); pc++)
    {
        auto& inst = code[pc];
        // 如果当前pc有标签，输出标签
        if(pcToLabels.count(pc))
        {
            for(auto& lab : pcToLabels[pc])
            {
                oss << lab << ":\n";
            }
        }

        // 输出指令
        auto opIt = std::find_if(OPCODE_MAP.begin(), OPCODE_MAP.end(),
        [&](const std::pair<std::string,OpCode>& p) {
            return p.second == inst.op;
        });
        if(opIt == OPCODE_MAP.end())
        {
            oss << "  ; unknown_op(" << (int)inst.op << ")\n";
            continue;
        }
        std::string opname = opIt->first;
        oss << "  " << opname;

        switch(inst.op)
        {
        case op_push:
            oss << " " << inst.num;
            break;
        case op_jmp:
        case op_jz:
        case op_jnz:
        case op_jg:
        case op_jl:
        case op_call:
        case op_je:
        case op_jne:
        case op_jle:
        case op_jge:
        {
            int targetPc = static_cast<int>(inst.num);
            // 优先找标签
            std::string targetName = std::to_string(targetPc);
            for(auto& e : symbolTable)
            {
                if(e.pc == targetPc)
                {
                    targetName = e.name;
                    break;
                }
            }
            oss << " " << targetName;
            break;
        }
        case op_print:
        {
            int idx = static_cast<int>(inst.num);
            if(idx == -1)
            {
                // 打印栈
            }
            else
            {
                if(idx >=0 && idx < (int)stringPool.size())
                {
                    oss << " \"" << stringPool[idx] << "\"";
                }
                else
                {
                    oss << " ?bad_idx(" << idx << ")";
                }
            }
            break;
        }
        case op_load_var:
        case op_store_var:
        case op_inc_to:
        case op_dec_to:
        case op_inc:
        case op_dec:
        case op_pop:
        case op_neg:
        case op_abs:
        case op_dup:
        case op_swap:
        case op_not:
        {
            int vid = static_cast<int>(inst.num);
            oss << " var(" << vid << ")";
            break;
        }
        case op_mov:
        {
            // mov 保存 id1,id2,num
            // 注意：从字节码加载后str1/str2丢失，只能输出id
            // inst.num 是立即数
            oss << " var(??), var(??)";
            if(inst.num != 0)
            {
                oss << ", " << inst.num;
            }
            break;
        }
        case op_add:
        case op_sub:
        case op_mul:
        case op_div:
        case op_mod:
        case op_and:
        case op_or:
        case op_xor:
        case op_shl:
        case op_shr:
        case op_ret:
        case op_nop:
        case op_halt:
        case op_input:
            // 无参数
            break;
        default:
            oss << " ; unhandled op";
            break;
        }
        oss << "\n";
    }
    return oss.str();
}

inline void MGVM::disasmPrint()
{
    std::cout << disasm();
}

//</> mg_vm_impl.h
