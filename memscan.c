/*
 *      memscan.c - 2010/11/12 15:31
 *      
 *      Copyright 2010 Riccardo Cecolin <rikiji@playkanji.com>
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

#include <ruby.h>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <linux/ptrace.h>

#define DEBUG

#define V_PID rb_intern("pid")
#define GETPID() get_and_check_pid(self)
#define RAISE(x) rb_raise(rb_eRuntimeError,x)
#define NOTMAPPED() rb_ivar_get(self,rb_intern("@mapped")) != Qtrue

#ifdef DEBUG
#define DBG(x) printf("0x%08x: %s, %s: %d\n", x, #x, __FILE__, __LINE__)
#define DBGS(x) printf("%s, %s: %d\n", x, __FILE__, __LINE__)
#else
#define DBG(x)
#define DBGS(x) 
#endif

VALUE rb_cMemscan;

static pid_t get_and_check_pid(VALUE self)
{
  VALUE rpid = rb_ivar_get(self,V_PID);
  if(TYPE(rpid) != T_FIXNUM)
    RAISE("pid not set");
  
  pid_t pid = (pid_t)NUM2LONG(rpid);
  if (!pid)
    RAISE("pid not valid");

  return pid;
}

static VALUE
rb_memscan_initialize (VALUE self)
{
  unsigned long debugON = 0xdeadbeef; 
  DBG(debugON);

  return self;
}

static VALUE 
rb_memescan_search_float (VALUE self, VALUE val)
{
	/* 2DO */
}

static VALUE
rb_memscan_search_byte (VALUE self, VALUE val)
{
	/* 2DO */
}

static VALUE
rb_memescan_search_string (VALUE self, VALUE val)
{
	/* 2DO */
}

static void
internal_search_long(pid_t pid, unsigned long begin, unsigned long end, unsigned long ref, VALUE match)
{
	/* match an already initialized array */
	/* 2DO: check match rarray-ness */
	unsigned long addr, u;
	
	for (addr=begin; addr<end; addr+=sizeof(long)) {
		u = ptrace(PTRACE_PEEKDATA,
				   pid, addr, 0);
		if(u == -1)
			if(errno)
				RAISE(strerror(errno));
		
		if(u == ref)
			rb_ary_push(match,ULONG2NUM(addr));
	}	
} 

static VALUE 
rb_memscan_search_long (VALUE self, VALUE val)
{
  unsigned long addr, begin, end;
  unsigned long i, u, ref = NUM2ULONG(val);
  
  if (NOTMAPPED())
    rb_funcall(self,rb_intern("memmap"),0);

  pid_t pid = GETPID();
  VALUE match = rb_ary_new();

  /* search stack */
  VALUE stack= rb_ivar_get(self,rb_intern("@stack"));
  if (stack != Qnil) {
	  begin= NUM2ULONG(rb_ary_entry(stack,0));
	  end= NUM2ULONG(rb_ary_entry(stack,1));
	  internal_search_long(pid,begin,end,ref,match);
  } else {
	  DBGS("segment not found!");
  }
  

  /* search heap */  
  VALUE heap= rb_ivar_get(self,rb_intern("@heap"));
  if (heap != Qnil) {
	  begin = NUM2ULONG(rb_ary_entry(heap,0));
	  end= NUM2ULONG(rb_ary_entry(heap,1));
	  internal_search_long(pid,begin,end,ref,match);
  } else {
	  DBGS("segment not found!");
  }

  /* search data */
  VALUE data_ary= rb_ivar_get(self,rb_intern("@data"));
  if (data_ary != Qnil) {
	  for(i=0;i<RARRAY(data_ary)->len;i++) {	  
		  VALUE slice = rb_ary_entry(data_ary,i);
		  begin= NUM2ULONG(rb_ary_entry(slice,0));
		  end= NUM2ULONG(rb_ary_entry(slice,1));
		  internal_search_long(pid,begin,end,ref,match);
	  }
  } else {
	  DBGS("segment not found!");
  }
  
  return match;
}

static void
internal_dump_region (pid_t pid, unsigned int begin, unsigned int end, VALUE dump)
{
	/* dump must be an already initialized rarray */
	/* 2DO: check dump rarray-ness */
	unsigned int addr, u;
	for (addr=begin; addr<end; addr+=sizeof(long)) {
		u = ptrace(PTRACE_PEEKDATA,
				   pid, addr, 0);
		if(u == -1)
			if(errno)
				RAISE(strerror(errno));
		
		rb_ary_push(dump,ULONG2NUM(u));
	}
}

#define DATA 0x00
#define HEAP 0x01
#define STACK 0x02

static VALUE
rb_memscan_dump_generic (VALUE self, VALUE val)
{
  unsigned long begin, end;
  unsigned long i;

  if (NOTMAPPED())
	  rb_funcall(self,rb_intern("memmap"),0);

  pid_t pid = GETPID();
  VALUE dump = rb_ary_new();
  
  if (NUM2INT(val) == DATA) {
	  /* data is an array */
	  VALUE data_ary= rb_ivar_get(self,rb_intern("@data"));
	  if (data_ary == Qnil)
		  RAISE("data segment not found");

	  for(i=0;i<RARRAY(data_ary)->len;i++) {		  
		  VALUE slice = rb_ary_entry(data_ary,i);
		  begin= NUM2ULONG(rb_ary_entry(slice,0));
		  end= NUM2ULONG(rb_ary_entry(slice,1));
		  DBG(end-begin);
		  internal_dump_region(pid,begin,end,dump);
	  }

  } else {

	  VALUE region;
	  if(NUM2INT(val) == STACK)
		  region= rb_ivar_get(self,rb_intern("@stack"));
	  else
		  region= rb_ivar_get(self,rb_intern("@heap"));
	  
	  if (region == Qnil) {
		  /* 2DO: error handling */
		  DBGS("segment not found!");
		  return dump;
	  }
	  begin= NUM2ULONG(rb_ary_entry(region,0));
	  end= NUM2ULONG(rb_ary_entry(region,1));
	  DBG(end-begin);
	  internal_dump_region(pid,begin,end,dump);	  	  	  
  }
  
  return dump;
}

static VALUE
rb_memscan_dump_data (VALUE self)
{
	return rb_memscan_dump_generic(self,INT2FIX(DATA));
}

static VALUE
rb_memscan_dump_heap (VALUE self)
{
	return rb_memscan_dump_generic(self,INT2FIX(HEAP));
}

static VALUE
rb_memscan_dump_stack (VALUE self)
{
	return rb_memscan_dump_generic(self,INT2FIX(STACK));
}

static VALUE
rb_memscan_attach(int argc, VALUE * argv, VALUE self)
{
  pid_t pid;

  /* 2DO: pid string to fixnum handling */
  if (argc == 1) 
    rb_ivar_set(self,V_PID,argv[0]);
  
  pid = GETPID();

  DBG(pid);
  ptrace(PTRACE_ATTACH,pid, 0, 0);
  //    RAISE("attach failed");

  int status;

  rb_waitpid(pid,&status,0);//==-1)
  //    RAISE("waitpid failed");

  if(WIFSTOPPED(status))
    return Qtrue;

  return Qnil;
    
}

static VALUE
rb_memscan_get_pid (VALUE self)
{
  return rb_ivar_get(self,V_PID);
}

static VALUE
rb_memscan_set_pid (VALUE self, VALUE val)
{
  /* pid string to fixnum handling */
  rb_ivar_set(self,V_PID,val);
  return self;
}

void
Init_memscan()
{
  rb_cMemscan= rb_define_class("Memscan", rb_cObject);

  rb_define_method(rb_cMemscan, "initialize", rb_memscan_initialize, 0);
  rb_define_method(rb_cMemscan, "pid", rb_memscan_get_pid, 0);
  rb_define_method(rb_cMemscan, "pid=", rb_memscan_set_pid, 1);
  rb_define_method(rb_cMemscan, "attach", rb_memscan_attach, -1);
  rb_define_method(rb_cMemscan, "dump_stack", rb_memscan_dump_stack, 0);
  rb_define_method(rb_cMemscan, "dump_heap", rb_memscan_dump_heap, 0);
  rb_define_method(rb_cMemscan, "dump_data", rb_memscan_dump_data , 0);
  rb_define_method(rb_cMemscan, "search_long", rb_memscan_search_long, 1);
  /*  
	  rb_define_method(rb_cMemscan, "search_byte", rb_memscan_search_long, 1);
	  rb_define_method(rb_cMemscan, "search_string", rb_memscan_search_string, 1);
	  rb_define_method(rb_cMemscan, "search_long", rb_memscan_search_byte, 1);
  */
}
