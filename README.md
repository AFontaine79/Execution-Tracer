# Execution Tracer

> **Note:** You are in the `add-python-script` branch. The Python scripts are not being actively developed here.  They are being worked on in the `test-execution-tracer` branch of my [fork](https://github.com/AFontaine79/LoRaMac-node) of the LoRaMac-node project.  TRACE macros have been added to `src/apps/ping-pong/NucleoL476/main.c`.  I am building using Semtech's CMake scripts and testing on a NucleoL476 with SX1276MB1LAS shield.  When the Python scripts are ready for initial release, they will be copied back to this project.

## Overview

Execution Tracer is a simple, minimalist, low overhead and open-source tool to help you understand the sequencing of your programs.  It is designed to be usable on any system with a minimum amount of effort and without any extra hardware.  It gives you real trace data on exactly what events occurred and in what order.  It can provide this information in real time or after the fact (e.g. from a log capture triggered by a fault).

## Motivation

Reconstructing the events that lead to a fault or understanding the real-time behavior of an embedded system can be notoriously difficult.  Off the shelf commercial solutions, such as SystemView or J-Trace are out of reach for many developers.  They are also overkill.  While I think such tools are nice, I've tackled some of the most difficult problems of my career with a much simpler solution and I wish to share it with others.

## Theory of Operation

The main concept is a trace buffer and a set of macros to write to it.  The macros must complete as fast as possible.  Decoding of the buffer is done after the fact by a backend.  There are multiple possibilities for the backend.

- Have a background thread deliver raw contents in real time to RTT, SWO or serial port.
  - Save this stream to a file for processing later.
  - Process it on the fly with a Python script.
- Rather than stream in real-time, snapshot the buffer after a crash or hard fault.
  - Save this snapshot to flash or RAM.
  - Offload the snapshot later as part of a crashlytics report.

The trace log is in a raw binary format.  Effective processing requires a MAP and possibly an SVD file.  Future work may also make use of the ELF file (with DWARF info).  This means you must preserve the build artifacts for the firmware image being analyzed.  Processing is always done off-target and is handled via cross-platform Python scripts.

## Goals

- Device and toolchain agnostic
- Development platform agnostic
- Simple to use (low learning curve)
- Lightweight enough to pull into any project
- No special equipment or software license required
- Powerful and flexible enough to be helpful for most problems
- Provide critical insight necessary for the truly difficult bugs (deadlocks, race conditions, etc.)
