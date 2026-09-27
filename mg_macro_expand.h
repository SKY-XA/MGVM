#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "mg_macro.h"
#include "mg_vm.h"
/*
 * MGVM Implementation
 * Copyright © 2026 SKY-XA
 * Underlying runtime implementation for self‑developed ML (MemLock) & Lava languages.
 * Full license terms: see repository root LICENSE / LICENSE‑CH.
 * Note: Official full bundled interpreters of ML & Lava are copyrighted.
 */
namespace mg_macro {
// 复用VM内置trim/toLower工具，全局文本展开
inline std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) start++;
    auto end = s.end();
    do { end--; } while (end > start && std::isspace(static_cast<unsigned char>(*end)));
    return std::string(start, end + 1);
}

inline std::string toLower(const std::string& s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return res;
}

// 分割指令参数，支持逗号分隔
inline std::vector<std::string> SplitMacroArgs(const std::string& paramLine) {
    std::vector<std::string> args;
    std::string buf;
    bool inQuote = false;
    for (char ch : paramLine) {
        if (ch == '"') inQuote = !inQuote;
        if (!inQuote && ch == ',') {
            args.push_back(trim(buf));
            buf.clear();
            continue;
        }
        buf += ch;
    }
    if (!trim(buf).empty()) args.push_back(trim(buf));
    return args;
}

// 核心入口：原始ASM文本 → 消除所有复合指令后的纯基础ASM
inline std::string ExpandAllMacro(const std::string& rawAsm) {
    std::istringstream stream(rawAsm);
    std::string line;
    std::stringstream output;

    while (std::getline(stream, line)) {
        std::string rawLine = line;
        std::string trimLine = trim(rawLine);
        // 空行直接保留
        if (trimLine.empty()) {
            output << rawLine << "\n";
            continue;
        }
        // 截取注释前内容
        size_t commentPos = trimLine.find(';');
        std::string codePart = trimLine;
        std::string comment = "";
        if (commentPos != std::string::npos) {
            codePart = trim(trimLine.substr(0, commentPos));
            comment = trimLine.substr(commentPos);
        }
        // 处理标签行：标签直接输出，后面指令继续解析
        size_t colonPos = codePart.find(':');
        std::string labelPrefix = "";
        if (colonPos != std::string::npos) {
            labelPrefix = trim(codePart.substr(0, colonPos)) + ":";
            codePart = trim(codePart.substr(colonPos + 1));
        }
        // 无指令只有标签，直接输出
        if (codePart.empty()) {
            output << labelPrefix << comment << "\n";
            continue;
        }
        // 拆分指令名与参数
        std::istringstream lineSs(codePart);
        std::string opStr;
        lineSs >> opStr;
        std::string restParam;
        std::getline(lineSs, restParam);
        opStr = toLower(opStr);
        // 判断是否为复合宏指令
        if (IsMacroOpcode(opStr)) {
            // 分割参数，展开为多条基础指令
            auto args = SplitMacroArgs(restParam);
            auto expandLines = ExpandSingleMacro(opStr, args);
            // 标签只加在第一条展开指令前
            bool first = true;
            for (auto& expandLine : expandLines) {
                if (first && !labelPrefix.empty()) {
                    output << labelPrefix << " " << expandLine << comment << "\n";
                    first = false;
                } else {
                    output << expandLine << comment << "\n";
                }
            }
        } else {
            // 普通基础指令，原样输出
            std::string fullLine = labelPrefix;
            if (!codePart.empty()) fullLine += " " + codePart;
            output << fullLine << comment << "\n";
        }
    }
    return output.str();
}
}
