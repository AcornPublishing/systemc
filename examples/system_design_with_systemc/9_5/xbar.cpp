
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
// Example using the simple abstract crossbar model
// introduced in section 9.5.
//


#include "systemc.h"

// Simple abstract crossbar model introduced in section 9.5.

template <class T> SC_MODULE(XBAR) {

    // ports
    sc_port<sc_fifo_in_if<T>, 0> inputs; // arbitrary # channels
    sc_port<sc_fifo_out_if<T>, 0> outputs; // ditto: outputs
    sc_in_clk clk; // the, uh, clock

    // data
    unsigned last_; // index of last channel granted access

    // constructor
    SC_CTOR(XBAR) : last_(-1) {
        SC_METHOD(process);
        sensitive << clk.pos();
    }

    // processes
    void process() {
        // same # channels required
        assert(inputs.size() == outputs.size()); 
        // Iterate over all ports starting with the one that
        // comes after the one that was granted access last time.
        // Transmit data if both data available at the input and
        // space available at the corresponding output.
        for (int i=1; i<=inputs.size(); i++) {
            int current = (last_ + i) % inputs.size();
            //if (inputs[current]->data_available() &&
            if (inputs[current]->num_available() &&
                //outputs[current]->space_available()) {
                outputs[current]->num_free()) {
                outputs[current]->write(inputs[current]->read()); 
                last_ = current; // store info and we're done
                break;
            }
        }
    }
};


// Simple ramp generator. Works at least for builtin C types.
// Initial value (default: 0.0) and increment (default: 1.0)
// can be specified as constructor arguments.

template <class T> SC_MODULE(DF_Ramp) { 

    // ports
    sc_fifo_out<T> output;

    // data
    T counter_;
    T increment_;

    // constructors
    SC_HAS_PROCESS(DF_Ramp);
    DF_Ramp(sc_module_name nm,
            const T& initial_value,
            const T& increment)
        : sc_module(nm),  counter_(initial_value), increment_(increment) 
    { 
        SC_THREAD(process);
    }

    // processes
    void process() { 
        while (1) {
            output.write(counter_); 
            counter_ += increment_;
        }
    }
}; 


// Simple dataflow module that runs for a given number of iterations 
// (constructor argument) during which it prints the values read from
// its input on stdout. Works at least for builtin C types.

template <class T> SC_MODULE(DF_Printer) { 
    sc_fifo_in<T> input;
    SC_HAS_PROCESS(DF_Printer); // needed as we do not use SC_CTOR(.) 

    // constructor w/ name and number of iterations 
    DF_Printer(sc_module_name NAME, unsigned n_iterations) : 
        sc_module(NAME), n_iterations_(n_iterations) 
        { 
            SC_THREAD(process); 
        } 

    void process() { 
        for (unsigned i=0; i<n_iterations_; i++) { 
            T value = input.read(); 
            cout << name() << " " << value
                 << " [t=" << sc_time_stamp()
                 << "]" << endl; 
        } 
        return; // terminate process after given # iterations 
    } 

    unsigned n_iterations_; // number of iterations
}; 


int sc_main(int argc, char* argv[])
{
    // module instantiations
    DF_Ramp<int>    R1("R1", 0.0 /*initial value*/, 2.0 /*increment*/);
    DF_Ramp<int>    R2("R2", 1.0 /*initial value*/, 2.0 /*increment*/);
    XBAR<int>       X("X");
    DF_Printer<int> P1("P1", 10 /*# iterations*/);
    DF_Printer<int> P2("P2", 20 /*# iterations*/);

    // channel instances
    // NB: Changing the FIFO size will lead to different
    //     output (same value sequences but values come out
    //     at different points in time).
    sc_fifo<int> r1_to_x("r1_to_x", 5 /*size*/);
    sc_fifo<int> r2_to_x("r2_to_x", 5 /*size*/);
    sc_fifo<int> x_to_p1("x_to_p1", 5 /*size*/);
    sc_fifo<int> x_to_p2("x_to_p2", 5 /*size*/);
    sc_clock clk("clk", sc_time(10, SC_NS) /*period*/);

    // port-channel connections
    R1.output(r1_to_x);
    R2.output(r2_to_x);
    // ATTN: The sequence in which channels are connected to
    // X.inputs and X.outputs matters! The N-th input is routed
    // to the N-th output.
    X.inputs(r1_to_x);
    X.inputs(r2_to_x);
    X.outputs(x_to_p1);
    X.outputs(x_to_p2);
    X.clk(clk);
    P1.input(x_to_p1);
    P2.input(x_to_p2);

    // start the simulation without a time limit
    sc_start(500, SC_NS);

    return 0;
}
