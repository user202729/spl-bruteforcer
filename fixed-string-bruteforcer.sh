#!/bin/sh

exec=./spl-bruteforcer1
string="Hello, World!"

len=${#string}
last=0
for i in $( seq 0 $((len-1)) ); do
	chr=${string:i:1}
	ord_chr=$(printf "%d\n" "'$chr")

	printf 'Remember'
	$exec <<- eof | sed 's/.*://; s/ *$/!/'
	0 I
	$last you
	-
	$ord_chr
	eof

	last=$ord_chr
done
