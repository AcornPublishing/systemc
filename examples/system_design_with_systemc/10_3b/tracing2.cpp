
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
// This example shows how to trace
// - signals
// - ports
// - data fields of a module
// - user-defined aggregate data types
// - vectors
// - enumerated types
// (See section 10.3)
//


#include "systemc.h"

// --------------------------------------------------------
// Tracing signals, ports and data members
// (cf. section 10.3)
// --------------------------------------------------------

// This module generates a ramp signal.
SC_MODULE(ramp) {
    sc_out<int> output;
    SC_CTOR(ramp) { SC_THREAD(proc); }
    void proc() {
        int value = 0;
        while (true) { 
            output = value++; 
            wait(sc_time(10, SC_NS)); }
    }
};


// A dummy module. It's only prupose is to provide
// us with some ports, signals, and data fields that
// we can trace.
SC_MODULE(my_module)
{
    // stuff to trace: ports, signals, data fields
    sc_in<int> input;
    sc_out<int> output;
    sc_inout<int> foobar; // same as sc_out anyways
    sc_in_clk clk; // same as sc_in<bool>
    sc_signal<int> local_sig;
    int state;
    // constructor and other methods
    SC_CTOR(my_module) { 
        local_sig = state = 0;
        SC_METHOD(proc); sensitive << clk.pos(); 
    }
    void proc() {
        state = input + 1;
        output = state; foobar = state; local_sig = state;
    }
};


// --------------------------------------------------------
// Tracing user-defined aggregate data-types
// (cf. section 10.3)
// --------------------------------------------------------

struct data {
    unsigned char raw;
    bool parity;
    bool valid;
    bool operator==(const data& other) {
        return other.raw==raw && other.parity==parity && other.valid==valid;
    }
};

ostream& operator<<(ostream& o, const data& d) {
    return o << d.raw << '/' << d.parity << '/' << d.valid;
}

void sc_trace(sc_trace_file* tf, const data& d, const sc_string& name)
{
    sc_trace(tf, d.raw, name    + ".raw");
    sc_trace(tf, d.parity, name + ".parity");
    sc_trace(tf, d.valid, name  + ".valid");
}

// --------------------------------------------------------
// Support for tracing vectors
// (cf. section 10.3)
// --------------------------------------------------------

template <class T> 
void sc_trace_vector(sc_trace_file* tf, const T x[],
                     const sc_string& name, int length)
{
    for (int i=0; i<length; i++)
        sc_trace(tf, x[i], name + "." + sc_string::to_string("%d", i));
}



// --------------------------------------------------------
// Tracing enumerated types
// (cf. section 10.3)
// --------------------------------------------------------

enum status {
    idle=0, busy=1, sleeping=2, suspended=3
};
const char* status_as_string[] = { "idle", "busy", "sleeping",
                                   "suspended", NULL };


// --------------------------------------------------------


int sc_main(int argc, char** argv)
{
    // ----------------------------------------------------
    // create trace files
    // ----------------------------------------------------

    sc_trace_file* trace_file = sc_create_vcd_trace_file("trace");
    sc_trace_file* wif_trace_file = sc_create_wif_trace_file("trace");

    // ----------------------------------------------------
    // Tracing signals, ports and data members
    // ----------------------------------------------------

    // channels
    sc_signal<int> sig1("sig1");
    sc_signal<int> sig2("sig2");
    sc_signal<int> sig3("sig3");
    sc_clock clk("clk");

    // modules
    ramp driver1("driver1");
    my_module abc("abc");
 
    // interconnect
    driver1.output(sig1);
    abc.input(sig1);
    abc.output(sig2);
    abc.foobar(sig3);
    abc.clk(clk);

    // tracing
    sc_trace(trace_file, abc.input,     "abc.input");
    sc_trace(trace_file, abc.output,    "abc.output");
    sc_trace(trace_file, abc.foobar,    "abc.foobar");
    sc_trace(trace_file, abc.clk,       "abc.clk");
    sc_trace(trace_file, abc.local_sig, "abc.local_sig");
    sc_trace(trace_file, abc.state,     "abc.state");

    // For signed/unsigned char/int/long we can also specify
    // a bit-width. Here, we just take lowest 2 bits.
    sc_trace(trace_file, abc.input,     "abc.input2",     2);
    sc_trace(trace_file, abc.output,    "abc.output2",    2);
    sc_trace(trace_file, abc.foobar,    "abc.foobar2",    2);
    sc_trace(trace_file, abc.local_sig, "abc.local_sig2", 2);
    sc_trace(trace_file, abc.state,     "abc.state2",     2);

    // ----------------------------------------------------
    // Tracing user-defined aggregate data-types
    // ----------------------------------------------------

    // signal and variable to be traced
    data my_data = { 42, true, true};
    sc_signal<data> my_data_sig; 
    my_data_sig = my_data; // initialize signal

    // tracing
    sc_trace(trace_file, my_data, "my_data");
    sc_trace(trace_file, my_data_sig, "my_data_sig");

    // ----------------------------------------------------
    // Tracing vectors
    // ----------------------------------------------------

    // Just to prove that it works: let's use arrays of
    // variables and signals of user-defined data-types
    data data_array[4];
    sc_signal<data> data_sig_array[4];
    // initialize variables and signals
    for (int k=0; k<3; k++) {
        data_array[k] = my_data;
        data_sig_array[k] = my_data;
    }

    // tracing
    sc_trace_vector(trace_file, data_array, "data_array", 4);
    sc_trace_vector(trace_file, data_sig_array, "data_sig_array", 4);

    // ----------------------------------------------------
    // Tracing enumerated types
    // ----------------------------------------------------

    // Signal and variable using an enumerated type
    sc_signal<unsigned> status_sig("status_sig");
    // Tracing will fail on HPUX (aCC can't find an appropriate
    // sc_trace(.) function) if the signal is declared as
    //     sc_signal<status> status_sig("status_sig");
    status_sig = busy; // initialize signal
    status status_var = idle;

    // tracing
    sc_trace(wif_trace_file, status_sig, 
             "status_sig", status_as_string);
    sc_trace(wif_trace_file, status_var,
             "status_var", status_as_string);

    // ----------------------------------------------------
    // run the simulation
    // ----------------------------------------------------

    sc_start(sc_time(500, SC_NS));

    // ----------------------------------------------------
    // close the trace file(s)
    // ----------------------------------------------------

    sc_close_vcd_trace_file(trace_file);
    sc_close_wif_trace_file(wif_trace_file);

    return 0;
}

