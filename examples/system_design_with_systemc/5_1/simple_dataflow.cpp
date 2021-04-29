
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
// Simple dataflow example introduced in section 5.1.
//


#include "systemc.h" 


// Simple constant generator. Works at least for builtin C types.
template <class T> SC_MODULE(DF_Const) { 
    sc_fifo_out<T> output;
    void process() { while (1) output.write(constant_); }
    SC_HAS_PROCESS(DF_Const); // needed as we do not use SC_CTOR(.)

    // constructor w/ module name and constant
    DF_Const(sc_module_name NAME, const T& CONSTANT) :
        sc_module(NAME), constant_(CONSTANT) { SC_THREAD(process); }

    T constant_; // the constant value we write to the output
}; 


// Simple dataflow adder. Works at least for builtin C types.
template <class T> SC_MODULE(DF_Adder) { 
    sc_fifo_in<T> input1, input2;
    sc_fifo_out<T> output;
    void process() { while (1) output.write(input1.read() + input2.read()); } 
    SC_CTOR(DF_Adder) { SC_THREAD(process); } 
}; 


// Simple dataflow module that runs for a given number of iterations 
// (constructor argument) during which it prints the values read from
// its input on stdout. Works at least for builtin C types.
template <class T> SC_MODULE(DF_Printer) { 
    sc_fifo_in<T> input;
    SC_HAS_PROCESS(DF_Printer); // needed as we do not use SC_CTOR(.) 

    // constructor w/ name and number of iterations 
    DF_Printer(sc_module_name NAME, unsigned N_ITERATIONS) : 
        sc_module(NAME), n_iterations_(N_ITERATIONS), done_(false) 
        { SC_THREAD(process); } 

    void process() { 
        for (unsigned i=0; i<n_iterations_; i++) { 
            T value = input.read(); 
            cout << name() << " " << value << endl; 
        } 
        done_ = true;
        return; // terminate process after given # iterations 
    } 

    // destructor: check whether we have actually read a sufficient
    // number of values when the simulation ends.
    ~DF_Printer() { 
        if (!done_) cout << name() << " not done yet" << endl; 
    }

    unsigned n_iterations_; // number of iterations
    bool done_; // flag indicating whether we are done
}; 


// This module forks a dataflow stream.
template <class T> SC_MODULE(DF_Fork) {
    sc_fifo_in<T> input;
    sc_fifo_out<T> output1, output2;
    void process() {
        while(1) {
            T value = input.read();
            output1.write(value);
            output2.write(value);
        }
    }
    SC_CTOR(DF_Fork) { SC_THREAD(process); }
};


int sc_main(int, char**) 
{ 
    // module instances 
    DF_Const<int> constant("constant", 1);
    DF_Adder<int> adder("adder"); 
    DF_Fork<int> fork("fork");
    DF_Printer<int> printer("printer", 10);


    // fifos 
    sc_fifo<int> const_out("const_out", 5); 
    sc_fifo<int> adder_out("adder_out", 1);
    sc_fifo<int> feedback("feedback", 1);
    sc_fifo<int> to_printer("to_printer", 1);

    // initial values
    feedback.write(42); // forget about this and the
                        // system will deadlock

    // interconnect 
    constant.output(const_out);
    adder.input1(const_out);
    adder.input2(feedback);
    adder.output(adder_out);
    fork.input(adder_out);
    fork.output1(feedback);
    fork.output2(to_printer);
    printer.input(to_printer);

    // Start simulation w/o time limit. The simulation will stop
    // when there are not more events. Once the printer module
    // has terminated the complete  simulation will eventually
    // come to a halt (after all fifos have been filled up) 
    sc_start(-1); 

    return 0; 
}
