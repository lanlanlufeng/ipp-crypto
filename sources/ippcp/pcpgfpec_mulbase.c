/*******************************************************************************
* Copyright 2010-2019 Intel Corporation
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
// 
//  Purpose:
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
//     Internal EC over GF(p^m) basic Definitions & Function Prototypes
// 
//     Context:
//        gfec_base_point_mul()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpgfpecstuff.h"
#include "pcpmask_ct.h"

void gfec_base_point_mul(BNU_CHUNK_T* pRdata, const Ipp8u* pScalar8, int scalarBitSize, IppsGFpECState* pEC)
{
   /* size of window, get function and pre-computed table */
   int window_size = ECP_PREMULBP(pEC)->w;
   selectAP select_affine_point = ECP_PREMULBP(pEC)->select_affine_point;
   const BNU_CHUNK_T* pTbl = ECP_PREMULBP(pEC)->pTbl;

   IppsGFpState* pGF = ECP_GFP(pEC);
   gsModEngine* pGFE = GFP_PMA(pGF);
   int elmLen = GFP_FELEN(pGFE);

   mod_neg negF = GFP_METHOD(pGFE)->neg;

   BNU_CHUNK_T* mont1 = GFP_MNT_R(pGFE);

   /* number of points per table slot */
   int tslot_point = 1<<(window_size-1);
   int tslot_size = tslot_point * (elmLen*2);

   BNU_CHUNK_T* negtmp = cpGFpGetPool(1, pGFE);  /* temporary element */
   BNU_CHUNK_T* pointT = cpEcGFpGetPool(1, pEC); /* temporary point */

   Ipp8u digit, sign;
   int mask = (1<<(window_size+1)) -1;
   int bit = 0;

   /* processing of window[0] */
   int wvalue = *((Ipp16u*)&pScalar8[0]);
   wvalue = (wvalue << 1) & mask;

   booth_recode(&sign, &digit, (Ipp8u)wvalue, window_size);
   select_affine_point(pRdata, pTbl, digit);

   negF(negtmp, pRdata+elmLen, pGFE);
   cpMaskedReplace_ct(pRdata+elmLen, negtmp, elmLen, ~cpIsZero_ct(sign));
   cpGFpElementCopy(pRdata+elmLen*2, mont1, elmLen);
   cpGFpElementCopy(pointT+elmLen*2, mont1, elmLen);

   /* processing of other windows.. [1],[2],... */
   for(bit+=window_size, pTbl+=tslot_size; bit<=scalarBitSize; bit+=window_size, pTbl+=tslot_size) {
      wvalue = *((Ipp16u*)&pScalar8[(bit-1)/8]);
      wvalue = (wvalue>> ((bit-1)%8)) & mask;

      booth_recode(&sign, &digit, (Ipp8u)wvalue, window_size);
      select_affine_point(pointT, pTbl, digit);

      negF(negtmp, pointT+elmLen, pGFE);
      cpMaskedReplace_ct(pointT+elmLen, negtmp, elmLen, ~cpIsZero_ct(sign));

      gfec_affine_point_add(pRdata, pRdata, pointT, pEC);
   }

   cpEcGFpReleasePool(1, pEC);
   cpGFpReleasePool(1, pGFE);
}
