set(LLVM_TARGET_DEFINITIONS REDEFINE.td)

tablegen(LLVM REDEFINEGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM REDEFINEGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM REDEFINEGenCallingConv.inc -gen-callingconv)
tablegen(LLVM REDEFINEGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM REDEFINEGenMCCodeEmitter.inc -gen-emitter -mc-emitter)
tablegen(LLVM REDEFINEGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM REDEFINEGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM REDEFINEGenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(REDEFINECommonTableGen)

add_llvm_target(REDEFINECodeGen
  REDEFINEAsmPrinter.cpp
  REDEFINECallingConv.cpp
  REDEFINEFrameLowering.cpp
  REDEFINEISelDAGToDAG.cpp
  REDEFINEISelLowering.cpp
  REDEFINEInstrInfo.cpp
  REDEFINEMCInstLower.cpp
  REDEFINERegisterInfo.cpp
  REDEFINESubtarget.cpp
  REDEFINETargetMachine.cpp
  )

add_dependencies(LLVMREDEFINECodeGen intrinsics_gen)

add_subdirectory(AsmParser)
add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
