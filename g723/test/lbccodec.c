/*
**
** File:        "lbccodec.c"
**
** Description:     Top-level source code for G.723.1 dual-rate codec
**
** Functions:       main
**                  Process_files()
**
**
*/


/*
  ITU-T G.723.1 Software Package Release 2 (June 2006)
    
  ITU-T G.723.1 Speech Coder   ANSI-C Source Code     Version 5.2
  copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
  Universite de Sherbrooke.  All rights reserved.
  Last modified : March 2006
*/

#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

//begin-----------------------------------add by haiping 2009-06-17
#define LIMIT 2
//end  -----------------------------------add by haiping 2009-06-17

#include "../include/g723_const.h"
#include "../include/lbccodec.h"

extern void    Init_Vad(void) ;
extern void    Init_Coder( void);
extern void    Init_Decod(void);
extern void    Init_Cod_Cng(void);
extern void    Init_Dec_Cng(void);
extern Flag    Coder( Word16 *DataBuff, char *Vout );
extern Flag    Decod( Word16 *DataBuff, char *Vinp, Word16 Crc );
extern void    Write_lbc( Word16 *Dpnt, int Len, FILE *Fp );
extern void    Read_lbc( Word16 *Dpnt, int Len, FILE *Fp );
extern void    Line_Wr( char *, FILE * ) ;
extern int     Line_Rd( char *, FILE * ) ;
extern void    reset_max_time(void);

//begin-----------------------------------add by haiping 2009-06-17

/* C-callable function to return value of CYCLES register */
unsigned int cycles() {

    unsigned int ret;
    __asm__ __volatile__
        (
         "%0 = CYCLES;\n\t"
         : "=&d" (ret)
         :
         : "R1"
         );

    return ret;
}

struct timeval start[10],finish[10];

float msec[10]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float sum[10]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float mips[10]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

unsigned int before=0;
unsigned int temp;
unsigned int max_diff[10]={0,0,0,0,0,0,0,0,0,0};
//end ------------------------------------add by haiping 2009-06-17
/* Global variables */
enum  Wmode    WrkMode = Both ;
enum  Crate    WrkRate = Rate63 ;

int   PackedFrameSize[2] = {
    24 ,
    20
} ;

Flag    UseHp = True ;
Flag    UsePf = True ;
Flag    UseVx = False ;
Flag    UsePr = True ;

char  SignOn[] = "ITU-T G.723.1 ANSI C FIXED POINT CODEC Ver 5.2 (Release 2, June 2006)\n" ;

int main( int argc, char *argv[] )
{
    FILE    *Ifp, *Ofp ;            /* I/O File pointers */
    FILE    *Fep  = NULL;           /* Frame erasures file pointer */
    FILE    *Ratp = NULL;           /* Rate file pointer */

    long    FrCnt = 0 ;
    long    FlLen ;

    Word16  DataBuff[Frame] ;
    Word16  Crc ;
    char Rate_Rd;

    char    Line[24] ;

    //begin ------------------------------------add by haiping 2009-06-17
    int count = 0;
    //end   ------------------------------------add by haiping 2009-06-17

    printf("%s", SignOn ) ;

    /* Process all the input file */
    //begin for(;;) ----------------------------add by haiping 2009-06-17
    for(count=0;count<LIMIT;count++)
        {
            /* Process arguments and open I/O files */
            FlLen = Process_Files( &Ifp, &Ofp, &Fep, &Ratp, argc, argv ) ;
            printf("$$$$$$$-----FlLen = %ld-----$$$$$$$$ \n",FlLen);
            /*
              Init coder and the decoder
            */
            Init_Coder( ) ;
            Init_Decod( ) ;

            /* Init Comfort Noise Functions */
            if( UseVx ) {
                Init_Vad();
                Init_Cod_Cng( );
            }
            Init_Dec_Cng( );

            FrCnt = 0;
            //end   ------------------------------------add by haiping 2009-06-17

            do {
                //begin ---------------------------add by haiping 2009-06-17
                temp=0;
                //end  ----------------------------add by haiping 2009-06-17

                switch ( WrkMode ) {

                case Both:
                    if(Ratp != NULL) {
                        fread((char *)&Rate_Rd, sizeof(char), 1, Ratp);
                        WrkRate = (enum Crate)Rate_Rd;
                    }
                    if ( WrkRate == Rate53) reset_max_time();
                    Read_lbc( DataBuff, Frame, Ifp ) ;
                    Coder( DataBuff, Line ) ;
                    Decod( DataBuff, Line, (Word16) 0 ) ;
                    Write_lbc( DataBuff, Frame, Ofp ) ;
                    break ;

                case Cod :
                    if(Ratp != NULL) {
                        fread((char *)&Rate_Rd, sizeof(char), 1, Ratp);
                        WrkRate = (enum Crate)Rate_Rd;
                    }
                    if ( WrkRate == Rate53) reset_max_time();
                    Read_lbc( DataBuff, Frame, Ifp ) ;

                    //begin ---------------------------add by haiping 2009-06-17               
                    gettimeofday(&start[count],NULL);
                    before = cycles();
                    //end  ----------------------------add by haiping 2009-06-17
                

                    Coder( DataBuff, Line ) ;
                    Line_Wr( Line, Ofp ) ;
               
                    //begin ---------------------------add by haiping 2009-06-17
                    temp= cycles() - before;
                    if (temp>max_diff[count]) max_diff[count]=temp;
                    gettimeofday(&finish[count],NULL);
                    //end  ----------------------------add by haiping 2009-06-17


                    break ;

                case Dec :
                    if(Line_Rd( Line, Ifp ) == (-1)) {
                        FlLen = FrCnt;
                        break;
                    }
                    if ( Fep == NULL )
                        Crc = (Word16) 0 ;
                    else
                        fread( (char *)&Crc, sizeof(Word16), 1, Fep ) ;

                    //begin ---------------------------add by haiping 2009-06-17
                    gettimeofday(&start[count],NULL);
                    before = cycles();
                    //end  ----------------------------add by haiping 2009-06-17

                    Decod( DataBuff, Line, Crc ) ;
                    Write_lbc( DataBuff, Frame, Ofp ) ;
                
                    //begin ---------------------------add by haiping 2009-06-17
                    temp= cycles() - before;
                    if (temp>max_diff[count]) max_diff[count]=temp;
                    gettimeofday(&finish[count],NULL);
                    //end  ----------------------------add by haiping 2009-06-17


                    break ;
                }
        
                //begin ---------------------------add by haiping 2009-06-17
                msec[count] =finish[count].tv_sec*1000+finish[count].tv_usec/1000;
                msec[count]-=(start[count].tv_sec*1000+start[count].tv_usec/1000);//time for a frame;
                sum[count]+=msec[count];
                //end  ----------------------------add by haiping 2009-06-17

                FrCnt ++ ;
                if( UsePr) {
                    if( WrkMode == Dec) {
                        if(FrCnt < FlLen) {
                            fprintf( stdout, "Done : %6ld\r", FrCnt) ;
                        }
                    }
                    else {
                        fprintf( stdout, "Done : %6ld %3ld ％\r", FrCnt, FrCnt*100/FlLen ) ;
                    }
                    fflush(stdout);
                }

            } while(FrCnt < FlLen) ;
            //begin ---------------------------------add by haiping 2009-06-17
            
            if(Ifp) { (void)fclose(Ifp); }
            if(Ofp) { (void)fclose(Ofp); }
            if(Fep) { (void)fclose(Fep); }
            if(Ratp) { (void)fclose(Ratp); }
        }
    printf("\n");
    //end for(;;) ---------------------------add by haiping 2009-06-17
    
    //begin ---------------------------add by haiping 2009-06-17

    //consume time
    printf("\nCode or Decode  %ld frame consumed milliseconds is :\n",FrCnt);

    for(count=0;count<LIMIT;count++)
        {
            printf("sum[%d] = %f milliseconds \n",count,sum[count]);
        }

    printf("\nCode or Decode a frame consumed milliseconds is: \n");

    for(count=0;count<LIMIT;count++)
        {
            printf("sum[%d]/FrCnt = %f milliseconds \n",count,sum[count]/FrCnt);
        }

    //mips

    //av_mips = (float)(max_diff/1)*((1000/30)/1E6);简化如下：
    float mips_sum=0.0;

    printf("=======MIPS PER CYCLE=======\n");
    for(count=0;count<LIMIT;count++)
        {
            mips[count] = (float)(max_diff[count]/30000);
            mips_sum += mips[count];
            printf("cycles = %d MIPS = %5.2f\n",max_diff[count],mips[count]);
        }
    printf("=======average MIPS=========\n");
    printf("av_mips = %5.2f\n",mips_sum/LIMIT);

    //end  ----------------------------add by haiping 2009-06-17

    return 0 ;
}


/*
  This function processes the argument parameters. The function
  opens the IO files, and sets the global arguments accordingly
  to the command line parameters.
*/

long  Process_Files( FILE **Ifp, FILE **Ofp, FILE **Fep, FILE **Ratp,
                     int Argc, char *Argv[] )
{
    int     i ;
    long    Flen ;
    char    *FerFileName = NULL ;
    char    *RateFileName = NULL ;

    /*
      Process the argument list, if any
    */
    if (Argc < 3 ) {
        printf("Usage: %s [options] inputfile outputfile \n", Argv[0]);
        exit(1);
    }

    for ( i = 1 ; i < Argc-2 ; i ++ ) {

        /* Check the coder rate */
        if ( ! strncmp( "-r", Argv[i], 2) ) {
            if ( ! strcmp("63", Argv[i]+2) ) {
                WrkRate = Rate63 ;
                continue ;
            }

            else if ( ! strcmp("53", Argv[i]+2) ) {
                WrkRate = Rate53 ;
                continue ;
            }

            else {
                RateFileName = &Argv[i][2] ;
                continue ;
            }
        }

        /* Check Working mode */
        if ( ! strcmp("-b", Argv[i]) ) {
            WrkMode = Both ;
            continue ;
        }

        if ( ! strcmp("-c", Argv[i]) ) {
            WrkMode = Cod ;
            continue ;
        }

        if ( ! strcmp("-d", Argv[i]) ) {
            WrkMode = Dec ;
            continue ;
        }

        if ( ! strcmp("-v", Argv[i]) ) {
            UseVx = True ;
            continue ;
        }

        if ( ! strcmp("-Noh", Argv[i]) ) {
            UseHp = False;
            continue ;
        }

        if ( ! strcmp("-Nop", Argv[i]) ) {
            UsePf = False;
            continue ;
        }

        if ( ! strncmp( "-f", Argv[i], 2) ) {
            FerFileName = &Argv[i][2] ;
            continue ;
        }

        if ( ! strcmp("-n", Argv[i]) ) {
            UsePr = False;
            continue ;
        }

        fprintf(stderr, "Illegal argument, %s\n", Argv[i] ) ;
        exit(1) ;
    }

    *Ifp = fopen( Argv[Argc-2], "rb") ;
    if ( *Ifp == NULL ) {
        fprintf(stderr, "Invalid input file name: %s\n", Argv[Argc-2] ) ;
        exit(1) ;
    }

    if ( UsePr )
        printf("Input  file:     %s\n", Argv[Argc-2] ) ;

    *Ofp = fopen( Argv[Argc-1], "wb") ;
    if ( *Ofp == NULL ) {
        fprintf(stderr, "Can't open output file: %s\n", Argv[Argc-1] ) ;
        exit(1) ;
    }
    if ( UsePr )
        printf("Output file:     %s\n", Argv[Argc-1] ) ;

    /* Open Fer file if required */
    if ( WrkMode == Dec ) {

        if ( FerFileName != NULL ) {
            *Fep = fopen( FerFileName, "rb" ) ;
            if ( *Fep == NULL ) {
                fprintf(stderr, "Can't open FER file: %s\n", FerFileName ) ;
                exit(1) ;
            }
            if ( UsePr )
                printf("FER    file:     %s\n", FerFileName ) ;
        }
    }else {
        if ( RateFileName != NULL ) {
            *Ratp = fopen( RateFileName, "rb" ) ;
            if ( *Ratp == NULL ) {
                fprintf(stderr, "Can't open Rate file: %s\n", RateFileName ) ;
                exit(1) ;
            }
            if ( UsePr )
                printf("Rate   file:     %s\n", RateFileName ) ;
        }
    }

    /* Options report */
    if ( UsePr ) {

        printf("Options:\n");
        if (WrkMode == Both )
            printf("Encoder/Decoder\n");
        else if (WrkMode == Cod )
            printf("Encoder\n");
        else
            printf("Decoder\n");


        if( WrkMode != Cod ) {
            if (UsePf == 0 )
                printf("Postfilter disabled\n");
            else
                printf("Postfilter enabled\n");
        }

        if( WrkMode <= Cod ) {
            if(*Ratp == NULL) {
                if (WrkRate == Rate63 )
                    printf("Rate 6.3 kb/s\n");
                else
                    printf("Rate 5.3 kb/s\n");
            }
            if (UseHp == 0 )
                printf("Highpassfilter disabled\n");
            else
                printf("Highpassfilter enabled\n");
            if (UseVx == 0 )
                printf("VAD/CNG disabled\n");
            else
                printf("VAD/CNG enabled\n");
        }
    }

    /*
      Compute the file length
    */
    fseek( *Ifp, 0L, SEEK_END ) ;
    Flen = ftell( *Ifp ) ;
    rewind( *Ifp ) ;
    if ( WrkMode == Dec )
        Flen = 0x7fffffffL ;
    else
        Flen /= sizeof(Word16)*Frame ;

    return Flen ;
}

