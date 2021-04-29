
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





#include "wrapper.h"

// This is how the customer might instantiate the
// module (here named "wrapper") provided by the IP vendor

// simple stimulus generator

template <class T> class stimulus : public sc_module {
public:
    sc_out<T> out;

    SC_HAS_PROCESS(stimulus);

    stimulus(sc_module_name name) : sc_module(name) {
        SC_THREAD(main);
    }

    void main() {
        out.write(0);
        wait(10, SC_NS);
        out.write(2);
        wait(1, SC_NS);
        out.write(0);
    }
};

// simple response logger

template <class T> class response : public sc_module {
public:
    sc_in<T> in;

    SC_HAS_PROCESS(response);

    response(sc_module_name name) : sc_module(name) {
        SC_METHOD(main);
        sensitive << in;
    }

    void main() {
        cout << "at time: " << sc_time_stamp() << " output: " 
             << in.read() << endl;
    }
};


int sc_main (int argc , char *argv[])
{
    const unsigned width = 16;
    typedef sc_uint<width> type;

    sc_signal<type> mult_in;
    sc_signal<type> mult_out;

    wrapper<width> mult1("mult1");
    mult1.in1(mult_in);
    mult1.in2(mult_in);
    mult1.out1(mult_out);

    stimulus<type> stim1("stim1");
    stim1.out(mult_in);

    response<type> resp1("resp1");
    resp1.in(mult_out);

    sc_start(100, SC_NS);
    return 0;
}
