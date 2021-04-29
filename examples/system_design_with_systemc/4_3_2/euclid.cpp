
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


#include "systemc.h"
#include "euclid.h"

void
euclid_gcd::compute()
{
    // reset section
    unsigned tmp_a = 0, tmp_b;

    while (true) {	// main loop

        // Here is an I/O cycle
        C.write(tmp_a);		// place output on C
        READY.write(true);	// and assert READY
        wait();

        // Another I/O cycle starts here
        tmp_a = A.read();	// sample inputs
        tmp_b = B.read();
        READY.write(false);	// lower READY
        wait();

        // No I/O takes place during the computation of GCD.
        // Computation and communication are separated
        while (tmp_b != 0) {	// Euclid's algorithm
            unsigned r = tmp_a;
            tmp_a = tmp_b;
            while (r >= tmp_b) {
                r = r - tmp_b;
            }
            tmp_b = r;
        }
    }
}
