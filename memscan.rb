#!/usr/bin/ruby

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
      data.each do |a|
        a= a.map do |e|
          e.to_i(16)
        end
      end
      @data = data
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
