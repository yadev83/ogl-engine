#include "stacktrace.hpp"
#include "logger.hpp"

namespace Engine::Core {    
    void PrintStackTrace(CONTEXT *context, std::ostream& out) {
        HANDLE process = GetCurrentProcess();
        HANDLE thread = GetCurrentThread();
    
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
        SymInitialize(process, NULL, TRUE);
    
        STACKFRAME64 stack = {};
        DWORD imageType;
    
        #ifdef _M_IX86
            imageType = IMAGE_FILE_MACHINE_I386;
            stack.AddrPC.Offset = context->Eip;
            stack.AddrPC.Mode = AddrModeFlat;
            stack.AddrFrame.Offset = context->Ebp;
            stack.AddrFrame.Mode = AddrModeFlat;
            stack.AddrStack.Offset = context->Esp;
            stack.AddrStack.Mode = AddrModeFlat;
        #elif _M_X64
            imageType = IMAGE_FILE_MACHINE_AMD64;
            stack.AddrPC.Offset = context->Rip;
            stack.AddrPC.Mode = AddrModeFlat;
            stack.AddrFrame.Offset = context->Rsp;
            stack.AddrFrame.Mode = AddrModeFlat;
            stack.AddrStack.Offset = context->Rsp;
            stack.AddrStack.Mode = AddrModeFlat;
        #else
            #error "Unsupported platform"
        #endif
    
        for(int i = 0; i < 64; ++i) {
            if(!StackWalk64(imageType, process, thread, &stack, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
                break;
            }
    
            DWORD64 addr = stack.AddrPC.Offset;
            if(addr == 0) break;
    
            char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
            SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(buffer);
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbol->MaxNameLen = MAX_SYM_NAME;
    
            DWORD64 displacement;
            if (SymFromAddr(process, addr, &displacement, symbol)) {
                out << symbol->Name << " - 0x" << std::hex << symbol->Address << std::dec << "\n";
    
                IMAGEHLP_LINE64 line;
                DWORD displacement = 0;
                ZeroMemory(&line, sizeof(IMAGEHLP_LINE64));
                line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    
                if (SymGetLineFromAddr64(process, addr, &displacement, &line)) {
                    out << "    at " << line.FileName << ":" << line.LineNumber << "\n";
                }
            } else {
                out << i << ": (no symbol)\n";
            }
        }
    
        SymCleanup(process);
    }
    
    LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* exceptionInfo) {
        std::cerr << "The program encountered an exception - Code: " << ExceptionCodeString(exceptionInfo->ExceptionRecord->ExceptionCode) << "(" << std::hex << exceptionInfo->ExceptionRecord->ExceptionCode << std::dec << ")" << std::endl;
        PrintStackTrace(exceptionInfo->ContextRecord);
        PrintStackTrace(exceptionInfo->ContextRecord, Logger::Instance().Stream());
    
        return EXCEPTION_EXECUTE_HANDLER;
    }
    
    std::string ExceptionCodeString(DWORD code) {
        switch(code) {
            case EXCEPTION_ACCESS_VIOLATION: return "EXCEPTION_ACCESS_VIOLATION";
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
            case EXCEPTION_BREAKPOINT: return "EXCEPTION_BREAKPOINT";
            case EXCEPTION_DATATYPE_MISALIGNMENT: return "EXCEPTION_DATATYPE_MISALIGNMENT";
            case EXCEPTION_FLT_DENORMAL_OPERAND: return "EXCEPTION_FLT_DENORMAL_OPERAND";
            case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
            case EXCEPTION_FLT_INEXACT_RESULT: return "EXCEPTION_FLT_INEXACT_RESULT";
            case EXCEPTION_FLT_INVALID_OPERATION: return "EXCEPTION_FLT_INVALID_OPERATION";
            case EXCEPTION_FLT_OVERFLOW: return "EXCEPTION_FLT_OVERFLOW";
            case EXCEPTION_FLT_UNDERFLOW: return "EXCEPTION_FLT_UNDERFLOW";
            case EXCEPTION_FLT_STACK_CHECK: return "EXCEPTION_FLT_STACK_CHECK";
            case EXCEPTION_ILLEGAL_INSTRUCTION: return "EXCEPTION_ILLEGAL_INSTRUCTION";
            case EXCEPTION_IN_PAGE_ERROR: return "EXCEPTION_IN_PAGE_ERROR";
            case EXCEPTION_INT_DIVIDE_BY_ZERO: return "EXCEPTION_INT_DIVIDE_BY_ZERO";
            case EXCEPTION_INT_OVERFLOW: return "EXCEPTION_INT_OVERFLOW";
            case EXCEPTION_INVALID_DISPOSITION: return "EXCEPTION_INVALID_DISPOSITION";
            case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
            case EXCEPTION_PRIV_INSTRUCTION: return "EXCEPTION_PRIV_INSTRUCTION";
            case EXCEPTION_SINGLE_STEP: return "EXCEPTION_SINGLE_STEP";
            case EXCEPTION_STACK_OVERFLOW: return "EXCEPTION_STACK_OVERFLOW";
            default: return "EXCEPTION";
        }
    }
    
    void EnableStacktrace() {
        SetUnhandledExceptionFilter(UnhandledExceptionHandler);
    }
}