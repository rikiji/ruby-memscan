MEMSCAN
-------

Riccardo Cecolin <r at rikiji dot de>

This ruby extension provides access to other processes memory from ruby scripts.
Its development isn't completed (yet), although you can already have some fun with it. Check "test/test.rb" for sample usage.

* 0.02, 2012/03/23
  caching and string searching functions added. ported to ruby 1.9. 

* 0.01, 2010/11/12
  first working version.
  heap stack bss and data correctly dumped into arrays of ulong values.
  basic (long) search function completed. 
