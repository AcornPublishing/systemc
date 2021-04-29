
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
// This example depicts the use of the DF_Terminator
// module to terminate a dataflow simulation. (See
// section 5.3.)
//


#include "systemc.h" 


// Simple dataflow module that runs for a given number of
// iterations (template argument) during which it prints
// the values read from its input on stdout.
template <class T, unsigned n_iterations> SC_MODULE(DF_Printer) {
    sc_fifo_in<T> input;
    sc_out<bool> done;

    SC_CTOR(DF_Printer) {
        SC_THREAD(process);
        done.initialize(false);
    }

    void process() {
        for (unsigned i=0; i<n_iterations; i++) {
            cout << name() << " " << input.read() << endl;
        }
        done = true;
        while (1) input.read(); // avoid data backlog
    }
};


// This module terminates the simulation when all signals
// connected to its input are set to 'true'.
SC_MODULE(Terminator) {
    sc_port<sc_signal_in_if<bool>, 0> inputs;
    SC_CTOR(Terminator) { SC_METHOD(arnold); sensitive << inputs; }
    void arnold() { // terminate when all inputs are 'true'
        for (int i=0; i<inputs.size(); i++)
            if (inputs[i]->read() == false) return;
        sc_stop(); // we only get to this point if all inputs are 'true'
    }
};


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
    DF_Printer<int, 20> printer("printer");
    // more module instances
    DF_Const<int> constant2("constant2", 1);
    DF_Adder<int> adder2("adder2"); 
    DF_Fork<int> fork2("fork2");
    DF_Printer<int, 10> printer2("printer2");

    // NB: printer runs for 20 iterations, printer2
    //     for 10 (see above).

    // fifos
    sc_fifo<int> const_out("const_out", 5); 
    sc_fifo<int> adder_out("adder_out", 1);
    sc_fifo<int> feedback("feedback", 1);
    sc_fifo<int> to_printer("to_printer", 1);
    // more fifos
    sc_fifo<int> const_out2("const_out2", 5); 
    sc_fifo<int> adder_out2("adder_out2", 1);
    sc_fifo<int> feedback2("feedback2", 1);
    sc_fifo<int> to_printer2("to_printer2", 1);

    // initial values
    feedback.write(42);  // forget about this and the
    feedback2.write(42); // system will deadlock

    // interconnect 
    constant.output(const_out);
    adder.input1(const_out);
    adder.input2(feedback);
    adder.output(adder_out);
    fork.input(adder_out);
    fork.output1(feedback);
    fork.output2(to_printer);
    printer.input(to_printer);
    // and the same thing again
    constant2.output(const_out2);
    adder2.input1(const_out2);
    adder2.input2(feedback2);
    adder2.output(adder_out2);
    fork2.input(adder_out2);
    fork2.output1(feedback2);
    fork2.output2(to_printer2);
    printer2.input(to_printer2);

    // termination
    sc_signal<bool> done, done2;
    printer.done(done);
    printer2.done(done2);
    Terminator arnie("arnie");
    arnie.inputs(done);
    arnie.inputs(done2);

    sc_start(-1); 

    return 0; 
} 
