#!/usr/bin/ruby
#
#      memscan.rb - 2010/11/12 15:31
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

require 'memscan.so'
require 'hash-plus.rb'
require 'format.rb'

# Memscan usage
# 
#
# basic:
# m= Memscan.new
# m.attach pid
#
# m.search_long val
# m.search_string val
#
# m.put_long addr val
# m.put_string addr val
#
# advanced:
#
# m.stack_dump
# m.heap_dump
# m.data_dump
#
# m.search_cache_clean
# m.reload_maps
# m.maps

class Memscan

  attr_reader :maps

  def initialize
    @cache = nil
    @maps = Hash.new
  end

  def search_long val
    search_string [val].pack("i")
  end
  
  def search_long_nocache val
    search_long_raw val
  end

  def search_string s
    dump if @cache.nil?
    res = Array.new
    
    @cache.each do |key,val|
      segment = 0
      val.each do |pak|
        r = Array.new
        # pp key
        # pp pak.size
        pak.scan(s) do |t|
          match = Regexp.last_match.offset(0).first
          r << [@maps[key][segment].first, match]
          puts "DBG: found #{pak[match...match+(s.size)]} in #{key},#{segment}"
        end
        res += r if r.size > 0
        segment +=1
      end
    end
    res
  end
  
  def dump_stack
    dump if @cache.nil?
    @cache[:stack]
  end

  def dump_heap
    dump if @cache.nil?
    @cache[:heap]
  end

  def dump_data
    dump if @cache.nil?
    @cache[:data]
  end

private

  def dump
    @cache = Hash.new
    # entries are Arrays with one packed binary string for each mapped segment
    @cache[:stack] = dump_stack_raw.map { |segm| segm.sorted_values.pack("L*") }
    @cache[:heap] = dump_heap_raw.map { |segm| segm.sorted_values.pack("L*") }
    @cache[:data] = dump_data_raw.map { |segm| segm.sorted_values.pack("L*") }
  end

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
      @maps[:data] = @data
    end

    unless stack.nil?
      @stack= stack.map do |e|
        e.to_i(16)
      end
      @maps[:stack] = [ @stack ]
    end

    unless heap.nil?
      @heap= heap.map do |e|
        e.to_i(16)
      end
      @maps[:heap] = [ @heap ]
    end

  end
end
