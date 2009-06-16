#!/bin/sh -x
# qcmp.sh
#v0.1
#g723.1 test for all the tv
#haiping zeng 2009-04-14
#------------------------------------
#TEST_DIR=/home/happysict/program/codeblocds/bc_96_g723_4_uclinux/g723/test
TEST_DIR=.

#encoder test

./tvcmp    $TEST_DIR/tv/PATHC63H.PCO  $TEST_DIR/tv/PATHC63H.PCO
./tvcmp    $TEST_DIR/tv/PATHC53.PCO   $TEST_DIR/tv/PATHC53.PCO
./tvcmp    $TEST_DIR/tv/OVERC63.PCO   $TEST_DIR/tv/OVERC63.PCO
./tvcmp    $TEST_DIR/tv/OVERC53H.PCO  $TEST_DIR/tv/OVERC53H.PCO
./tvcmp    $TEST_DIR/tv/CODEC63.PCO   $TEST_DIR/tv/CODEC63.PCO
./tvcmp    $TEST_DIR/tv/INEQC53.PCO   $TEST_DIR/tv/INEQC53.PCO
./tvcmp    $TEST_DIR/tv/TAMEC63H.PCO  $TEST_DIR/tv/TAMEC63H.PCO


#decoder test

./tvcmp    $TEST_DIR/tv/INEQD53.POU   $TEST_DIR/tv/INEQD53.POU
./tvcmp    $TEST_DIR/tv/OVERD53.POU   $TEST_DIR/tv/OVERD53.POU
./tvcmp    $TEST_DIR/tv/OVERD63P.POU  $TEST_DIR/tv/OVERD63P.POU
./tvcmp    $TEST_DIR/tv/PATHD53.POU   $TEST_DIR/tv/PATHD53.POU
./tvcmp    $TEST_DIR/tv/PATHD63P.POU  $TEST_DIR/tv/PATHD63P.POU
./tvcmp    $TEST_DIR/tv/TAMED63P.POU  $TEST_DIR/tv/TAMED63P.POU
