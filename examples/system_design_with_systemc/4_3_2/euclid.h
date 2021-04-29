
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


#ifndef EUCLID_H
#define EUCLID_H

SC_MODULE(euclid_gcd) {
    sc_in_clk		CLOCK;
    sc_in<bool>		RESET;
    sc_in<unsigned>	A, B;
    sc_out<unsigned>	C;
    sc_out<bool>	READY;

    void compute();
    SC_CTOR(euclid_gcd) {
        SC_CTHREAD(compute, CLOCK.pos());
        watching(RESET.delayed() == true);
    }
};

#endif
