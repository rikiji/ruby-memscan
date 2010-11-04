#!/usr/bin/ruby

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
