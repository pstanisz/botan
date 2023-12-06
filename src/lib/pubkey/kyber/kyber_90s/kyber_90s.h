/*
 * Symmetric primitives for Kyber (90s mode)
 * (C) 2022 Jack Lloyd
 * (C) 2022 Hannes Rantzsch, René Meusel, neXenio GmbH
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#ifndef BOTAN_KYBER_90S_H_
#define BOTAN_KYBER_90S_H_

#include <botan/hash.h>
#include <botan/internal/aes_crystals_xof.h>

#include <botan/internal/kyber_symmetric_primitives.h>

#include <array>
#include <memory>

namespace Botan {

class Kyber_90s_Symmetric_Primitives : public Kyber_Symmetric_Primitives {
   public:
      Kyber_90s_Symmetric_Primitives() :
            m_sha512(HashFunction::create_or_throw("SHA-512")),
            m_sha256(HashFunction::create_or_throw("SHA-256")),
            m_aes256_ctr_xof(std::make_unique<AES_256_CTR_XOF>()),
            m_aes256_ctr_prf(std::make_unique<AES_256_CTR_XOF>()) {}

      std::unique_ptr<HashFunction> G() const override { return m_sha512->new_object(); }

      std::unique_ptr<HashFunction> H() const override { return m_sha256->new_object(); }

      std::unique_ptr<HashFunction> KDF() const override { return m_sha256->new_object(); }

<<<<<<< HEAD
      Botan::XOF& XOF(std::span<const uint8_t> seed, std::tuple<uint8_t, uint8_t> mpos) const override {
         m_aes256_ctr_xof->clear();
         const std::array<uint8_t, 12> iv{std::get<0>(mpos), std::get<1>(mpos), 0};
         m_aes256_ctr_xof->start(iv, seed);
         return *m_aes256_ctr_xof;
=======
      std::unique_ptr<Kyber_XOF> XOF(Botan::span<const uint8_t> seed) const override {
         class Kyber_90s_XOF final : public Kyber_XOF {
            public:
               Kyber_90s_XOF(std::unique_ptr<StreamCipher> cipher, Botan::span<const uint8_t> seed) :
                     m_cipher(std::move(cipher)) {
                  m_cipher->set_key(seed);
               }

               void set_position(const std::tuple<uint8_t, uint8_t>& matrix_position) override {
                  std::array<uint8_t, 12> iv = {std::get<0>(matrix_position), std::get<1>(matrix_position), 0};
                  m_cipher->set_iv(iv.data(), iv.size());
               }

               void write_output(Botan::span<uint8_t> out) override { m_cipher->write_keystream(out.data(), out.size()); }

            private:
               std::unique_ptr<StreamCipher> m_cipher;
         };

         return std::make_unique<Kyber_90s_XOF>(m_aes256_ctr->new_object(), seed);
>>>>>>> 1937774b4 ([c++17] Botan 3.1.1 backported to C++17)
      }

      secure_vector<uint8_t> PRF(Botan::span<const uint8_t> seed,
                                 const uint8_t nonce,
                                 const size_t outlen) const override {
         m_aes256_ctr_prf->clear();
         const std::array<uint8_t, 12> nonce_buffer{nonce, 0};
         m_aes256_ctr_prf->start(nonce_buffer, seed);
         return m_aes256_ctr_prf->output(outlen);
      }

   private:
      std::unique_ptr<HashFunction> m_sha512;
      std::unique_ptr<HashFunction> m_sha256;
      std::unique_ptr<AES_256_CTR_XOF> m_aes256_ctr_xof;
      std::unique_ptr<AES_256_CTR_XOF> m_aes256_ctr_prf;
};

}  // namespace Botan

#endif
