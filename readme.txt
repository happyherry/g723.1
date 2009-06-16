This project is for porting the g723.1A to uclinux on the bf-533 target!

g723.1(2006:5.2)

2008-12-23 
2009-04-14
2009-06-14
2009-06-14:21:26
just to test git stash;
1)stash 21:38
2)stash 21:40
doc  
------------
3)test time
Makefile  
include ---
          |g723_const.h
          -
          |lbccodec.h
test------
          | Makefile
          -
          |lbccodec.c
          -
          |tv/ #test vector
src.orig---
          |Makefile
          -
          |basop.c     
          -
          |cod_cng.c  
          -
          |dec_cng.c  
          -
          |exc_lbc.c  
          -
          |lsp.c     
          -
          |tab_lbc.c  
          -
          |util_cng.c  
          -
          |vad.c
          -
          |coder.c    
          -
          |decod.c    
          -
          |lpc.c
          -
          |tame.c     
          -
          |util_lbc.c



hello! i am here.
