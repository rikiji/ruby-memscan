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
#define DBG(x) printf("0x%8x: %s, %s: %d\n", x, #x, __FILE__, __LINE__)
#else
#define DBG(x)
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
}

static VALUE
rb_memscan_search_byte (VALUE self, VALUE val)
{
}

static VALUE
rb_memescan_search_string (VALUE self, VALUE val)
{
}

static VALUE 
rb_memscan_search_long (VALUE self, VALUE val)
{
  unsigned long addr, begin, end;
  unsigned long data, ref = NUM2ULONG(val);

  if (NOTMAPPED())
    rb_funcall(self,rb_intern("memmap"),0);

  pid_t pid = GETPID();

  /* search stack */
  VALUE stack= rb_ivar_get(self,rb_intern("@stack"));
  begin = NUM2ULONG(rb_ary_entry(stack,0));
  end= NUM2ULONG(rb_ary_entry(stack,1));

  VALUE match = rb_ary_new();

  for (addr=begin; addr<end; addr+=sizeof(long)) {
    printf("%d %x\n",pid,addr);
    data = ptrace(PTRACE_PEEKDATA,
		  pid, addr, 0);
    if(data == -1)
      if(errno)
	RAISE(strerror(errno));

    DBG(data);

    if(data == ref)
      rb_ary_push(match,ULONG2NUM(addr));
  }

  return match;
}

static VALUE
rb_memscan_dump_text (VALUE self) {
  /* GET TEXT */
  return rb_memscan_dump_generic( /* TEXT */);
}

static VALUE
rb_memscan_dump_heap (VALUE self) {
  /* GET HEAP */
  return rb_memscan_dump_generic( /* HEAP */);
}

static VALUE
rb_memscan_dump_stack (VALUE self) {
  /* GET STACK */
  return rb_memscan_dump_generic( /* STACK */);
}

static VALUE
rb_memscan_dump_generic (VALUE self, value val)
{

  /* FIX to generic */
  unsigned long begin, end, addr, data;

  if (NOTMAPPED())
    rb_funcall(self,rb_intern("memmap"),0);

  VALUE stack= rb_ivar_get(self,rb_intern("@stack"));
  if (stack == Qnil)
    RAISE("stack segment not found");

  pid_t pid = GETPID();
  begin= NUM2ULONG(rb_ary_entry(stack,0));
  end= NUM2ULONG(rb_ary_entry(stack,1));

  DBG(end-begin);

  VALUE dump = rb_ary_new();

  for (addr=begin; addr<end; addr+=sizeof(long)) {
    data = ptrace(PTRACE_PEEKDATA,
		  pid, addr, 0);
    if(data == -1)
      if(errno)
	RAISE(strerror(errno));

    rb_ary_push(dump,ULONG2NUM(data));
  }
  return dump;
}

static VALUE
rb_memscan_attach(int argc, VALUE * argv, VALUE self)
{
  pid_t pid;

  if (argc == 1) 
    rb_ivar_set(self,V_PID,argv[0]);
  
  pid = GETPID();

  printf("piid: %d\n",pid);
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
  rb_define_method(rb_cMemscan, "search_long", rb_memscan_search_long, 1);
}
