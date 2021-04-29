
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
// Untimed function models communicating over a
// FIFO channel. This example is the starting point
// of the HW-HW communication refinement process
// detailed in section 9.3.
//


#include "systemc.h"


// A simple dataflow source.
SC_MODULE(SOURCE) {

    // output port
    sc_fifo_out<int> output;

    // the one and only process
    void my_process() {
        int counter = 0;
        while(1) {
            counter++; // placeholder for decent functionality
            output->write(counter);
        }
    }

    // constructor
    SC_CTOR(SOURCE) {
        SC_THREAD(my_process); // declare process
    }
};


// A simple dataflow sink. It consumes input tokens
// and prints their value on stdout. After 100 tokens
// it stops consuming data.
SC_MODULE(SINK) {

    // input port
    sc_fifo_in<int> input;

    // the one and only process
    void process() {
        // consume 100 samples and return
        for (int i = 0; i < 100; i++) {
            cout << name() << ": read " << input.read()
                 << " [t=" << sc_time_stamp() << "]"
                 << endl;
        }
        return;
    }

    // constructor
    SC_CTOR(SINK) {
        SC_THREAD(process);
    }

};

int sc_main(int argc, char* argv[])
{
    // module instances
    SOURCE source("source");
    SINK   sink("sink");

    // fifo channel
    sc_fifo<int> fifo("fifo", 10);

    // interconnect
    source.output(fifo);
    sink.input(fifo);

    // start the simulation
    sc_start(-1);

    return 0;
}
