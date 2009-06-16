#!/bin/sh -x
# quik.sh
#v0.0
#g723.1 tv test shell programme!
#haiping zeng 2008-12-1
#------------------------------------
#v0.1
#modified by hpz 2002-02-16
#add the reference file to be compare
#with the extension *.POU or *.PCO
#------------------------------------
#TEST_DIR=/home/happysict/program/codeblocds/bc_96_g723_4_uclinux/g723/test
TEST_DIR=.. 

#encoder test

 ./$1 -c -r63      $TEST_DIR/tv/PATHC63H.TIN $TEST_DIR/tv/PATHC63H.PCO $TEST_DIR/tv/PATHC63H.RCO
 ./$1 -c -r53 -Noh $TEST_DIR/tv/PATHC53.TIN  $TEST_DIR/tv/PATHC53.PCO $TEST_DIR/tv/PATHC53.RCO

#decoder test
 ./$1 -r53 -d -Nop      $TEST_DIR/tv/PATHD53.TCO  $TEST_DIR/tv/PATHD53.POU $TEST_DIR/tv/PATHD63P.ROU
 ./$1 -r63 -d -f$TEST_DIR/tv/PATHD63P.CRC $TEST_DIR/tv/PATHD63P.TCO $TEST_DIR/tv/PATHD63P.POU $TEST_DIR/tv/PATHD53.ROU


