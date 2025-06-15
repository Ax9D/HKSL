cmake_minimum_required(VERSION 3.30)
project(HKSL)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include include/AST include/Parse include/Analysis include/Codegen)

file(GLOB all_sources CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/AST/*.cpp"
    "src/Analysis/*.cpp"
    "src/Parse/*.cpp"
    "src/Codegen/*.cpp"
)

find_package(MLIR REQUIRED CONFIG)

message(STATUS "Using MLIRConfig.cmake in: ${MLIR_DIR}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(TableGen)
include(AddLLVM)
include(AddMLIR)
include(HandleLLVMOptions)

# Include directories
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${MLIR_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_subdirectory(include/Codegen/MLIR)

get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)
get_property(extension_libs GLOBAL PROPERTY MLIR_EXTENSION_LIBS)

add_executable(${PROJECT_NAME} ${all_sources})
add_dependencies(${PROJECT_NAME} OpsIncGen)
# add_dependencies(${PROJECT_NAME} ToyCombineIncGen)
# add_dependencies(${PROJECT_NAME} ShapeInferenceInterfaceIncGen)

llvm_update_compile_flags(${PROJECT_NAME})

target_link_libraries(
${PROJECT_NAME}
  PRIVATE ${dialect_libs}
          ${conversion_libs}
          ${extension_libs}
          MLIRAnalysis
          MLIRBuiltinToLLVMIRTranslation
          MLIRIR
          MLIRExecutionEngine
          MLIRParser
          MLIRPass
          MLIRLLVMCommonConversion
          MLIRLLVMDialect
          MLIRLLVMToLLVMIRTranslation
          MLIRTargetLLVMIRExport
          MLIRMemRefDialect
          MLIRFunctionInterfaces
          MLIRSideEffectInterfaces
          MLIRTransforms
          MLIRCastInterfaces)

# Disable warnings
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-covered-switch-default)
mlir_check_link_libraries(${PROJECT_NAME})
