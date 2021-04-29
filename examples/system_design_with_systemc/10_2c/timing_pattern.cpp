
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
// This example shows a converter (cf. section 10.2)
// that translates a value sequence into a hardware
// signal with a fixed timing pattern. In order to
// exemply this, we generate a ramp signal, transport
// the value sequence through a FIFO (sc_fifo), convert
// it into a hardware signal (sc_signal), and store this
// in a VCD trace file.
//


#include "systemc.h"


// f2s_pattern converter discussed in section 10.
template <class T> SC_MODULE(f2s_pattern)
{
    // ports
    sc_fifo_in<T> input;
    sc_in<bool> clk;
    sc_out<T> output;

    // data fields
    const int* pattern_;
    unsigned int interval_;
    unsigned int offset_;

    // we need this as we do not use SC_CTOR
    SC_HAS_PROCESS(f2s_pattern);

    // contructor w/ name, timing pattern, interval, and offset:
    f2s_pattern(sc_module_name nm, const int* pattern,
                unsigned int interval, unsigned int offset = 0)
        : sc_module(nm), pattern_(pattern),
          interval_(interval), offset_(offset) {

        // check arguments; return w/o creating process
        // in case of improper constructor arguments
        if (pattern == NULL) {
            cerr << name() << ": (ERROR) no timing pattern\n";
            return;
        }
        int max = -1;
        do { max = (max < *pattern) ? *pattern++ : -1; } 
        while(max>=0 && *pattern>=0);
        if (max==-1 || max>((int)interval_)-1) {
            cerr << name() 
                 << ": (ERROR) invalid timing pattern\n";
            return;
        }

        // create process
        SC_THREAD(main_action);
        sensitive << clk.pos();
        dont_initialize(); // we don't want the process to
        // be triggered during initialization
    }

    // main action
    void main_action() {
        for (unsigned i=0; i<offset_; i++) wait(); // initial offset

        // periodic i/o as specified by timing pattern
        for(int counter=0, index=0;;) {
            if (counter==pattern_[index]) {
                do_io();
                if (pattern_[++index] < 0) index = 0;
            }
            counter = ++counter%interval_;
            wait();
        }
    }

    // helper function: do the i/o thing
    void do_io() {
        if (input->num_available() == 0)
            cerr << name() << ": (ERROR) input FIFO empty at t="
                 << sc_time_stamp() << endl;
        else
            output = input.read();
    }
};


// Simple ramp generator.
SC_MODULE(ramp) {
    sc_fifo_out<int> output;
    int counter_;
    SC_CTOR(ramp) : counter_(0) { SC_THREAD(pump); }
    void pump() { for(;;) output.write(counter_++); }
};


int sc_main(int argc, char** argv)
{
    // channel instances
    sc_fifo<int> the_fifo("the_fifo", 10);
    sc_signal<int> the_signal("the_signal");
    sc_clock clk("clk", 1, SC_NS);

    // initialize signal
    the_signal = -1; 

    // create VCD trace file
    sc_trace_file* tf = sc_create_vcd_trace_file("trace");
    
    // trace signal
    sc_trace(tf, the_signal, "the_signal");

    // timing pattern
    // int pattern[] = { 1, 2, 3, -1}; // good
    // int pattern[] = { 1, 5, 3, -1}; // bad, not strongly monotonic
    // int pattern[] = { -1}; // bad, no pattern
    int pattern[] = { 0, 2, 3, -1}; // good, too

    // module instances
    ramp the_ramp("the_ramp");
    f2s_pattern<int> converter("converter", pattern, 10, 20);

    // connect modules and channels
    the_ramp.output(the_fifo);
    converter.clk(clk);
    converter.input(the_fifo);
    converter.output(the_signal);

    // run the simulation for 100 time steps
    sc_start(100);

    // close the trace file
    sc_close_vcd_trace_file(tf);

    return 0;
}
