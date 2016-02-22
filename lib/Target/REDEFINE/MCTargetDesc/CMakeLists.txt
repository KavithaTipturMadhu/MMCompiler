add_llvm_library(LLVMRISCVDesc
  REDEFINEMCAsmBackend.cpp
  REDEFINEMCAsmInfo.cpp
  REDEFINEMCCodeEmitter.cpp
  REDEFINEMCObjectWriter.cpp
  REDEFINEMCTargetDesc.cpp
  )

add_dependencies(LLVMREDEFINEDesc REDEFINECommonTableGen)
