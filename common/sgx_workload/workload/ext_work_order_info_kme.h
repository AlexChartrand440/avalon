/* Copyright 2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <stdlib.h>

#include "types.h"
#include "ext_work_order_info_impl.h"

class ExtWorkOrderInfoKME: virtual public ExtWorkOrderInfoImpl {
public:
    ExtWorkOrderInfoKME(void);

    ~ExtWorkOrderInfoKME(void);
    /* Prameteres:
       type – defines what key type to generate, only KeyType_SECP256K1
              is curretly supported
       nonce_hex – Nonce to use as a part of signature returned in vk_sig
       signing_key [OUT] - Randomly generated private signing key as bytes
       verification_key_hex [OUT] - Corresponding public verification key
                as hex string
       verification_key_signature_hex – signature(base64 string) of nonce_hex
                and verification_key_hex signed by the KME private signing key
       returns:
            zero on success or an error code otherwise
    */ 
    int GenerateSigningKey(KeyType type,
        const ByteArray& nonce_hex,
        ByteArray& signing_key,
        ByteArray& verification_key_hex,
        ByteArray& verification_key_signature_hex);

    /* This function is called by the KME workload to verify
       attestation info for the associated WPE.
       
    Parameters:
        attestation_data - attestation data to verify
        hex_id - id of the remote enclave as hex string. 
                 It must match REPORTDATA[32, 63] in attestation_data
        mrenclave [OUT] - MRENCLAVE value from the attestation_data
                          on success or not used
        mrsigner [OUT] - MRSIGNER value from the attestation_data
                         on success or not used
        encryption_public_key [OUT] - public encryption key from the
                      attestation_data on success or not used
        verification_key [OUT] - public verification key from
                      attestation_data on success or not used
    Returns:
        zero on success or an error code otherwise
    */
    int VerifyAttestaionWpe(const ByteArray& attestation_data,
        const ByteArray& hex_id,
        ByteArray& mrenclave,
        ByteArray& mrsigner,
        ByteArray& encryption_public_key,
        ByteArray& verification_key);

    /*
      Creates workorder key data to be returned to the WPE in json format
      {
        “signature”: <base64 string - signature using kme_skey (see notes below)>
        "sym-encryption-key": <base64 string - symmetric encryption key
              encrypted with WPE public encryption key>,
        "encrypted-wo-key": <base64 string - encrypted one time symmetric key
              in work order>,
        "wo-signing-key": <base64 string - signing key encrypted with
              sym-encryption-key>,
        "wo-verification-key": <base64 string - verifying key corresponding to
              above generated signing key>,
        "wo-verification-key-sig": <base64 string - signature of verification-key
              signed by the KME, see below>,
    	  "input-data-keys": [
        {
    		    "index": <integer matching index the workorder input data>
            "key": <base64 string encrypted with sym-encryption-key>
        },
    	     . . .
    	  ],
    	  "output-data-keys": [
        {
    		    "index": <integer matching index the workorder output data>
            "key": <base64 string encrypted with sym-encryption-key>
        },
        . ..
        ]
      }

        Parameters:
          wpe_encryption_key - WPE encryption key to encrypt
                    "sym-encryption-key" above
          kme_signing_key - this KME's signing key for the WPE retreived during
                    GenerateSigningKey() API
          work_order_key_data - [OUT] work order key info in json
                    as described above
        Returns:
          zero on success or an error code otherwise
    */
    int CreateWorkorderKeyInfo(const ByteArray& wpe_encryption_key,
        const ByteArray& kme_signing_key,
        ByteArray& work_order_key_data);

    /* Reserved function that verifies attestation info of (another) KME.
       Parameters:
           attestation_data - attestation to verify
           mrenclave [OUT] - MRENCLAVE value from the attestation_data
                  on success or not used
           mrsigner [OUT] - MRSIGNER value from the attestation_data
                  on success or not used
           encryption_public_key [OUT] - public encryption key from
                  attestation_data on success or not used
           verification_key [OUT] - public verification key from
                  attestation_data on success or not used

       Returns:
           zero on success or an error code otherwise
    */
    bool CheckAttestationSelf(const ByteArray& att_info,
        ByteArray& mrenclave,
        ByteArray& mrsigner,
        ByteArray& verification_key,
        ByteArray& encryption_public_key);
};
