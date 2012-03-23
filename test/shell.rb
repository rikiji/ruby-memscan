#!/usr/bin/ruby
#
#      shell.rb - 2012/03/23 19:36
#      
#      Copyright 2010-2012 Riccardo Cecolin <r@rikiji.de>
#
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation; either version 2 of the License, or
#      (at your option) any later version.
#      
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#      GNU General Public License for more details.
#      
#      You should have received a copy of the GNU General Public License
#      along with this program; if not, write to the Free Software
#      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#      MA 02110-1301, USA.
#

$LOAD_PATH << '..'

require 'memscan'
require 'pp'

def format_a r
  unless r.nil?
    n = 0
    r.each do |m|
      print "0x%08x " % m
      n += 1
      puts if (n %= 4).zero?
    end
  end
end

def format_dump r
  unless r.nil?
    n = 0
    r.keys.sort.each do |k|
      print "0x%08x: " % k if (n %= 4).zero?
      print "%08x " % r[k]
      n += 1
      puts if (n %= 4).zero?
    end
  end
end

def format_search r
  unless r.nil?
    r.each do |v|
      puts "0x%08x" % (v[0] + v[1])
    end
  end
end

m= Memscan.new

pid= ARGV[0].to_i
m.attach pid

puts "stack size: " + (m.dump_stack.size * 4).to_s
puts "heap size: " + (m.dump_heap.size * 4).to_s
puts "data size: " + (m.dump_data.size * 4).to_s

# format_dump m.dump_stack

# search_string "foo"

while true do
  print "search value: "
  val= $stdin.gets.to_i(16)
  format_a m.search_long val
  puts
end


#r = m.search_string("ZOMBIES")
#pp r
#format_search(r)
#format_r m.search_long 3405691582
