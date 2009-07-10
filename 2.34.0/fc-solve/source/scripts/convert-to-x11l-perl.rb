require "FcsLicense.rb"

ARGV.each { |fn| change_perl_file(fn) }
