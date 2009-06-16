/*
**
** File:    lsp.c
**
** Description: Functions that implement line spectral pair 
**      (LSP) operations.  
**
** Functions:
**
**  Converting between linear predictive coding (LPC) coefficients
**  and LSP frequencies:
**
**      AtoLsp()
**      LsptoA()
**
**  Vector quantization (VQ) of LSP frequencies:
**
**      Lsp_Qnt()
**      Lsp_Svq()
**      Lsp_Inq()
**
**  Interpolation of LSP frequencies:
**
**      Lsp_Int()
*/

/*
	ITU-T G.723.1 Software Package Release 2 (June 2006)
    
    ITU-T G.723.1 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/

/*  
# **  
# ** 文件:    lsp.c  
# **  
# ** LPC系数和LSP系数之间的转化：  
# **              AtoLsp()：把LPC系数转化为LSP系数  
# **              LsptoA()：把LSP系数转化为LPC系数  
# ** LSP 系数的矢量量化(VQ)：:  
# **              Lsp_Qnt()：把第三个子矢量进行量化，量化为8bit  
# **              Lsp_Svq()：把10维的LSP系数的矢量转变为三个子矢量  
# **              Lsp_Inq()：三个子矢量分别量化  
# ** LSP系数的插值:  
# **              Lsp_Int()：对所有的LSP系数，进行线性插值。  
# **  
# 
*/
   
#include <stdio.h>

#include "../include/g723_const.h"

extern Word16  *BandQntTable[LspQntBands] ;
extern Word16   BandInfoTable[LspQntBands][2] ;
extern Word16   LspDcTable[LpcOrder] ;
extern Word16   CosineTable[CosineTableSize] ;
extern Word16   BandExpTable[LpcOrder] ;

#define MIN_16 (Word16)0x8000
#define MAX_16 (Word16)0x7fff
#define  CosineTableSize   512

extern Word16 mult_r(Word16 var1, Word16 var2);  /* Mult with round,     2 */
extern Word16 round_(Word32 L_var1);              /* Round,               1 */
extern Word16 mac_r(Word32 L_var3, Word16 var1, Word16 var2);
extern Word16 msu_r(Word32 L_var3, Word16 var1, Word16 var2);
extern Word32 L_mls( Word32, Word16 ) ;        /* Wght ?? */
extern Word32 L_mult(Word16 var1, Word16 var2);  /* Long mult,           1 */
extern Word16 negate(Word16 var1);               /* Short negate,        1 */
extern Word32 L_negate(Word32 L_var1);               /* Long negate,     2 */
extern void  LsptoA( Word16 *Lsp );
extern Word16 shr(Word16 var1, Word16 var2);     /* Short shift right,   1 */
extern Word32 L_shr(Word32 L_var1, Word16 var2); /* Long shift right,    2 */
extern Word16 shl(Word16 var1, Word16 var2);     /* Short shift left,    1 */
extern Word32 L_shl(Word32 L_var1, Word16 var2); /* Long shift left,     2 */
extern Word16 add(Word16 var1, Word16 var2);     /* Short add,           1 */
extern Word32 L_add(Word32 L_var1, Word32 L_var2);   /* Long add,        2 */
extern Word16 sub(Word16 var1, Word16 var2);     /* Short sub,           1 */
extern Word32 L_sub(Word32 L_var1, Word32 L_var2);   /* Long sub,        2 */
extern Word16 div_s(Word16 var1, Word16 var2); /* Short division,       18 */
extern Word32   Lsp_Svq( Word16 *Tv, Word16 *Wvect );
extern Word32 L_msu(Word32 L_var3, Word16 var1, Word16 var2); /* Msu,    1 */
extern Word16 extract_h(Word32 L_var1);          /* Extract high,        1 */
extern Word16 div_l( Word32, Word16 ) ;
extern Word32 L_mac(Word32 L_var3, Word16 var1, Word16 var2); /* Mac,    1 */
extern Word32 L_deposit_h(Word16 var1);       /* 16 bit var1 -> MSB,     2 */
extern Word32 L_sub(Word32 L_var1, Word32 L_var2);   /* Long sub,        2 */
extern Word32 L_abs(Word32 L_var1);            /* Long abs,              3 */
extern Word16 norm_l(Word32 L_var1);           /* Long norm,            30 */
extern Word32 L_shl(Word32 L_var1, Word16 var2); /* Long shift left,     2 */
extern Word32 L_mac(Word32 L_var3, Word16 var1, Word16 var2); /* Mac,    1 */
extern Word16 norm_s(Word16 var1);             /* Short norm,           15 */

/*
**
** Function:            AtoLsp()
**
** Description:     Transforms 10 LPC coefficients to the 10
**          corresponding LSP frequencies for a subframe.
**          This transformation is done once per frame,
**          for subframe 3 only.  The transform algorithm
**          generates sum and difference polynomials from
**          the LPC coefficients.  It then evaluates the
**          sum and difference polynomials at uniform
**          intervals of pi/256 along the unit circle.
**          Intervals where a sign change occurs are
**          interpolated to find the zeros of the
**          polynomials, which are the LSP frequencies.
**
** Links to text:   Section 2.5
**
** Arguments:       
**
**  Word16 *LspVect     Empty Buffer
**  Word16 Lpc[]        Unquantized LPC coefficients (10 words)
**  Word16 PrevLsp[]    LSP frequencies from the previous frame (10 words)
**
** Outputs:
**
**  Word16 LspVect[]    LSP frequencies for the current frame (10 words)
**
** Return value:        None
**
**/

/*  
# ** 函数:            Lsp_Qnt()  
# ** 作用:     量化LSP系数,LSP系数被分为三个子矢量，分别为3维、3维、4维。  
# ** 每个子矢量使用不同的矢量量化表。每个量化表有256个数据，所以用8比特量化。  
# ** 参数:         
# **  Word16 CurrLsp[]    当前帧的未被量化的LSP矢量  
# **  Word16 PrevLsp[]    前一帧的10个LSP矢量  
# ** 输出:          
# ** 当前帧的10个已被量化的LSP矢量  
# 
*/   

void AtoLsp( Word16 *LspVect, Word16 *Lpc, Word16 *PrevLsp )
{

    int   i,j,k ;

    Word32   Lpq[LpcOrder+2] ;
    Word16   Spq[LpcOrder+2] ;

    Word16   Exp   ;
    Word16   LspCnt ;

    Word32   PrevVal,CurrVal   ;
    Word32   Acc0,Acc1   ;


 /*
  * Perform a bandwidth expansion on the LPC coefficients.  This
  * scales the poles of the LPC synthesis filter by a factor of
  * 0.994.
  */

/*  
#   现在LPC系数的带宽扩充.  把LPC合成滤波器的零点扩大0.994倍.  
#   
*/ 
    for ( i = 0 ; i < LpcOrder ; i ++ )
        LspVect[i] = mult_r( Lpc[i], BandExpTable[i] ) ;


 /*
  * Compute the sum and difference polynomials with the roots at z =
  * -1 (sum) or z = +1 (difference) removed.  Let these polynomials
  * be P(z) and Q(z) respectively, and let their coefficients be
  * {p_i} amd {q_i}.  The coefficients are stored in the array Lpq[]
  * as follows: p_0, q_0, p_1, q_1, ..., p_5, q_5.  There is no need
  * to store the other coefficients because of symmetry.
  */


 /*
  * Set p_0 = q_0 = 1.  The LPC coefficients are already scaled by
  *  1/4.  P(z) and Q(z) are scaled by an additional scaling factor of
  *  1/16, for an overall factor of 1/64 = 0x02000000L.
  */

/*
#  计算 sum 和 difference 极点 根为z = -1 (sum) or z = +1 (difference)  
#   *把几点以p_0, q_0, p_1, q_1, ..., p_5, q_5.的形式存放在数组中  
#   
*/
   
    Lpq[0] = Lpq[1] = (Word32) 0x02000000L ;

 /*
  * This loop computes the coefficients of P(z) and Q(z).  The long
  * division (to remove the real zeros) is done recursively.
  */
    for ( i = 0 ; i < LpcOrder/2 ; i ++ ) {

        /* P(z) */
        Acc0 = L_negate( Lpq[2*i+0] ) ;
        Acc1 = L_deposit_h( LspVect[i] ) ;
        Acc1 = L_shr( Acc1, (Word16) 4 ) ;
        Acc0 = L_sub( Acc0, Acc1 ) ;
        Acc1 = L_deposit_h( LspVect[LpcOrder-1-i] ) ;
        Acc1 = L_shr( Acc1, (Word16) 4 ) ;
        Acc0 = L_sub( Acc0, Acc1 ) ;
        Lpq[2*i+2] = Acc0 ;

        /* Q(z) */
        Acc0 = Lpq[2*i+1] ;
        Acc1 = L_deposit_h( LspVect[i] ) ;
        Acc1 = L_shr( Acc1, (Word16) 4 ) ;

        Acc0 = L_sub( Acc0, Acc1 ) ;
        Acc1 = L_deposit_h( LspVect[LpcOrder-1-i] ) ;
        Acc1 = L_shr( Acc1, (Word16) 4 ) ;
        Acc0 = L_add( Acc0, Acc1 ) ;
        Lpq[2*i+3] = Acc0 ;
    }

 /*
  * Divide p_5 and q_5 by 2 for proper weighting during polynomial
  * evaluation.
  */

/*  
#   在多项式估计中把p_5和 q_5一分为二   
#   
*/ 
    Lpq[LpcOrder+0] = L_shr( Lpq[LpcOrder+0], (Word16) 1 ) ;
    Lpq[LpcOrder+1] = L_shr( Lpq[LpcOrder+1], (Word16) 1 ) ;

 /*
  * Normalize the polynomial coefficients and convert to shorts
  */


/*  
#   * 把多项式系数正常化并且转化成短型  
#   
*/
    /* Find the maximum */      
/*找到最大值*/  
    Acc1 = L_abs( Lpq[0] ) ;
    for ( i = 1 ; i < LpcOrder+2 ; i ++ ) {
        Acc0 = L_abs( Lpq[i] ) ;
        if ( Acc0 > Acc1 )
            Acc1 = Acc0 ;
    }

    /* Compute the normalization factor */
    Exp = norm_l( Acc1 ) ;


    /* Normalize and convert to shorts */
    for ( i = 0 ; i < LpcOrder+2 ; i ++ ) {
        Acc0 = L_shl( Lpq[i], Exp ) ;
        Spq[i] = round( Acc0 ) ;
    }

 /*
  * Initialize the search loop
  */

 /*
  * The variable k is a flag that indicates which polynomial (sum or
  * difference) the algorithm is currently evaluating.  Start with
  * the sum.
  */

/*  
#   * 初始化搜索环  
#   
*/   

    
/*  
#  变量K是一个标记用来区分多项式是sum还是difference，以sum开始  
#   
*/ 
    k = 0 ;

    /* Evaluate the sum polynomial at frequency zero */
    /* 估计 sum 多项式为0的频率 */ 
    PrevVal = (Word32) 0 ;
    for ( j = 0 ; j <= LpcOrder/2 ; j ++ )
        PrevVal = L_mac( PrevVal, Spq[2*j], CosineTable[0] ) ;


 /*
  * Search loop.  Evaluate P(z) and Q(z) at uniform intervals of
  * pi/256 along the unit circle.  Check for zero crossings.  The
  * zeros of P(w) and Q(w) alternate, so only one of them need by
  * evaluated at any given step.
  */
 
/*  
#   * 搜索环. 在单位圆中以pi/256的间隔来估计P(z)和Q(z)  
#   
*/ 
    LspCnt = (Word16) 0 ;
    for ( i = 1 ; i < CosineTableSize/2 ; i ++ ) {

        /* Evaluate the selected polynomial */
        /* 估计选择的多项式 */ 
        CurrVal = (Word32) 0 ;
        for ( j = 0 ; j <= LpcOrder/2 ; j ++ )
            CurrVal = L_mac( CurrVal, Spq[LpcOrder-2*j+k],
                                    CosineTable[i*j%CosineTableSize] ) ;

        /* Check for a sign change, indicating a zero crossing */
        /* 检测信号的改变 */ 
        if ( (CurrVal ^ PrevVal) < (Word32) 0 ) {

 /*
  * Interpolate to find the bottom 7 bits of the
  * zero-crossing frequency
  */
            Acc0 = L_abs( CurrVal ) ;
            Acc1 = L_abs( PrevVal ) ;
            Acc0 = L_add( Acc0, Acc1 ) ;

            /* Normalize the sum */
            Exp = norm_l( Acc0 ) ;
            Acc0 = L_shl( Acc0, Exp ) ;
            Acc1 = L_shl( Acc1, Exp ) ;

            Acc1 = L_shr( Acc1, (Word16) 8 ) ;

            LspVect[LspCnt] = div_l( Acc1, extract_h( Acc0 ) ) ;

 /*
  * Add the upper part of the zero-crossing frequency,
  * i.e. bits 7-15
  */
            Exp = shl( (Word16) (i-1), (Word16) 7 ) ;
            LspVect[LspCnt] = add( LspVect[LspCnt], Exp ) ;
            LspCnt ++ ;

            /* Check if all zeros have been found */
             /* 检测是否所有的零点都找到 */
            if ( LspCnt == (Word16) LpcOrder )
                break ;

 /*
  * Switch the pointer between sum and difference polynomials
  */
            k ^= 1 ;

 /*
  * Evaluate the new polynomial at the current frequency
  */
/*  
#   * 估计在现有的频率下的新的多项式  
*/
            CurrVal = (Word32) 0 ;
            for ( j = 0 ; j <= LpcOrder/2 ; j ++ )
                CurrVal = L_mac( CurrVal, Spq[LpcOrder-2*j+k],
                                    CosineTable[i*j%CosineTableSize] ) ;
        }

        /* Update the previous value */
    
         /* 更新前面的值 */ 
        PrevVal = CurrVal ;
    }


 /*
  * Check if all 10 zeros were found.  If not, ignore the results of
  * the search and use the previous frame's LSP frequencies instead.
  */

/*  
#   *检测是否所有的10个零点都找到。如果没有找到，则忽略找到的结果，  
#   *并使用先前的LSP系数的来代替   
*/
    if ( LspCnt != (Word16) LpcOrder ) {
        for ( j = 0 ; j < LpcOrder ; j ++ )
            LspVect[j] = PrevLsp[j] ;
    }

    return ;
}

/*
**
** Function:            Lsp_Qnt()
**
** Description:     Vector quantizes the LSP frequencies.  The LSP
**          vector is divided into 3 sub-vectors, or
**          bands, of dimension 3, 3, and 4.  Each band is
**          quantized separately using a different VQ
**          table.  Each table has 256 entries, so the
**          quantization generates three indices of 8 bits
**          each.  (Only the LSP vector for subframe 3 is
**          quantized per frame.)
**
** Links to text:   Section 2.5
**
** Arguments:       
**
**  Word16 CurrLsp[]    Unquantized LSP frequencies for the current frame (10 words)
**  Word16 PrevLsp[]    LSP frequencies from the previous frame (10 words)
**
** Outputs:             Quantized LSP frequencies for the current frame (10 words)
**
** Return value:
**
**  Word32      Long word packed with the 3 VQ indices.  Band 0
**          corresponds to bits [23:16], band 1 corresponds
**          to bits [15:8], and band 2 corresponds to bits [7:0].
**          (Bit 0 is the least significant.)
**
*/

/*  
# ** 函数:            Lsp_Qnt()  
# ** 作用:     量化LSP系数,LSP系数被分为三个子矢量，分别为3维、3维、4维。  
# ** 每个子矢量使用不同的矢量量化表。每个量化表有256个数据，所以用8比特量化。  
# ** 参数:         
# **  Word16 CurrLsp[]    当前帧的未被量化的LSP矢量  
# **  Word16 PrevLsp[]    前一帧的10个LSP矢量  
# ** 输出:          
# ** 当前帧的10个已被量化的LSP矢量  
# 
*/
  
Word32   Lsp_Qnt( Word16 *CurrLsp, Word16 *PrevLsp )
{
    int   i ;

    Word16   Wvect[LpcOrder] ;

    Word16   Tmp0,Tmp1   ;
    Word16   Exp   ;


 /*
  * Compute the VQ weighting vector.  The weights assign greater
  * precision to those frequencies that are closer together.
  */

    /* Compute the end differences */
    Wvect[0] = sub( CurrLsp[1], CurrLsp[0] ) ;
    Wvect[LpcOrder-1] = sub( CurrLsp[LpcOrder-1], CurrLsp[LpcOrder-2] ) ;

    /* Compute the rest of the differences */
    for ( i = 1 ; i < LpcOrder-1 ; i ++ ) {
        Tmp0 = sub( CurrLsp[i+1], CurrLsp[i] ) ;
        Tmp1 = sub( CurrLsp[i], CurrLsp[i-1] ) ;
        if ( Tmp0 > Tmp1 )
            Wvect[i] = Tmp1 ;
        else
            Wvect[i] = Tmp0 ;
    }

    /* Invert the differences */
    Tmp0 = (Word16) 0x0020 ;
    for ( i = 0 ; i < LpcOrder ; i ++ ) {

        if ( Wvect[i] > Tmp0 )
            Wvect[i] = div_s( Tmp0, Wvect[i] ) ;
        else
            Wvect[i] = MAX_16 ;
    }

    /* Normalize the weight vector */
    Tmp0 = (Word16) 0 ;
    for ( i = 0 ; i < LpcOrder ; i ++ )
        if ( Wvect[i] > Tmp0 )
            Tmp0 = Wvect[i] ;

    Exp = norm_s( Tmp0 ) ;
    for ( i = 0 ; i < LpcOrder ; i ++ )
        Wvect[i] = shl( Wvect[i], Exp ) ;


 /*
  * Compute the VQ target vector.  This is the residual that remains
  * after subtracting both the DC and predicted
  * components.
  */

 /*
  * Subtract the DC component from both the current and previous LSP
  * vectors.
  */

/*  
#   * 计算矢量量化的目标矢量，它是去除直流分量和预测分量的残差矢量.  
#  
 */   
   
/*  
#   * 从当前的，先前的LSP矢量中去除直流分量  
#   
*/   
    for ( i = 0 ; i < LpcOrder ; i ++ ) {
        CurrLsp[i] = sub( CurrLsp[i], LspDcTable[i] ) ;
        PrevLsp[i] = sub( PrevLsp[i], LspDcTable[i] ) ;
    }

 /*
  * Generate the prediction vector and subtract it.  Use a constant
  * first-order predictor based on the previous (DC-free) LSP
  * vector.
  */
    for ( i = 0 ; i < LpcOrder ; i ++ ) {
        Tmp0 = mult_r( PrevLsp[i], (Word16) LspPrd0 ) ;
        CurrLsp[i] = sub( CurrLsp[i], Tmp0 ) ;
    }

 /*
  * Add the DC component back to the previous LSP vector.  This
  * vector is needed in later routines.
  */
    for ( i = 0 ; i < LpcOrder ; i ++ )
        PrevLsp[i] = add( PrevLsp[i], LspDcTable[i] ) ;

 /*
  * Do the vector quantization for all three bands
  */
    return Lsp_Svq( CurrLsp, Wvect ) ;
}

/*
**
** Function:            Lsp_Svq()
**
** Description:     Performs the search of the VQ tables to find
**          the optimum LSP indices for all three bands.
**          For each band, the search finds the index which 
**          minimizes the weighted squared error between 
**          the table entry and the target.
**
** Links to text:   Section 2.5
**
** Arguments:       
**
**  Word16 Tv[]     VQ target vector (10 words)
**  Word16 Wvect[]      VQ weight vector (10 words)
**
** Outputs:         None
**
** Return value:    
**
**  Word32      Long word packed with the 3 VQ indices.  Band 0
**          corresponds to bits [23:16], band 1 corresponds
**          to bits [15:8], and band 2 corresponds to bits [7:0].
**              
*/

/*  
# ** 函数:            Lsp_Svq()  
# ** 作用:       
# **           对三个子矢量寻找最佳（使加权均方误差最小的）的矢量量化表  
# ** 参数:         
# **  Word16 Tv[]     VQ 目标矢量 (10 words)  
# **  Word16 Wvect[]      VQ 权重矢量 (10 words)  
# ** 输出:         无  
# ** 返回值:      
# **  Word32      长整型的矢量索引.   
# 
*/   

Word32   Lsp_Svq( Word16 *Tv, Word16 *Wvect )
{
    int   i,j,k ;

    Word32   Rez,Indx    ;
    Word32   Acc0,Acc1   ;

    Word16   Tmp[LpcOrder] ;
    Word16  *LspQntPnt  ;


 /*
  * Initialize the return value
  */
    Rez = (Word32) 0 ;

 /*
  * Quantize each band separately
  */
    for ( k = 0 ; k < LspQntBands ; k ++ ) {

 /*
  * Search over the entire VQ table to find the index that
  * minimizes the error.
  */

/*  
#   *  搜索矢量量化表，找到使均方误差最小的索引  
#   
*/   

        /* Initialize the search */
        Acc1 = (Word32) -1 ;
        Indx = (Word32) 0 ;
        LspQntPnt = BandQntTable[k] ;

        for ( i = 0 ; i < LspCbSize ; i ++ ) {

 /*
  * Generate the metric, which is the negative error with the
  * constant component removed.
  */

/*  
#   * 做矩阵运算  
*/ 
            for ( j = 0 ; j < BandInfoTable[k][1] ; j ++ )
                Tmp[j] = mult_r( Wvect[BandInfoTable[k][0]+j],
                                                            LspQntPnt[j] ) ;

            Acc0 = (Word32) 0 ;
            for ( j = 0 ; j < BandInfoTable[k][1] ; j ++ )
                Acc0 = L_mac( Acc0, Tv[BandInfoTable[k][0]+j], Tmp[j] ) ;
            Acc0 = L_shl( Acc0, (Word16) 1 ) ;
            for ( j = 0 ; j < BandInfoTable[k][1] ; j ++ )
                Acc0 = L_msu( Acc0, LspQntPnt[j], Tmp[j] ) ;

            LspQntPnt += BandInfoTable[k][1] ;

 /*
  * Compare the metric to the previous maximum and select the
  * new index
  */
            if ( Acc0 > Acc1 ) {
                Acc1 = Acc0 ;
                Indx = (Word32) i ;
            }
        }

 /*
  * Pack the result with the optimum index for this band
  */
        Rez = L_shl( Rez, (Word16) LspCbBits ) ;
        Rez = L_add( Rez, Indx ) ;
    }

    return Rez ;
}

/*
**
** Function:            Lsp_Inq()
**
** Description:     Performs inverse vector quantization of the
**          LSP frequencies.  The LSP vector is divided
**          into 3 sub-vectors, or bands, of dimension 3,
**          3, and 4.  Each band is inverse quantized
**          separately using a different VQ table.  Each
**          table has 256 entries, so each VQ index is 8
**          bits.  (Only the LSP vector for subframe 3 is
**          quantized per frame.)
**
** Links to text:   Sections 2.6, 3.2
**
** Arguments:
**
**  Word16 *Lsp     Empty buffer
**  Word16 PrevLsp[]    Quantized LSP frequencies from the previous frame
**               (10 words)
**  Word32 LspId        Long word packed with the 3 VQ indices.  Band 0
**               corresponds to bits [23:16], band 1 corresponds
**               to bits [15:8], and band 2 corresponds to bits
**               [7:0].
**  Word16 Crc      Frame erasure indicator
**
** Outputs:
**
**  Word16 Lsp[]        Quantized LSP frequencies for current frame (10
**               words)
**
** Return value:         None
**
*/
void Lsp_Inq( Word16 *Lsp, Word16 *PrevLsp, Word32 LspId, Word16 Crc )
{
    int  i,j   ;

    Word16  *LspQntPnt  ;


    Word16   Scon  ;
    Word16   Lprd  ;

    Word16   Tmp   ;
    Flag     Test  ;


 /*
  * Check for frame erasure.  If a frame erasure has occurred, the
  * resulting VQ table entries are zero.  In addition, a different
  * fixed predictor and minimum frequency separation are used.
  */
    if ( Crc == (Word16) 0 ) {
        Scon = (Word16) 0x0100 ;
        Lprd = LspPrd0 ;
    }
    else {
        LspId = (Word32) 0 ;
        Scon = (Word16) 0x0200 ;
        Lprd = LspPrd1 ;
    }


 /*
  * Inverse quantize the 10th-order LSP vector.  Each band is done
  * separately.
  */
    for ( i = LspQntBands-1; i >= 0 ; i -- ) {

 /*
  * Get the VQ table entry corresponding to the transmitted index
  */
        Tmp = (Word16) ( LspId & (Word32) 0x000000ff ) ;
        LspId >>= 8 ;

        LspQntPnt = BandQntTable[i] ;

        for ( j = 0 ; j < BandInfoTable[i][1] ; j ++ )
            Lsp[BandInfoTable[i][0] + j] =
                                LspQntPnt[Tmp*BandInfoTable[i][1] + j] ;
    }

 /*
  * Subtract the DC component from the previous frame's quantized
  * vector
  */
    for ( j = 0 ; j < LpcOrder ; j ++ )
        PrevLsp[j] = sub(PrevLsp[j], LspDcTable[j] ) ;

 /*
  * Generate the prediction vector using a fixed first-order
  * predictor based on the previous frame's (DC-free) quantized
  * vector
  */
    for ( j = 0 ; j < LpcOrder ; j ++ ) {
        Tmp = mult_r( PrevLsp[j], Lprd ) ;
        Lsp[j] = add( Lsp[j], Tmp ) ;
    }

 /*
  * Add the DC component back to the previous quantized vector,
  * which is needed in later routines
  */
    for ( j = 0 ; j < LpcOrder ; j ++ ) {
        PrevLsp[j] = add( PrevLsp[j], LspDcTable[j] ) ;
        Lsp[j] = add( Lsp[j], LspDcTable[j] ) ;
    }


 /*
  * Perform a stability test on the quantized LSP frequencies.  This
  * test checks that the frequencies are ordered, with a minimum
  * separation between each.  If the test fails, the frequencies are
  * iteratively modified until the test passes.  If after 10
  * iterations the test has not passed, the previous frame's
  * quantized LSP vector is used.
  */
    for ( i = 0 ; i < LpcOrder ; i ++ ) {

        /* Check the first frequency */
        if ( Lsp[0] < (Word16) 0x180 )
            Lsp[0] = (Word16) 0x180 ;

        /* Check the last frequency */
        if ( Lsp[LpcOrder-1] > (Word16) 0x7e00 )
            Lsp[LpcOrder-1] = (Word16) 0x7e00 ;

        /* Perform the modification */
        for ( j = 1 ; j < LpcOrder ; j ++ ) {

            Tmp = add( Scon, Lsp[j-1] ) ;
            Tmp = sub( Tmp, Lsp[j] ) ;
            if ( Tmp > (Word16) 0 ) {
                Tmp = shr( Tmp, (Word16) 1 ) ;
                Lsp[j-1] = sub( Lsp[j-1], Tmp ) ;
                Lsp[j] = add( Lsp[j], Tmp ) ;
            }
        }

        Test = False ;

 /*
  * Test the modified frequencies for stability.  Break out of
  * the loop if the frequencies are stable.
  */
        for ( j = 1 ; j < LpcOrder ; j ++ ) {
            Tmp = add( Lsp[j-1], Scon ) ;
            Tmp = sub( Tmp, (Word16) 4 ) ;
            Tmp = sub( Tmp, Lsp[j] ) ;
            if ( Tmp > (Word16) 0 )
                Test = True ;
        }

        if ( Test == False )
            break ;
    }


 /*
  * Return the result of the stability check.  True = not stable,
  * False = stable.
  */
    if ( Test == True) {
        for ( j = 0 ; j < LpcOrder ; j ++ )
            Lsp[j] = PrevLsp[j] ;
    }

    return;
}

/*
**
** Function:            Lsp_Int()
**
** Description:     Computes the quantized LPC coefficients for a
**          frame.  First the quantized LSP frequencies
**          for all subframes are computed by linear
**          interpolation.  These frequencies are then
**          transformed to quantized LPC coefficients.
**
** Links to text:   Sections 2.7, 3.3
**
** Arguments:
**
**  Word16 *QntLpc      Empty buffer
**  Word16 CurrLsp[]    Quantized LSP frequencies for the current frame,
**               subframe 3 (10 words)
**  Word16 PrevLsp[]    Quantized LSP frequencies for the previous frame,
**               subframe 3 (10 words)
**
** Outputs:
**
**  Word16 QntLpc[]     Quantized LPC coefficients for current frame, all
**               subframes (40 words)
**
** Return value:        None
**
*/
void  Lsp_Int( Word16 *QntLpc, Word16 *CurrLsp, Word16 *PrevLsp )
{
    int   i,j   ;

    Word16   Tmp   ;
    Word16  *Dpnt  ;

    Word32   Acc0  ;


 /*
  * Initialize the interpolation factor
  */
    Tmp = (Word16) (MIN_16 / SubFrames ) ;

    Dpnt = QntLpc ;


 /*
  * Do for all subframes
  */
    for ( i = 0 ; i < SubFrames ; i ++ ) {

 /*
  * Compute the quantized LSP frequencies by linear interpolation
  * of the frequencies from subframe 3 of the current and
  * previous frames
  */
        for ( j = 0 ; j < LpcOrder ; j ++ ) {
            Acc0 = L_deposit_h( PrevLsp[j] ) ;
            Acc0 = L_mac( Acc0, Tmp, PrevLsp[j] ) ;
            Acc0 = L_msu( Acc0, Tmp, CurrLsp[j] ) ;
            Dpnt[j] = round( Acc0 ) ;
        }

 /*
  * Convert the quantized LSP frequencies to quantized LPC
  * coefficients
  */
        LsptoA( Dpnt ) ;
        Dpnt += LpcOrder ;

        /* Update the interpolation factor */
        Tmp = add( Tmp, (Word16) (MIN_16 / SubFrames ) ) ;
    }

}


/*
**
** Function:            LsptoA()
**
** Description:     Converts LSP frequencies to LPC coefficients
**          for a subframe.  Sum and difference
**          polynomials are computed from the LSP
**          frequencies (which are the roots of these
**          polynomials).  The LPC coefficients are then
**          computed by adding the sum and difference
**          polynomials.
**          
** Links to text:   Sections 2.7, 3.3
**
** Arguments:       
**
**  Word16 Lsp[]        LSP frequencies (10 words)
**
** Outputs:
**
**  Word16 Lsp[]        LPC coefficients (10 words)
**
** Return value:        None
** 
*/
void  LsptoA( Word16 *Lsp )
{
    int   i,j   ;

    Word32   Acc0,Acc1   ;
    Word16   Tmp ;

    Word32   P[LpcOrder/2+1] ;
    Word32   Q[LpcOrder/2+1] ;


 /*
  * Compute the cosines of the LSP frequencies by table lookup and
  * linear interpolation
  */
    for ( i = 0 ; i < LpcOrder ; i ++ ) {

 /*
  * Do the table lookup using bits [15:7] of the LSP frequency
  */
        j = (int) shr( Lsp[i], (Word16) 7 ) ;
        Acc0 = L_deposit_h( CosineTable[j] ) ;

 /*
  * Do the linear interpolations using bits [6:0] of the LSP
  * frequency
  */
        Tmp = sub(CosineTable[j+1], CosineTable[j] ) ;
        Acc0 = L_mac( Acc0, Tmp, add( shl( (Word16)(Lsp[i] & 0x007f) ,
                                (Word16)8 ), (Word16) 0x0080 ) ) ;
        Acc0 = L_shl( Acc0, (Word16) 1 ) ;
        Lsp[i] = negate( round( Acc0 ) ) ;
    }


 /*
  * Compute the sum and difference polynomials with the real roots
  * removed.  These are computed by polynomial multiplication as
  * follows.  Let the sum polynomial be P(z).  Define the elementary
  * polynomials P_i(z) = 1 - 2cos(w_i) z^{-1} + z^{-2}, for 1<=i<=
  * 5, where {w_i} are the LSP frequencies corresponding to the sum
  * polynomial.  Then P(z) = P_1(z)P_2(z)...P_5(z).  Similarly
  * the difference polynomial Q(z) = Q_1(z)Q_2(z)...Q_5(z).
  */

 /*
  * Initialize the arrays with the coefficients of the product
  * P_1(z)P_2(z) and Q_1(z)Q_2(z).  Scale by 1/8.
  */
    P[0] = (Word32) 0x10000000L ;
    P[1] = L_mult( Lsp[0], (Word16) 0x2000 ) ;
    P[1] = L_mac( P[1], Lsp[2], (Word16) 0x2000 ) ;
    P[2] = L_mult( Lsp[0], Lsp[2] ) ;
    P[2] = L_shr( P[2], (Word16) 1 ) ;
    P[2] = L_add( P[2], (Word32) 0x20000000L ) ;

    Q[0] = (Word32) 0x10000000L ;
    Q[1] = L_mult( Lsp[1], (Word16) 0x2000 ) ;
    Q[1] = L_mac( Q[1], Lsp[3], (Word16) 0x2000 ) ;
    Q[2] = L_mult( Lsp[1], Lsp[3] ) ;
    Q[2] = L_shr( Q[2], (Word16) 1 ) ;
    Q[2] = L_add( Q[2], (Word32) 0x20000000L ) ;

 /*
  * Compute the intermediate polynomials P_1(z)P_2(z)...P_i(z) and
  * Q_1(z)Q_2(z)...Q_i(z), for i = 2, 3, 4.  Each intermediate
  * polynomial is symmetric, so only the coefficients up to i+1 need
  * by computed.  Scale by 1/2 each iteration for a total of 1/8.
  */
    for ( i = 2 ; i < LpcOrder/2 ; i ++ ) {

        /* Compute coefficient (i+1) */
        Acc0 = P[i] ;
        Acc0 = L_mls( Acc0, Lsp[2*i+0] ) ;
        Acc0 = L_add( Acc0, P[i-1] ) ;
        P[i+1] = Acc0 ;

        Acc1 = Q[i] ;
        Acc1 = L_mls( Acc1, Lsp[2*i+1] ) ;
        Acc1 = L_add( Acc1, Q[i-1] ) ;
        Q[i+1] = Acc1 ;

        /* Compute coefficients i, i-1, ..., 2 */
        for ( j = i ; j >= 2 ; j -- ) {
            Acc0 = P[j-1] ;
            Acc0 = L_mls( Acc0, Lsp[2*i+0] ) ;
            Acc0 = L_add( Acc0, L_shr(P[j], (Word16) 1 ) ) ;
            Acc0 = L_add( Acc0, L_shr(P[j-2], (Word16) 1 ) ) ;
            P[j] = Acc0 ;

            Acc1 = Q[j-1] ;
            Acc1 = L_mls( Acc1, Lsp[2*i+1] ) ;
            Acc1 = L_add( Acc1, L_shr(Q[j], (Word16) 1 ) ) ;
            Acc1 = L_add( Acc1, L_shr(Q[j-2], (Word16) 1 ) ) ;
            Q[j] = Acc1 ;
        }

        /* Compute coefficients 1, 0 */
        P[0] = L_shr( P[0], (Word16) 1 ) ;
        Q[0] = L_shr( Q[0], (Word16) 1 ) ;

        Acc0 = L_deposit_h( Lsp[2*i+0] ) ;
        Acc0 = L_shr( Acc0, (Word16) i ) ;
        Acc0 = L_add( Acc0, P[1] ) ;
        Acc0 = L_shr( Acc0, (Word16) 1 ) ;
        P[1] = Acc0 ;

        Acc1 = L_deposit_h( Lsp[2*i+1] ) ;
        Acc1 = L_shr( Acc1, (Word16) i ) ;
        Acc1 = L_add( Acc1, Q[1] ) ;
        Acc1 = L_shr( Acc1, (Word16) 1 ) ;
        Q[1] = Acc1 ;
    }


 /*
  * Convert the sum and difference polynomials to LPC coefficients
  * The LPC polynomial is the sum of the sum and difference
  * polynomials with the real zeros factored in: A(z) = 1/2 {P(z) (1
  * + z^{-1}) + Q(z) (1 - z^{-1})}.  The LPC coefficients are scaled
  * here by 16; the overall scale factor for the LPC coefficients
  * returned by this function is therefore 1/4.
  */
    for ( i = 0 ; i < LpcOrder/2 ; i ++ ) {
        Acc0 = P[i] ;
        Acc0 = L_add( Acc0, P[i+1] ) ;
        Acc0 = L_sub( Acc0, Q[i] ) ;
        Acc0 = L_add( Acc0, Q[i+1] ) ;
        Acc0 = L_shl( Acc0, (Word16) 3 ) ;
        Lsp[i] = negate( round( Acc0 ) ) ;

        Acc1 = P[i] ;
        Acc1 = L_add( Acc1, P[i+1] ) ;
        Acc1 = L_add( Acc1, Q[i] ) ;
        Acc1 = L_sub( Acc1, Q[i+1] ) ;
        Acc1 = L_shl( Acc1, (Word16) 3 ) ;
        Lsp[LpcOrder-1-i] = negate( round( Acc1 ) ) ;
    }

}
