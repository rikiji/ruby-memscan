require 'memscan'
require 'pp'

m=Memscan.new

pid = ARGV[0].to_i

p m.attach pid
p m.pid
p m.dump_stack.size
#p m.search_long 4000
