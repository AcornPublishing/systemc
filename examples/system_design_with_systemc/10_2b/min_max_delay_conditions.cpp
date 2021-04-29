
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
// This example shows the min_max_checker module 
// presented in section 10.2.
//


#include "systemc.h"


// This module checks the relative timing of two signals.
// The second signal is supposed to have a rising edge in
// the interval [min_time, max_time] after the first signal
// went from low to high. Protocol violations are reported
// on stderr.
SC_MODULE(min_max_checker) {
    sc_in<bool> first, second; // first and second input

    // we need this because we do not use SC_CTOR
    SC_HAS_PROCESS(min_max_checker);

    sc_time min_time_, max_time_;  // local data: min and max delays

    // constructor with name, min, and max delays
    min_max_checker(sc_module_name name,
                    const sc_time& min_time, 
                    const sc_time& max_time)
        : sc_module(name), 
          min_time_(min_time), 
          max_time_(max_time) 
        {
            SC_THREAD(check_it);
        }

    // helper function; prints error messages
    void print_error(const char* message) {
        cerr << name() << ": protocol violation at t="
             << sc_simulation_time() << " ("
             << message << ")" << endl;
    }

    // the one and only process
    void check_it() {
        while (true) {
            // wait for rising edge of first signal
            wait(first->posedge_event());

            if (second.read()) {
                print_error("second signal is already high");
                continue;
            }

            sc_time before = sc_time_stamp();

            // wait for either signal to change,
            // wait at most max_time_
            wait(max_time_, first->value_changed_event() | 
                 second->value_changed_event());

            if (first.event()) {
                print_error("first signal went low too early");
                continue;
            }

            if (!second.event())
                print_error("max delay overrun");

            sc_time delta = sc_time_stamp() - before;

            if (delta < min_time_)
                print_error("min delay underrun");
        }
    }
};


// This module generates a Boolean waveform. The waveform
// is specified by a start value and an array of sc_time
// objects (both are given as constructor arguments). The
// end of the array is indicated by an element with t=0.
// It must contain at least one non-zero element.
SC_MODULE(bool_wave_gen) {

    // ports
    sc_out<bool> output;

    // data
    sc_time* w_; // waveform, must be non-NULL, last element
                 // is indicated by T=0, must contain at least
                 // one non-zero element
    bool state_;

    // processes
    void toggle_it() {
        for (;;) {
            for (int i=0;;i++) {
                if (w_[i].value() == 0)
                    break; // repeat waveform
                wait(w_[i]);
                output = state_ = !state_;
                cout << name() << ": output=" << state_  << " at t="
                     << sc_time_stamp() << endl;
            }
        }
    }

    // constructor w/ waveform and start value
    SC_HAS_PROCESS(bool_wave_gen);
    bool_wave_gen(sc_module_name name,
                  sc_time waveform[],
                  bool start_value)
        : sc_module(name), w_(waveform), state_(start_value) 
        {
            SC_THREAD(toggle_it);
        }
};


int sc_main(int argc, char** argv)
{
    sc_signal<bool> first("first");
    sc_signal<bool> second("second");

    sc_time first_waveform[] = {
        sc_time( 10, SC_NS ), // t=10
        sc_time( 10, SC_NS ),

        sc_time( 10, SC_NS ), // t=30
        sc_time( 10, SC_NS ),

        sc_time( 10, SC_NS ), // t=50
        sc_time( 10, SC_NS ),

        sc_time( 10, SC_NS ), // t=70
        sc_time( 10, SC_NS ),

        sc_time( 10, SC_NS ), // t=90
        sc_time(  2, SC_NS ), // goes down too early

        sc_time( 18, SC_NS ),
        sc_time( 10, SC_NS ),

        sc_time( 0, SC_NS )
    };


    sc_time second_waveform[] = {
        sc_time( 13, SC_NS ), // OK
        sc_time(  1, SC_NS ),

        sc_time( 17, SC_NS ), // too early (31ns)
        sc_time(  1, SC_NS ),

        sc_time( 24, SC_NS ), // too late (56ns)
        sc_time(  1, SC_NS ),

        sc_time(  1, SC_NS ), // already high (58ns)
        sc_time( 22, SC_NS ), // -> back to normal

        sc_time( 14, SC_NS ), // OK (94ns)
        sc_time(  6, SC_NS ),

        sc_time( 0, SC_NS )
    };

    bool_wave_gen first_wave("first_wave", first_waveform, false);
    bool_wave_gen second_wave("second_wave", second_waveform, false);

    min_max_checker the_checker("the_checker", sc_time(2, SC_NS), 
                                sc_time(5, SC_NS));

    first_wave.output(first);
    second_wave.output(second);

    the_checker.first(first);
    the_checker.second(second);

    sc_start(500);

    return 0;
}
