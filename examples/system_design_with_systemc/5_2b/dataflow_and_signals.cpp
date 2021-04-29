
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
// This example exhibits a mix of dataflow and
// signal based communication. It uses the DF_CoeffMul
// module introduced in section 5.2.
//


#include "systemc.h" 

// This module multiplies a value sequence with a coefficient
// that is read from a (signal) input port.
template <class T> SC_MODULE(DF_CoeffMul)
{
    sc_fifo_in<T> input;
    sc_fifo_out<T> output;
    sc_in<T> coefficient;

    void process() {
        while (1) {
            output.write(input.read() * coefficient.read());
        }
    }

    SC_CTOR(DF_CoeffMul) { SC_THREAD(process); }
};

// Simple ramp generator. Update interval, initial value,
// and increment are given as constructor arguments.
template <class T> SC_MODULE(RampGen) 
{
    // ports
    sc_out<T> output;

    // processes
    void process() {
        output = state_;
        state_ += increment_;
        next_trigger(interval_);
    }

    // data
    T state_;
    T increment_;
    sc_time interval_;

    // constructor
    SC_HAS_PROCESS(RampGen);
    RampGen(sc_module_name nm,
            const sc_time& interval,
            const T& initial_value,
            const T& increment)
	: sc_module(nm), state_(initial_value), increment_(increment),
          interval_(interval)
    {
        SC_METHOD(process);
    }
};

// Simple constant generator. Works at least for builtin C types.
template <class T> SC_MODULE(DF_ConstTimed) { 

    sc_fifo_out<T> output;

    void process() { 
        while (1) {
            wait(200, SC_NS);
            output.write(constant_); 
        }
    }

    SC_HAS_PROCESS(DF_ConstTimed); // needed as we do not use SC_CTOR(.)

    // constructor w/ module name and constant
    DF_ConstTimed(sc_module_name NAME, const T& CONSTANT) :
        sc_module(NAME), constant_(CONSTANT) { SC_THREAD(process); }

    T constant_; // the constant value we write to the output
}; 

// Simple dataflow module that runs for a given number of iterations 
// (constructor argument) during which it prints the values read from
// its input on stdout. Works at least for builtin C types.
template <class T> SC_MODULE(DF_PrinterTimed) { 
    sc_fifo_in<T> input;
    SC_HAS_PROCESS(DF_PrinterTimed); // needed as we do not use SC_CTOR(.) 

    // constructor w/ name and number of iterations 
    DF_PrinterTimed(sc_module_name NAME, unsigned N_ITERATIONS) : 
        sc_module(NAME), n_iterations_(N_ITERATIONS), done_(false) 
        { SC_THREAD(process); } 

    void process() { 
        for (unsigned i=0; i<n_iterations_; i++) { 
            T value = input.read(); 
            cout << name() << " " << value
                 << " [t=" << sc_time_stamp()
                 << "]" << endl; 
        } 
        done_ = true;
        return; // terminate process after given # iterations 
    } 

    // destructor: check whether we have actually read a sufficient
    // number of values when the simulation ends.
    ~DF_PrinterTimed() { 
        if (!done_) cout << name() << " not done yet" << endl; 
    }

    unsigned n_iterations_; // number of iterations
    bool done_; // flag indicating whether we are done
}; 

int sc_main(int, char**) 
{ 
    // module instances 
    RampGen<int> ramp("ramp", sc_time(10, SC_NS), 0, 1);
    DF_ConstTimed<int> constant("constant", 1);
    DF_CoeffMul<int> coeff_mul("coeff_mul");
    DF_PrinterTimed<int> printer("printer", 10);


    // fifos 
    sc_fifo<int> const_out("const_out", 5);
    sc_fifo<int> coeff_mul_out("coeff_mul_out", 1);

    // signal
    sc_signal<int> coefficient("coefficient");

    // interconnect 
    constant.output(const_out);
    ramp.output(coefficient);
    coeff_mul.input(const_out);
    coeff_mul.output(coeff_mul_out);
    coeff_mul.coefficient(coefficient);
    printer.input(coeff_mul_out);

    // Start the simulation
    sc_start(3000, SC_NS); 

    return 0; 
}
