# Timely

**Timely** is a **header-only, lightweight benchmarking and analysis toolkit for C++** designed to provide simple, low-overhead instrumentation for performance measurement, runtime tracing, and debugging.

Built with modern C++ features, Timely enables developers to instrument functions, capture execution snapshots, and trace exceptions with minimal code changes. The library focuses on **simplicity, compile-time safety, and minimal runtime overhead**, making it suitable for both development diagnostics and performance analysis.

Version: **0.0.1**  

---

## Overview

Timely provides a set of lightweight APIs and macros that allow you to:

- Benchmark function execution with automatic instrumentation
- Generate consistent hashed identifiers for code regions
- Capture stack trace snapshots
- Track runtime behavior across threads
- Handle and trace exceptions with structured diagnostics

Because Timely is **header-only**, integration requires **no linking or build system configuration**—just include the header and start instrumenting.

---

## Key Features

- **Header-Only**  
  No external dependencies or compilation steps.

- **Minimal Overhead**  
  Designed to introduce negligible runtime cost when enabled.

- **Compile-Time Hashing**  
  Function signatures can be hashed at compile time to uniquely identify execution points.

- **Function Instrumentation Macros**  
  Simple macros allow quick benchmarking of functions or code regions.

- **Thread-Aware Profiling**  
  Supports both single-thread and multi-thread instrumentation.

- **Runtime Stack Snapshots**  
  Capture stack frame traces for debugging or diagnostics.

- **Structured Exception Handling**  
  Built-in wrappers provide consistent reporting for C++ exceptions.

- **Global Enable / Disable Switch**  
  Timely can be completely disabled via a compile-time flag without modifying source logic.

---

## Enabling or Disabling Timely

Timely can be globally toggled using the `__timelyon__` flag.