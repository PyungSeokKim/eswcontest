#!/bin/sh

if [ -z $1 ]
then
	echo "Invalid command line parameter"
	echo "Syntax: $0 [command] [argument]"
	echo "	[command] could be \"disable\", \"ap-play\", \"ap-record\" or \"ap-duplex\""
	echo
	exit
fi

if [ $1 = "disable" ]
then
	echo "Disable all audio paths"
	amixer cset name='set_scenario' 0,0,0,0,0,0
	exit
fi

if [ $1 = "ap-duplex" ]
then
	echo "Enable application processor full duplex audio scenario"
	amixer cset name='set_scenario' 1,0,0,0,0,48
	exit
fi


if [ $1 = "ap-play" ]
then
	echo "Enable application processor playback audio scenario"
	exit
fi

if [ $1 = "ap-record" ]
then
	echo "Enable application processor recording audio scenario"
	exit
fi

echo "Invalide command \"$1\""
