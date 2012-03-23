#!/usr/bin/ruby
#
#      dumpmap.rb - 2010/11/12 15:31
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

require 'pp'

pid = ARGV[0].to_i

f= File.open "/proc/#{pid}/cmdline"
cmd= f.read.split "\0"

sect= [cmd.first,"heap","stack"]

f= File.open "/proc/#{pid}/maps"
mem = Array.new
f.each_line do |line|
  mem << line.split(' ').first.split('-') if sect.any? { |s| line.match s }
end

pp mem
