
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
// This example depicts the use of the hs_checker
// module introduced in section 10.2.
//


#include "systemc.h"

SC_MODULE(toggle) {
    sc_in<bool> clk;
    sc_out<bool> output;
    bool state;
    void toggle_it() {
        output = state = !state;
    }
    SC_CTOR(toggle) {
        state = true;
        SC_METHOD(toggle_it);
        sensitive << clk.pos();
    }
};

// We make this a class template such that the data signal
// can have any type.
template <class T> SC_MODULE(hs_checker) {
    sc_in<T> data;
    sc_in<bool> data_valid;

    void check_it() {
        // This process is triggered only if the data changes.
        // It complains about glitches if data changes
        // while data_valid is asserted. We have to excuse
        // the case where data_valid just changed itself.
        if (data_valid.read() && !data_valid.event())
            cerr << name() << ": glitch at t=" 
                 << sc_simulation_time() << endl;
    }

    SC_CTOR(hs_checker) { 
        SC_METHOD(check_it); 
        sensitive << data; 
    }
};


int sc_main(int argc, char** argv)
{
    sc_signal<bool> data("data");
    sc_signal<bool> data_valid("data_valid");

    sc_clock data_clock("data_clock", 10);
    sc_clock data_valid_clock("data_valid_clock", 100);

    toggle toggle_data("toggle_data");
    toggle toggle_data_valid("toggle_data_valid");

    toggle_data.clk(data_clock);
    toggle_data.output(data);

    toggle_data_valid.clk(data_valid_clock);
    toggle_data_valid.output(data_valid);

    hs_checker<bool> checker("checker");

    checker.data(data);
    checker.data_valid(data_valid);

    sc_start(1000);

    return 0;
}
