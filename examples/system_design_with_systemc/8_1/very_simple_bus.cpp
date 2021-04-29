
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





#include <systemc.h>

class very_simple_bus_if: virtual public sc_interface
{
public:
  virtual void burst_read (char *data, 
                           unsigned addr, 
                           unsigned length) = 0;
  virtual void burst_write(char *data, 
                           unsigned addr, 
                           unsigned length) = 0;
};

class very_simple_bus 
  : public very_simple_bus_if, public sc_channel
{
public:
  very_simple_bus(sc_module_name nm, unsigned mem_size, 
     sc_time cycle_time) : sc_channel(nm), _cycle_time(cycle_time) 
  {
    // we model bus memory accesses using an embedded memory array
    _mem = new char [mem_size];

    // set initial value of memory to zero
    memset(_mem, 0, mem_size);
  }

  ~very_simple_bus() { delete [] _mem; }

  virtual void burst_read(char *data, unsigned addr, 
                          unsigned length)
  {
    // model bus contention using a mutex, but no arbitration rules
    _bus_mutex.lock();  

    cout << "read starts at: " << sc_time_stamp() << endl;

    // block the caller for length of burst transaction
    wait(length * _cycle_time);  

    //  copy the data from memory to requestor
    memcpy(data, _mem + addr, length); 

    cout << "read ends at: " << sc_time_stamp() << endl;

    // unlock the mutex to allow others access to the bus
    _bus_mutex.unlock();
   }

  virtual void burst_write(char *data, unsigned addr, 
                           unsigned length)
  {
    _bus_mutex.lock();

    cout << "write starts at: " << sc_time_stamp() << endl;

    wait(length * _cycle_time);

    //  copy the data from requestor to the memory
    memcpy(_mem + addr, data, length);

    cout << "write ends at: " << sc_time_stamp() << endl;

    _bus_mutex.unlock();
  }

protected:
  char* _mem;
  sc_time _cycle_time;
  sc_mutex _bus_mutex;
};


class writer : public sc_module
{
public:
  sc_port<very_simple_bus_if> bus_port;

  SC_HAS_PROCESS(writer);

  writer(sc_module_name nm) : sc_module(nm)
  {
    SC_THREAD(main);
  }

  void main()
  {
    char data[10];

    memset(data, 3, 10);

    while (true)
    {
	// every 50 NS (= 40 + 10) we cause some contention to occur...

       wait(40, SC_NS);
       bus_port->burst_write(data, 0, 10);
    }
  }
};

class reader : public sc_module
{
public:
  sc_port<very_simple_bus_if> bus_port;

  SC_HAS_PROCESS(reader);

  reader(sc_module_name nm) : sc_module(nm)
  {
    SC_THREAD(main);
  }

  void main()
  {
    char data[10];

    while (true)
    {
       bus_port->burst_read(data, 0, 5);
       wait(5, SC_NS);
    }
  }
};

int sc_main (int argc , char *argv[])
{
  very_simple_bus b("bus", 100, sc_time(1, SC_NS));
  reader r("reader");
  r.bus_port(b);
  writer w("writer");
  w.bus_port(b);
  
  sc_start(200, SC_NS);
  return 0;
}

