#!/usr/bin/ruby
#
#      format.rb - 2012/03/23 20:07
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

def format_a r
  unless r.nil?
    n = 0
    r.each do |m|
      print "0x%08x " % m
      n += 1
      puts if (n %= 4).zero?
    end
    puts unless (n %= 4).zero?
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

