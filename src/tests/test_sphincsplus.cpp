/*
* (C) 2023 Jack Lloyd
*     2023 Fabian Albert, René Meusel, Amos Treiber - Rohde & Schwarz Cybersecurity
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#include "tests.h"
#include "test_rng.h"

#if defined(BOTAN_HAS_SPHINCS_PLUS) && defined (BOTAN_HAS_SHA2_32)

#include <botan/hash.h>
#include <botan/hex.h>

#include <botan/internal/loadstor.h>
#include <botan/assert.h>
#include <botan/internal/sp_hash.h>
#include <botan/sp_parameters.h>
#include <botan/sphincsplus.h>
#include <botan/secmem.h>
#include <botan/pubkey.h>
#include <botan/pk_algs.h>
#include <botan/hash.h>


namespace Botan_Tests {

class SPHINCS_Plus_Test final : public Text_Based_Test
   {
   private:
      /// @returns (secret_seed, sk_prf, public_seed, sphincs_root)
      std::tuple<Botan::secure_vector<uint8_t>, Botan::secure_vector<uint8_t>, std::vector<uint8_t>, std::vector<uint8_t>>
      parse_sk(std::vector<uint8_t> sk, Botan::Sphincs_Parameters& params)
         {
         BOTAN_ASSERT_NOMSG(sk.size() == 4 * params.n());
         Botan::secure_vector<uint8_t> secret_seed(sk.begin(), sk.begin() + params.n());
         Botan::secure_vector<uint8_t> sk_prf(sk.begin() + params.n(), sk.begin() + 2*params.n());
         std::vector<uint8_t> public_seed(sk.begin() + 2*params.n(), sk.begin() + 3*params.n());
         std::vector<uint8_t> sphincs_root(sk.begin() + 3*params.n(), sk.end());

         return std::make_tuple(secret_seed, sk_prf, public_seed, sphincs_root);
         }

      std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
      parse_signature_with_message(std::vector<uint8_t> sig_with_msg, size_t msg_size, Botan::Sphincs_Parameters& params)
         {
         BOTAN_ASSERT_NOMSG(sig_with_msg.size() == params.sphincs_signature_bytes() + msg_size);
         std::vector<uint8_t> signature(sig_with_msg.begin(), sig_with_msg.begin() + params.sphincs_signature_bytes());
         std::vector<uint8_t> message(sig_with_msg.begin() + params.sphincs_signature_bytes(), sig_with_msg.end());

         return std::make_pair(signature, message);
         }

   public:
      SPHINCS_Plus_Test()
         : Text_Based_Test("pubkey/sphincsplus.vec", "SphincsParameterSet,seed,pk,sk,msg,sm,opt_rand")
      {}

      Test::Result run_one_test(const std::string&, const VarMap& vars) final
         {
         Test::Result result("SPHINCS+ ");

         auto params = Botan::Sphincs_Parameters::create(vars.get_req_str("SphincsParameterSet"));

         const std::vector<uint8_t> seed_ref = vars.get_req_bin("seed");
         const std::vector<uint8_t> msg_ref = vars.get_req_bin("msg");
         const std::vector<uint8_t> pk_ref = vars.get_req_bin("pk");
         const std::vector<uint8_t> sk_ref = vars.get_req_bin("sk");
         const std::vector<uint8_t> sig_msg_ref = vars.get_req_bin("sm");

         const std::vector<uint8_t> sig_ref(sig_msg_ref.begin(), sig_msg_ref.end() - msg_ref.size());

         auto [sk_seed, sk_prf, pk_seed, pk_root] = parse_sk(sk_ref, params);

         /*
          * To get the optional randomness from the given seed (from KAT), we need to create the
          * CTR_DRBG_AES256 rng and simulate the first call creating (sk_seed || sk_prf || pk_seed).
          * The next rng call in the reference implementation creates the optional randomness.
          */
         auto kat_rng = CTR_DRBG_AES256(seed_ref);
         kat_rng.random_vec<std::vector<uint8_t>>(3*params.n());
         std::vector<uint8_t> opt_rand = kat_rng.random_vec<std::vector<uint8_t>>(1*params.n());

         Fixed_Output_RNG fixed_rng;
         auto add_entropy = [&](auto v)
            { fixed_rng.add_entropy(v.data(), v.size()); };

         // The order of the RNG values is dependent on the order they are pulled
         // from the RNG in the production implementation.
         auto random_input(sk_ref);
         add_entropy(sk_seed);
         add_entropy(sk_prf);
         add_entropy(pk_seed);
         add_entropy(opt_rand);

         Botan::SphincsPlus_PrivateKey priv_key(fixed_rng, params);

         const std::string param = "Randomized";

         auto signer = Botan::PK_Signer(priv_key, fixed_rng, param);
         auto signature = signer.sign_message(msg_ref.data(), msg_ref.size(), fixed_rng);

         result.test_is_eq("signature creation", signature, sig_ref);

         Botan::PK_Verifier verifier(*priv_key.public_key(), params.algorithm_identifier());
         bool verify_success = verifier.verify_message(msg_ref.data(), msg_ref.size(), signature.data(), signature.size());
         result.test_is_eq("verification of valid signature", verify_success, true);

         return result;
         }

      bool skip_this_test(const std::string&,
                          const VarMap& vars) override
         {
         auto params = Botan::Sphincs_Parameters::create(vars.get_req_str("SphincsParameterSet"));
         return Botan::HashFunction::create(params.hash_name()) == nullptr;
         }
   };

   BOTAN_REGISTER_TEST("pubkey", "sphincsplus", SPHINCS_Plus_Test);

}  // namespace Botan_Tests

#endif  // BOTAN_HAS_SPHINCS_PLUS