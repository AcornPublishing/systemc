
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






// This is the header file delivered to the customer

#define SC_INCLUDE_FX
#include "systemc.h"

class secret : public sc_module
{
private:
    sc_fxtype_context _context; // must declare before ports
public:
    sc_in<sc_ufix> in1;
    sc_in<sc_ufix> in2;
    sc_out<sc_ufix> out1;

    SC_HAS_PROCESS(secret);
    
    secret(sc_module_name nm, unsigned w);

private:
    void main();
    unsigned _w;
    void *secret_data; // main() can save secret data here
};

template <unsigned W>
class wrapper : public sc_module
{
public:
    SC_HAS_PROCESS(wrapper);
    sc_in<sc_uint<W> > in1;
    sc_in<sc_uint<W> > in2;
    sc_out<sc_uint<W> > out1;

    wrapper(sc_module_name nm)
        : sc_module(nm), 
          mult1("mult1", W),
          _context(sc_fxtype_params(W, W))
    {
        SC_METHOD(in1_method);
        sensitive << in1;
        SC_METHOD(in2_method);
        sensitive << in2;
        SC_METHOD(out1_method);
        sensitive << out1_sig;
        _context.end();

        mult1.in1(in1_sig);
        mult1.in2(in2_sig);
        mult1.out1(out1_sig);
    }

private:
    secret mult1;
    sc_fxtype_context _context; // must declare before signals
    sc_signal<sc_ufix> in1_sig, in2_sig, out1_sig;

    void in1_method() 
    { sc_ufix t(W,W); t = in1.read(); in1_sig = t;}
    void in2_method() 
    { sc_ufix t(W,W); t = in2.read(); in2_sig = t;}
    void out1_method() 
    { sc_uint<W> t; t = out1_sig.read(); out1 = t; }
};

