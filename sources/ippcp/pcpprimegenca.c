/*******************************************************************************
* Copyright 2004-2019 Intel Corporation
* All Rights Reserved.
*
* If this  software was obtained  under the  Intel Simplified  Software License,
* the following terms apply:
*
* The source code,  information  and material  ("Material") contained  herein is
* owned by Intel Corporation or its  suppliers or licensors,  and  title to such
* Material remains with Intel  Corporation or its  suppliers or  licensors.  The
* Material  contains  proprietary  information  of  Intel or  its suppliers  and
* licensors.  The Material is protected by  worldwide copyright  laws and treaty
* provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
* modified, published,  uploaded, posted, transmitted,  distributed or disclosed
* in any way without Intel's prior express written permission.  No license under
* any patent,  copyright or other  intellectual property rights  in the Material
* is granted to  or  conferred  upon  you,  either   expressly,  by implication,
* inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
* property rights must be express and approved by Intel in writing.
*
* Unless otherwise agreed by Intel in writing,  you may not remove or alter this
* notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
* suppliers or licensors in any way.
*
*
* If this  software  was obtained  under the  Apache License,  Version  2.0 (the
* "License"), the following terms apply:
*
* You may  not use this  file except  in compliance  with  the License.  You may
* obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*
* Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
* distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the   License  for the   specific  language   governing   permissions  and
* limitations under the License.
*******************************************************************************/

/* 
//  Purpose:
//     Intel(R) Integrated Performance Primitives. Cryptographic Primitives (ippcp)
//     Prime Number Primitives.
// 
//  Contents:
//        ippsPrimeGen()
//
*/

#include "owncp.h"
#include "pcpprimeg.h"
#include "pcpprng.h"
#include "pcptool.h"

//tbcd: temporary excluded: #include <assert.h>
/*F*
// Name: ippsPrimeGen
//
// Purpose: Generates a random probable prime number of the specified bitlength.
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pCtx
//                               NULL == rndFunc
//    ippStsContextMatchErr      !PRIME_VALID_ID()
//    ippStsLengthErr            1 > nBits
//    ippStsOutOfRangeErr        nBits > PRIME_MAXBITSIZE()
//    ippStsBadArgErr            1 > nTrials
//    ippStsInsufficientEntropy  when prime generation fails due
//                               to a poor choice of seed/context bitstream.
//    ippStsNoErr                no error
//
// Parameters:
//    nBits       bitlength for the desired probable prime number
//    nTrials     parameter for the Miller-Rabin probable primality test
//    pCtx        pointer to the context
//    rndFunc     external PRNG
//    pRndParam   pointer to the external PRNG parameters
//
//
// Notes:
//    ippsPrimeGen()returns ippStsInsufficientEntropy, if it
//    detects that it needs more entropy seed during its probable prime
//    generation. In this case, the user should update PRNG parameters
//    and call the primitive again.
*F*/
IPPFUN(IppStatus, ippsPrimeGen, (int nBits, int nTrials, IppsPrimeState* pCtx,
                                 IppBitSupplier rndFunc, void* pRndParam))
{
   IPP_BAD_PTR2_RET(pCtx, rndFunc);

   /* use aligned context */
   pCtx = (IppsPrimeState*)( IPP_ALIGNED_PTR(pCtx, PRIME_ALIGNMENT) );
   IPP_BADARG_RET(!PRIME_VALID_ID(pCtx), ippStsContextMatchErr);

   IPP_BADARG_RET(nBits<1, ippStsLengthErr);
   IPP_BADARG_RET(nBits>PRIME_MAXBITSIZE(pCtx), ippStsOutOfRangeErr);

   IPP_BADARG_RET(nTrials<1, ippStsBadArgErr);

   {
      cpSize count;

      BNU_CHUNK_T botPattern = 0x1;
      BNU_CHUNK_T topPattern = (BNU_CHUNK_T)1 << ((nBits-1)&(BNU_CHUNK_BITS-1));
      BNU_CHUNK_T topMask = MASK_BNU_CHUNK(nBits);

      BNU_CHUNK_T* pRand = PRIME_NUMBER(pCtx);
      cpSize randLen = BITS_BNU_CHUNK(nBits);

      ZEXPAND_BNU(pRand, 0, BITS_BNU_CHUNK(PRIME_MAXBITSIZE(pCtx)));

      #define MAX_COUNT (1000)
      for(count=0; count<MAX_COUNT; count++) {
         Ipp32u result;

         /* get trial number */
         IppStatus sts = rndFunc((Ipp32u*)pRand, nBits, pRndParam);
         if(ippStsNoErr!=sts)
            return sts;

         /* set up top and bottom bit to 1 */
         pRand[0] |= botPattern;
         pRand[randLen-1] &= topMask;
         pRand[randLen-1] |= topPattern;

         /* test trial number */
         sts = ippsPrimeTest(nTrials, &result, pCtx, rndFunc, pRndParam);
         if(ippStsNoErr!=sts)
            return sts;

         if (result == IPP_IS_PRIME)
            return ippStsNoErr;
      }
      #undef MAX_COUNT

      return ippStsInsufficientEntropy;
   }
}
