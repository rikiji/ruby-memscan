/*
 *      test.c - 2010/11/12 15:31
 *      
 *      Copyright 2010-2012 Riccardo Cecolin <r@rikiji.de>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
#include <stdlib.h>
#include <stdio.h>

int datav = 0xdeadbeef;

int main (int argc, char ** argv)
{
  printf("pid: %d\n",getpid());
  foo();
}

int foo()
{
  
  unsigned int stackv = 0xcafebabe;
  int stack2v = 0xdeadbeef;
  char z[] = "ZOMBIES";  
    fprintf(stderr,"values: %p %s\n",z,z);
    while (1)  {
      printf("data  %p: 0x%x\n"
	     "stack %p: 0x%x\n"
	     "\n", &datav, datav, &stackv, stackv);
      sleep(1);      
    }
}
