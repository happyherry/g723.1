#!/bin/sh -x
# qcmp.sh
#v0.1
#g723.1 test for all the tv
#haiping zeng 2009-04-14
#fix the bug :same files compare,pco compare with the pco ,not rco
#haiping zeng 2009-06-15
#------------------------------------
TEST_DIR=./g723/test

#encoder test

./$TEST_DIR/tvcmp    $TEST_DIR/tv/PATHC63H.PCO  $TEST_DIR/tv/PATHC63H.RCO
./$TEST_DIR/tvcmp    $TEST_DIR/tv/PATHC53.PCO   $TEST_DIR/tv/PATHC53.RCO
./$TEST_DIR/tvcmp    $TEST_DIR/tv/OVERC63.PCO   $TEST_DIR/tv/OVERC63.RCO
./$TEST_DIR/tvcmp    $TEST_DIR/tv/OVERC53H.PCO  $TEST_DIR/tv/OVERC53H.RCO
./$TEST_DIR/tvcmp    $TEST_DIR/tv/CODEC63.PCO   $TEST_DIR/tv/CODEC63.RCO
./$TEST_DIR/tvcmp    $TEST_DIR/tv/INEQC53.PCO   $TEST_DIR/tv/INEQC53.RCO
./$TEST_DIR/tvcmp    $TEST_DIR/tv/TAMEC63H.PCO  $TEST_DIR/tv/TAMEC63H.RCO


#decoder test

./$TEST_DIR/tvcmp    $TEST_DIR/tv/INEQD53.POU   $TEST_DIR/tv/INEQD53.ROU
./$TEST_DIR/tvcmp    $TEST_DIR/tv/OVERD53.POU   $TEST_DIR/tv/OVERD53.ROU
./$TEST_DIR/tvcmp    $TEST_DIR/tv/OVERD63P.POU  $TEST_DIR/tv/OVERD63P.ROU
./$TEST_DIR/tvcmp    $TEST_DIR/tv/PATHD53.POU   $TEST_DIR/tv/PATHD53.ROU
./$TEST_DIR/tvcmp    $TEST_DIR/tv/PATHD63P.POU  $TEST_DIR/tv/PATHD63P.ROU
./$TEST_DIR/tvcmp    $TEST_DIR/tv/TAMED63P.POU  $TEST_DIR/tv/TAMED63P.ROU
