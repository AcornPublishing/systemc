
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
#include "robot.h"

void
robot_controller::counter_proc()
{
    if (LDDIST.read()) {
        counter = uSEQ_BUS.read();
    } else if (COUNT.read()) {
        counter = counter - 1;
    }
    DONE.write(counter == 0);
    LSB_CNTR.write(counter[0]);
}

void
robot_controller::inst_reg_proc()
{
    if (LDINST.read()) {
        sc_bv<4>  inst = uSEQ_BUS.read();
        REPOS.write(   inst[0].to_bool());
        MAGNET.write(  inst[1].to_bool());
        XY.write(      inst[2].to_bool());
        REVERSE.write( inst[3].to_bool());
    }
}

void
robot_controller::mrdy_proc()
{
    if (CLRMRDY.read()) {
        MRDY.write(false);
    } else if (SETMRDY.read()) {
        MRDY.write(true);
    }
}

void
robot_controller::ctrl_fsm_state()
{
    curr_state.write(next_state.read());
}

void
robot_controller::ctrl_fsm()
{
    ctrl_state ns = curr_state;

    LDDIST.write(false);
    COUNT.write(false);
    LDINST.write(false);
    SETMRDY.write(false);

    if (RESET.read()) {
        ns = IDLE;
    } else {
        switch (curr_state.read()) {
        case IDLE:
            if (! MRDY.read()) {
                LDINST.write(true);
                ns = INST;
            }
            break;
        case INST:
            if (MAGNET.read()) {
                SETMRDY.write(true);
                ns = IDLE;
            } else if (REPOS.read()) {
                LDDIR.write(true);
                ns = RECAL;
            } else {
                SETMRDY.write(true);
                ns = DIST;
            }
            break;
        case RECAL:
            if (uSW_ZERO.read()) {
                SETMRDY.write(true);
                ns = IDLE;
            } else {
                COUNT.write(true);
            }
            break;
        case DIST:
            if (! MRDY.read()) {
                LDDIST.write(true);
                ns = DIR;
            }
            break;
        case DIR:
            LDDIR.write(true);
            ns = MOVE;
            break;
        case MOVE:
            if (DONE.read()) {
                SETMRDY.write(true);
                ns = IDLE;
            } else {
                COUNT.write(true);
            }
            break;
        } /* switch */
    }
    next_state.write(ns);
}

