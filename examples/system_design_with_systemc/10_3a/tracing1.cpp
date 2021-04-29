
//****************************************************************************
//****************************************************************************
//
// Copyright (c) 2002 Thorsten Groetker, Stan Liao, Grant Martin, Stuart Swan
//
// Permission is hereby granted to use, modify, and distribute this source
// code in any way as long as this entire copyright notice is retained
// in unmodified form within the source code.
//
// This software is distributed on an "AS IS" basis, without warranty
// of any kind, either express or implied.
//
// This source code is from the book "System Design with SystemC".
// For detailed discussion on this example, see the relevant section
// within the "System Design with SystemC" book.
//
// To obtain the book and find additional source code downloads, etc., visit
//     www.systemc.org 
// Look in the "Products & Solutions" -> "SystemC Books". Then look at the
// entry for "System Design with SystemC".
//
//****************************************************************************
//****************************************************************************


//
// This simple example shows how to trace signals
// and data fields of a module. See section 10.3.
//


#include "systemc.h"


// Simple module with an internal signal and variable --
// just to give us something to trace.
SC_MODULE(some_module) {
    sc_signal<int> sig; int var; // signal and variable to be traced
    SC_CTOR(some_module) { sig = var = 0; SC_METHOD(proc); }
    void proc() { sig = - ++var; next_trigger(sc_time(10, SC_NS)); }
};


int sc_main(int argc, char** argv)
{
    // STEP 1: create trace file(s)
    sc_trace_file* trace_file = sc_create_vcd_trace_file("trace");
    // module and channel instances
    some_module XYZ("XYZ");
    // STEP 2: register signals and variables to be traced
    sc_trace(trace_file, XYZ.sig, "XYZ.sig");
    sc_trace(trace_file, XYZ.var, "XYZ.var");
    // run the simulation
    sc_start(sc_time(500, SC_NS));
    // STEP 3: close the trace file(s)
    sc_close_vcd_trace_file(trace_file);

    return 0;
}
