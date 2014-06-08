#!/usr/bin/env perl

our %data = (
	"\n"=>'\n',
	"\r"=>'\r',
	"\t"=>'\t',
	"\""=>'\"',
	"\'"=>'\'',
	"\\"=>"\\\\",
);

our @hex=(0..9,a..f);

sub change{
	my $d = shift;
	return $data{$d} if $data{$d};
	$d = ord($d);
	return '\x'.($hex[$d>>4]).($hex[$d&15]);
}

print "\nstatic const char *$ENV{'VARNAME'} =\n";
while(<>){
	s/^\s+//g;
	next unless /[^\s]/;
	s/([\\\"\x00-\x1f\x7f\xff])/change($1)/ge;
	print "\"$_\"\n";
}
print ";\n";
