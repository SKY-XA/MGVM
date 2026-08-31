#ifndef MG_INSTRUCTIONS_H
#define MG_INSTRUCTIONS_H
/*
 * MGVM Implementation
 * Copyright © 2026 SKY-XA
 * Underlying runtime implementation for self‑developed ML (MemLock) & Lava languages.
 * Full license terms: see repository root LICENSE / LICENSE‑CH.
 * Note: Official full bundled interpreters of ML & Lava are copyrighted.
 */

#include "mg_opcode.h"

// 注册所有指令到数组跳转表
void registerAllInstructions(InstructionHandler handlers[]);

#endif // MG_INSTRUCTIONS_H
// >_ mg_instructions.h
