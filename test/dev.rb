#!/usr/bin/ruby
#
#      dev.rb - 2010/11/12 15:31
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

m= Memscan.new

pid= ARGV[0].to_i
m.attach pid

puts "stack segments: " + (m.dump_stack.size).to_s
puts "heap segments: " + (m.dump_heap.size).to_s
puts "data segments: " + (m.dump_data.size).to_s

pp m.maps

# format_dump m.dump_stack

# search_string "foo"
s = "ZOMBIES"
puts "searching #{s}"
r = m.search_string(s)
pp r
format_search(r)

puts "searching deadbeef raw"
format_a m.search_long_nocache 0xdeadbeef

puts "searching deadbeef regexp"
format_search m.search_long 0xdeadbeef

