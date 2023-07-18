/*
 * Copyright (c) 2021-22 CHIP-SPV developers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef HIP_COMMON_HH
#define HIP_COMMON_HH

#include "CHIPSPVConfig.hh"
#include "SPIRVFuncInfo.hh"

#include <map>
#include <set>
#include <vector>
#include <stdint.h>
#include <string>
#include <memory>
#include <unordered_set>
#include <utility>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <mutex>
#include <queue>
#include <stack>

/// For multiplexing purposes, the first field of our objects must be a void *
/// pointer
struct ihipDispatch {
  void *dispatch;
};

#define CHIP_HANDLE_TO_OBJ(handle, type)                                       \
  (reinterpret_cast<type *>(reinterpret_cast<uintptr_t>(handle) +              \
                            sizeof(ihipDispatch)))

#define CHIP_OBJ_TO_HANDLE(pobj, type)                                         \
  (reinterpret_cast<type *>(reinterpret_cast<uintptr_t>(pobj) -                \
                            sizeof(ihipDispatch)))

#define QUEUE(x)                                                               \
  ((!(x) || (x) == hipStreamPerThread || (x) == hipStreamLegacy)               \
       ? reinterpret_cast<chipstar::Queue *>(x)                                \
       : CHIP_HANDLE_TO_OBJ(x, chipstar::Queue))

#define STREAM(x)                                                              \
  ((!(x) || (x) == (chipstar::Queue *)hipStreamPerThread ||                    \
    (x) == (chipstar::Queue *)hipStreamLegacy)                                 \
       ? reinterpret_cast<ihipStream_t *>(x)                                   \
       : CHIP_OBJ_TO_HANDLE(x, ihipStream_t))

#define HIPTOCHIP(x, t)                                                        \
  (!(x) ? reinterpret_cast<t *>(x) : CHIP_HANDLE_TO_OBJ(x, t))

#define CHIPTOHIP(x, t)                                                        \
  (!(x) ? reinterpret_cast<t *>(x) : CHIP_OBJ_TO_HANDLE(x, t))

#define EVENT(x) HIPTOCHIP(x, chipstar::Event)

#define HIPEVENT(x) CHIPTOHIP(x, ihipEvent_t)

#define MODULE(x) HIPTOCHIP(x, chipstar::Module)

#define HIPMODULE(x) CHIPTOHIP(x, ihipModule_t)

#define KERNEL(x) HIPTOCHIP(x, chipstar::Kernel)

#define HIPMODULESYMBOL(x) CHIPTOHIP(x, ihipModuleSymbol_t)

#define CONTEXT(x) HIPTOCHIP(x, chipstar::Context)

#define HIPCONTEXT(x) CHIPTOHIP(x, ihipCtx_t)

#define GRAPH(x) HIPTOCHIP(x, CHIPGraph)

#define HIPGRAPH(x) CHIPTOHIP(x, ihipGraph)

#define GRAPHEXEC(x) HIPTOCHIP(x, CHIPGraphExec)

#define HIPGRAPHEXEC(x) CHIPTOHIP(x, hipGraphExec)

#define GRAPHNODE(x) HIPTOCHIP(x, CHIPGraphNode)

#define HIPGRAPHNODE(x) CHIPTOHIP(x, hipGraphNode)

/// The implementation of ihipEvent_t. The chipstar::Event class inherits this
/// so ihipEvent_t pointers may carry chipstar::Event instances.
struct ihipEvent_t : ihipDispatch {};
struct ihipCtx_t : ihipDispatch {};
struct ihipStream_t : ihipDispatch {};
struct ihipModule_t : ihipDispatch {};
struct ihipModuleSymbol_t : ihipDispatch {};
struct ihipGraph : ihipDispatch {};
struct hipGraphNode : ihipDispatch {};
struct hipGraphExec : ihipDispatch {};

bool filterSPIRV(const char *Bytes, size_t NumBytes, std::string &Dst);
bool parseSPIR(uint32_t *Stream, size_t NumWords,
               OpenCLFunctionInfoMap &FuncInfoMap);

/// A prefix given to lowered global scope device variables.
constexpr char ChipVarPrefix[] = "__chip_var_";
/// A prefix used for a shadow kernel used for querying device
/// variable properties.
constexpr char ChipVarInfoPrefix[] = "__chip_var_info_";
/// A prefix used for a shadow kernel used for binding storage to
/// device variables.
constexpr char ChipVarBindPrefix[] = "__chip_var_bind_";
/// A prefix used for a shadow kernel used for initializing device
/// variables.
constexpr char ChipVarInitPrefix[] = "__chip_var_init_";
/// A structure to where properties of a device variable are written.
/// CHIPVarInfo[0]: Size in bytes.
/// CHIPVarInfo[1]: Requested alignment.
/// CHIPVarInfo[2]: Non-zero if variable has initializer. Otherwise zero.
using CHIPVarInfo = int64_t[3];

/// The name of the shadow kernel responsible for resetting host-inaccessible
/// global device variables (e.g. static local variables in device code).
constexpr char ChipNonSymbolResetKernelName[] = "__chip_reset_non_symbols";

/// The prefix for global-scope variables in SPIR-V modules for carrying
/// information about "spilled" arguments
///
/// see HipKernelArgSpiller.cpp for details. Full name of such
/// variables is '<ChipSpilledArgsVarPrefix><kernel-name>'
constexpr char ChipSpilledArgsVarPrefix[] = "__chip_spilled_args_";

/// The name of a global variable which indicates, when non-zero, if
/// the abort() function was called by a kernel.
constexpr char ChipDeviceAbortFlagName[] = "__chipspv_abort_called";

#endif
