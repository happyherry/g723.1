#!/bin/sh -x
# quik.sh
#v0.0
#g723.1 tv test shell programme!
#haiping zeng 2008-12-1
#v0.1 
#g723.1 test for all the tv
#haiping zeng 2009-04-14
#------------------------------------
#TEST_DIR=/home/happysict/program/codeblocds/bc_96_g723_4_uclinux/g723/test
TEST_DIR=. 

#encoder test

# ./$1 -c -r63      $TEST_DIR/tv/PATHC63H.TIN $TEST_DIR/tv/PATHC63H.PCO 
# ./$1 -c -r53 -Noh $TEST_DIR/tv/PATHC53.TIN  $TEST_DIR/tv/PATHC53.PCO
#modify by hpz 2009-04-14

./lbccodec -c -r63       $TEST_DIR/tv/PATHC63H.TIN  $TEST_DIR/tv/PATHC63H.PCO
./lbccodec -c -r53 -Noh  $TEST_DIR/tv/PATHC53.TIN   $TEST_DIR/tv/PATHC53.PCO
./lbccodec -c -r63 -Noh  $TEST_DIR/tv/OVERC63.TIN   $TEST_DIR/tv/OVERC63.PCO
./lbccodec -c -r53       $TEST_DIR/tv/OVERC53H.TIN  $TEST_DIR/tv/OVERC53H.PCO
./lbccodec -c -r63 -Noh  $TEST_DIR/tv/CODEC63.TIN   $TEST_DIR/tv/CODEC63.PCO
./lbccodec -c -r53 -Noh  $TEST_DIR/tv/INEQC53.TIN   $TEST_DIR/tv/INEQC53.PCO
./lbccodec -c -r63       $TEST_DIR/tv/TAMEC63H.TIN  $TEST_DIR/tv/TAMEC63H.PCO
 

#decoder test
# ./$1 -r53 -d -Nop      $TEST_DIR/tv/PATHD53.TCO  $TEST_DIR/tv/PATHD53.POU 
# ./$1 -r63 -d -f$TEST_DIR/tv/PATHD63P.CRC $TEST_DIR/tv/PATHD63P.TCO $TEST_DIR/tv/PATHD63P.POU
#modify by hpz 2009-04-14

./lbccodec -r53 -d -Nop            $TEST_DIR/tv/INEQD53.TCO   $TEST_DIR/tv/INEQD53.POU
./lbccodec -r53 -d -Nop            $TEST_DIR/tv/OVERD53.TCO   $TEST_DIR/tv/OVERD53.POU
./lbccodec -r63 -d                 $TEST_DIR/tv/OVERD63P.TCO  $TEST_DIR/tv/OVERD63P.POU
./lbccodec -r53 -d -Nop            $TEST_DIR/tv/PATHD53.TCO   $TEST_DIR/tv/PATHD53.POU
./lbccodec -r63 -d -f$TEST_DIR/tv/PATHD63P.CRC  $TEST_DIR/tv/PATHD63P.TCO  $TEST_DIR/tv/PATHD63P.POU
./lbccodec -r63 -d -f$TEST_DIR/tv/TAMED63P.CRC  $TEST_DIR/tv/TAMED63P.TCO  $TEST_DIR/tv/TAMED63P.POU


