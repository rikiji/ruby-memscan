#!/usr/bin/ruby
#
#      memscan.rb - 2010/11/12 15:31
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

require 'memscan.so'

class Memscan

private

  def memmap

    @mapped = true

    raise "pid not set" if pid.nil?

    f= File.open "/proc/#{pid}/cmdline"
    cmd= f.read.split "\0"    
    f= File.open "/proc/#{pid}/maps"

    stack = heap = nil
    data = Array.new
    f.each_line do |line|
      data<< line.split(' ').first.split('-') if line.match cmd.first
      stack= line.split(' ').first.split('-') if line.match "stack"
      heap= line.split(' ').first.split('-') if line.match "heap"
    end

    unless data.empty?
      @data= data.map do |a|
        a= a.map do |e|
          e.to_i(16)
        end
      end
    end

    unless stack.nil?
      @stack= stack.map do |e|
        e.to_i(16)
      end
    end

    unless heap.nil?
      @heap= heap.map do |e|
        e.to_i(16)
      end
    end

  end
end
