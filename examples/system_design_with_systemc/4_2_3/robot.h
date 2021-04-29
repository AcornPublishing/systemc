
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


#ifndef ROBOT_H
#define ROBOT_H

SC_MODULE(robot_controller) {

    sc_in<bool>		CLOCK;
    sc_in<bool>		RESET;
    sc_in<sc_bv<8> >	uSEQ_BUS;
    sc_in<bool>		CLRMRDY;
    sc_in<bool>		uSW_ZERO;

    sc_inout<bool>	MRDY;
    sc_inout<bool>	REPOS;
    sc_inout<bool>	MAGNET;
    sc_out<bool>	XY;
    sc_out<bool>	REVERSE;
    sc_out<bool>	LDDIR;
    sc_out<bool>	LSB_CNTR;

    /* Internal variables and signals */
    enum ctrl_state { IDLE, INST, DIST, RECAL, DIR, MOVE };
    sc_signal<ctrl_state> curr_state, next_state;
    sc_signal<bool> DONE, LDDIST, COUNT;
    sc_uint<8> counter;
    sc_signal<bool> LDINST, SETMRDY;

    /* member function prototypes */
    void counter_proc();
    void inst_reg_proc();
    void mrdy_proc();
    void ctrl_fsm_state();
    void ctrl_fsm();

    /* Constructor */
    SC_CTOR(robot_controller) {
        SC_METHOD(counter_proc);	sensitive << CLOCK.pos();
        SC_METHOD(inst_reg_proc);	sensitive << CLOCK.pos();
        SC_METHOD(mrdy_proc);		sensitive << CLOCK.pos();
	SC_METHOD(ctrl_fsm_state);	sensitive << CLOCK.pos();
        SC_METHOD(ctrl_fsm);
            sensitive << RESET << REPOS << MAGNET
                      << DONE << uSW_ZERO << MRDY << curr_state;
    }
};
#endif
