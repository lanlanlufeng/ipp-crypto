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
//        ippsPrimeSet_BN()
//
*/

#include "owncp.h"
#include "pcpprimeg.h"

/*F*
// Name: ippsPrimeSet_BN
//
// Purpose: Sets a trial BN for further testing
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pCtx
//                               NULL == pPrime
//    ippStsContextMatchErr      illegal pCtx->idCtx
//                               illegal pPrime->idCtx
//    ippStsOutOfRangeErr        BITSIZE_BNU(BN_NUMBER(pPrime), BN_SIZE(pPrime)) 
//                                                  > PRIME_MAXBITSIZE(pCtx)
//    ippStsNoErr                no error
//
// Parameters:
//    pPrime      pointer to the BN to be set
//    pCtx     pointer to the context
//
*F*/
IPPFUN(IppStatus, ippsPrimeSet_BN, (const IppsBigNumState* pPrime, IppsPrimeState* pCtx))
{
   IPP_BAD_PTR2_RET(pCtx, pPrime);

   /* use aligned context */
   pPrime  = (IppsBigNumState*)( IPP_ALIGNED_PTR(pPrime, BN_ALIGNMENT) );
   IPP_BADARG_RET(!BN_VALID_ID(pPrime), ippStsContextMatchErr);
   pCtx = (IppsPrimeState*)( IPP_ALIGNED_PTR(pCtx, PRIME_ALIGNMENT) );
   IPP_BADARG_RET(!PRIME_VALID_ID(pCtx), ippStsContextMatchErr);

   IPP_BADARG_RET(BITSIZE_BNU(BN_NUMBER(pPrime), BN_SIZE(pPrime)) > PRIME_MAXBITSIZE(pCtx), ippStsOutOfRangeErr);

   {
      BNU_CHUNK_T* pPrimeU = BN_NUMBER(pPrime);
      cpSize ns = BN_SIZE(pPrime);
      cpSize nBits = BITSIZE_BNU(pPrimeU, ns);

      BNU_CHUNK_T* pPrimeCtx = PRIME_NUMBER(pCtx);
      BNU_CHUNK_T topMask = MASK_BNU_CHUNK(nBits);

      ZEXPAND_COPY_BNU(pPrimeCtx, BITS_BNU_CHUNK(PRIME_MAXBITSIZE(pCtx)), pPrimeU, ns);
      pPrimeCtx[ns-1] &= topMask;

      return ippStsNoErr;
   }
}
