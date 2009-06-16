/*
*-----------------------------------------------------------------------------------
*version:0.1
*文件名：compare.c
*创立者：海平
*创立时间：2009-02-20
*功能描述：用于测试编解码后的代码是否跟
*ITU-T所给的参照数据一样
*version:0.2
*mt:2009-04-14
*change:modified it for shell script
*----------------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int codec_compare_test_function(int Argc,char *Argv[]);
int main(int argc ,char * argv[])
{
    codec_compare_test_function(argc,argv);
    return 0;
}

int codec_compare_test_function(int Argc,char *Argv[])
{
    /* Verify Function*/

    FILE *resultfile, *comparefile;
    char compare_buff[24456];
    char result_buff[24456];


    int     error_num=0 ;
    int i;

    /*
      Process the argument list, if any
    */
    if (Argc !=3) {
        printf("Usage: %s  resultfile comparefile \n", Argv[0]);
        exit(1);
    }

    bzero(compare_buff,24456);
    bzero(result_buff,24456);

    //Open the output file
    resultfile = fopen(Argv[1] , "rb") ;
    if (resultfile == NULL ) {
        fprintf(stderr, "Can't open result file: %s\n",Argv[1]) ;
        exit(1) ;
    }

    //open the compare file
    comparefile = fopen(Argv[2],"rb");
    if (comparefile == NULL) {
    fprintf(stderr, "Error: can't open compare file %s\n",Argv[2]);
    exit(1);
    }

    fread(result_buff,1,24456,resultfile);
    fread(compare_buff,1,24456,comparefile);

    for(i=0;i<24456;i++)
        {
            if(result_buff[i]!=compare_buff[i])
                {
                    error_num++;
                }
        }
    
    printf(" The result  file :%s \n The compare file :%s \n",Argv[1],Argv[2]);

    if (error_num)
    {
        printf("\n-----------------------------------------------\n");
        printf("++++++++find errors: %d don't match+++++++\n", error_num);
        printf("-----------------------------------------------\n");

    }else{
        printf("\n------------------------\n");
        printf("++++++Verify Sucess+++++ \n");
        printf("------------------------ \n");
    }
    if(resultfile) { (void)fclose(resultfile); }
    if(comparefile) { (void)fclose(comparefile); }
    return 0;
}

