# 🚀 RISC-V CPU Simulator

A RISC-V CPU simulator implementing the Tomasulo algorithm for out-of-order execution and dynamic scheduling, written in C++.

## 📖 Project Overview

This project implements a RISC-V RV32I instruction set CPU simulator using the Tomasulo algorithm. The simulator supports:

- ⚡ **Out-of-Order Execution**: Instruction-level parallelism through Tomasulo algorithm
- 🔄 **Dynamic Scheduling**: Automatic resolution of data dependencies and resource conflicts
- 🎯 **Branch Prediction**: Improved performance for branch instructions

- 🔧 **Pipelined Execution**: Five-stage instruction pipeline (Fetch → Issue → Execute → Write & Broadcast → Commit)

## 📂 Project Structure

```
RISC-V-CPU-Simulator/
├── Headers/                   
│   ├── ALU.hpp                
│   ├── CPU.hpp               
│   ├── Decoder.hpp           
│   ├── LSB.hpp                
│   ├── Memory.hpp            
│   ├── Predictor.hpp         
│   ├── RegisterFile.hpp      
│   ├── ROB.hpp             
│   └── RS.hpp                 
├── main.cpp                
├── CMakeLists.txt         
└── README.md               
```

## 🏗️ Core Components

### 1. 🎯 Tomasulo Algorithm Components

- **🔄 ROB (Reorder Buffer)**: Ensures in-order instruction commitment
- **🎫 RS (Reservation Station)**: Maintains instruction dependencies, enables out-of-order execution
- **📦 LSB (Load Store Buffer)**: Dedicated buffer for memory access instructions
- **📋 Register File**: Register file with register renaming support

### 2. 🔧 Execution Units

- **🧮 ALU**: Arithmetic Logic Unit for computational instructions
- **💾 Memory**: Memory system with 3-cycle access latency simulation
- **🔍 Decoder**: Instruction decoder for RISC-V instruction parsing

### 3. 🎮 Control Units

- **🖥️ CPU**: Main controller coordinating all components
- **🔮 Predictor**: Branch predictor for improved branch instruction performance

## 📝 Supported Instruction Set

Implements 37 instructions from the RISC-V RV32I base integer instruction set, including:

- **🧮 Arithmetic Instructions**: ADD, ADDI, SUB, AND, OR, XOR, SLL, SRL, SRA, etc.
- **💾 Memory Instructions**: LW, LH, LB, LBU, LHU, SW, SH, SB
- **🔄 Jump Instructions**: JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU
- **🔢 Immediate Instructions**: LUI, AUIPC

## 📥 Input Format

The simulator reads binary machine instructions from standard input:
- Each instruction is 4 bytes in little-endian format 🔄
- Program execution starts from memory address 0x0000 📍
- Terminates when encountering instruction `0x0ff00513` (li a0, 255) and outputs return value 🏁

## ⭐ Design Features

### 1. 🔧 Hardware Simulation
- Strictly follows hardware timing design with parallel component operation ⚡
- Uses double buffering to simulate clock-synchronized register updates 🕰️
- Avoids STL containers and dynamic memory allocation 🚫

### 2. ⚠️ Hazard Handling
- **📖➡️✏️ RAW (Read After Write)**: Resolved through forwarding and stalling
- **✏️➡️✏️ WAW (Write After Write)**: Resolved through register renaming  
- **📖➡️✏️ WAR (Write After Read)**: Resolved through register renaming
- **🏗️ Structural Hazards**: Resolved through resource scheduling
- **🎯 Control Hazards**: Resolved through branch prediction and pipeline flushing

### 3. 🚀 Performance Optimizations
- Branch prediction reduces control hazard impact 🎯
- Out-of-order execution improves instruction-level parallelism ⚡
- Forwarding techniques reduce data hazard delays 🔄


## 📊 Performance Statistics

The simulator tracks and outputs:
- Total execution cycles ⏱️
- Branch prediction accuracy 🎯
- Number of committed instructions 📈
- Distribution of instruction types 📊

## 👤 Author

GranthyGu, independently.

## 🫶 Acknowledgement

Acuticalamum, Jane.Z, Yuchan.