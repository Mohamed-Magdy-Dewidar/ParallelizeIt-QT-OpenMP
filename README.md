🚀 ParallelizeIt-QT-OpenMP
ParallelizeIt-QT-OpenMP is a Qt-based desktop application that automatically transforms sequential C/C++ code into parallelized versions using OpenMP directives. It is built to help developers optimize their code with minimal manual effort.


![Main Interface](images/main_ui.png)


🏗️ Project Structure

ParallelizeIt-QT-OpenMP/
├── CMakeLists.txt             # Build configuration
├── README.md                  # Project documentation
├── images/                    # Visual assets
│   ├── architecture.png       # System diagram
│   ├── main_ui.png            # UI screenshot
│   └── handling_dataRace.png  # Race condition example
├── src/                       # Core application
│   ├── main.cpp               # Entry point
│   ├── mainwindow.{h,cpp}     # GUI controller
│   └── parallelizer/
│       ├── parallelizer.{h,cpp} # Core transformation logic
│       └── analyzer.{h,cpp}     # Code analysis (future feature)
└── ui/
    └── mainwindow.ui          # Qt Designer UI layout


💡 Key Features
🔧 Core Functionality
Automatic OpenMP Directive Insertion:

#pragma omp parallel for

Critical section protection

Reduction operations (reduction(+:sum), etc.)

Handles basic race conditions using appropriate OpenMP constructs

🖥️ User Interface
Dual-panel code view (original and parallelized)

File upload support for .cpp, .h, and .c

Light and dark theme support

Responsive features:

Line numbers

Syntax highlighting

Zoom controls

🧠 Advanced Capabilities
Basic race condition detection

Loop dependency analysis

Performance estimation (WIP)

🔍 Race Condition Handling Example
![](images/handling_dataRace.png)
This screenshot demonstrates how the tool detected a race condition and applied the appropriate OpenMP directive using reduction(+:sum) to safely parallelize the code.


🧠 System Architecture
graph LR
    A[User Interface] -->|Upload| B[Input Handler]
    B --> C[Code Parser]
    C --> D[AST Generator]
    D --> E[Parallelizer]
    E --> F[OpenMP Transformer]
    F --> G[Output Generator]
    G --> H[Result Display]
    
    style A fill:#5e81ac,stroke:#333
    style B fill:#8fbcbb,stroke:#333
    style C fill:#88c0d0,stroke:#333
    style D fill:#d08770,stroke:#333
    style E fill:#ebcb8b,stroke:#333
    style F fill:#a3be8c,stroke:#333
    style G fill:#b48ead,stroke:#333
    style H fill:#5e81ac,stroke:#333

⚙️ Build & Run
✅ Requirements
Component	Version	Notes
Qt	≥ 6.6	Qt 6 recommended
C++ Compiler	≥ C++17	GCC, Clang, or MSVC
OpenMP	≥ 4.5	Required for parallelism
CMake	≥ 3.15	Cross-platform build tool

🧪 Build Instructions

# Clone and build
git clone https://github.com/yourusername/ParallelizeIt-QT-OpenMP.git
cd ParallelizeIt-QT-OpenMP

open the .pro file using QT Creator and the run the Project
