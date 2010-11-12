#!/usr/bin/ruby
#
#      test.rb - 2010/11/12 15:31
#      
#      Copyright 2010 Riccardo Cecolin <rikiji@playkanji.com>
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

def format_results r
  unless r.nil?
    r.each do |m|
      puts "0x%08x" % m     
    end
  end
end

m= Memscan.new

pid= ARGV[0].to_i
m.attach pid

puts "stack size: " + (m.dump_stack.size * 4).to_s
puts "heap size: " + (m.dump_heap.size * 4).to_s
puts "data size: " + (m.dump_data.size * 4).to_s

format_results m.search_long 3735928559
format_results m.search_long 3405691582
