/*******************************************************************************
* Copyright 2002-2019 Intel Corporation
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
// Montgomery engine preparation (GetSize/init/Set)
*/

/*
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
// 
//     Context:
//        gsMethod_RSA_gpr_private()
//
*/

#include "owncp.h"
#include "pcpngmontexpstuff.h"
#include "gsscramble.h"
#include "pcpngrsamethod.h"
#include "pcpngrsa.h"

#if (_IPP32E>=_IPP32E_K0)
#include "pcpngmontexpstuff_avx512.h"

gsMethod_RSA* gsMethod_RSA_avx512_private(void)
{
   static gsMethod_RSA m = {
      RSA_AVX512_MIN_BITSIZE, RSA_AVX512_MAX_BITSIZE, /* RSA range */

      /* private key exponentiation: private, window, avx512 */
      gsMontExpWinBuffer_avx512,
      gsMontExpWin_BNU_sscm_avx512
   };
   return &m;
}
#endif /* _IPP32E_K0 */
