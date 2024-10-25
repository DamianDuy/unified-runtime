/*
 *
 * Copyright (C) 2024 Intel Corporation
 *
 * Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM Exceptions.
 * See LICENSE.TXT
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 */

#include "ur_sanitizer_layer.hpp"

#include "llvm/DebugInfo/Symbolize/DIPrinter.h"
#include "llvm/DebugInfo/Symbolize/Symbolize.h"

namespace ur_sanitizer_layer {

llvm::symbolize::PrinterConfig GetPrinterConfig() {
    llvm::symbolize::PrinterConfig Config;
    Config.Pretty = false;
    Config.PrintAddress = false;
    Config.PrintFunctions = true;
    Config.SourceContextLines = 0;
    Config.Verbose = false;
    return Config;
}

} // namespace ur_sanitizer_layer

extern "C" {

void SymbolizeCode(const char *ModuleName, uint64_t ModuleOffset,
                   char *ResultString, size_t ResultSize, size_t *RetSize) {
    std::string Result;
    llvm::raw_string_ostream OS(Result);
    llvm::symbolize::Request Request{ModuleName, ModuleOffset};
    llvm::symbolize::PrinterConfig Config =
        ur_sanitizer_layer::GetPrinterConfig();
    llvm::symbolize::ErrorHandler EH = [&](const llvm::ErrorInfoBase &ErrorInfo,
                                           llvm::StringRef ErrorBanner) {
        OS << ErrorBanner;
        ErrorInfo.log(OS);
        OS << '\n';
    };
    auto Printer =
        std::make_unique<llvm::symbolize::LLVMPrinter>(OS, EH, Config);

    auto ResOrErr = ur_sanitizer_layer::getContext()->symbolizer.symbolizeInlinedCode(
        ModuleName,
        {ModuleOffset, llvm::object::SectionedAddress::UndefSection});

    if (!ResOrErr) {
        return;
    }
    Printer->print(Request, *ResOrErr);
    ur_sanitizer_layer::getContext()->symbolizer.pruneCache();
    if (RetSize) {
        *RetSize = Result.size() + 1;
    }
    if (ResultString) {
        std::strncpy(ResultString, Result.c_str(), ResultSize);
        ResultString[ResultSize - 1] = '\0';
    }
}
}
