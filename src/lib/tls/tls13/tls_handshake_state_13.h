/*
* TLS handshake state (machine) implementation for TLS 1.3
* (C) 2022 Jack Lloyd
*     2022 Hannes Rantzsch, René Meusel - neXenio GmbH
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_TLS_HANDSHAKE_STATE_13_H_
#define BOTAN_TLS_HANDSHAKE_STATE_13_H_

#include <botan/optional.h>
#include <variant>
#include <vector>
#include <functional>

#include <botan/tls_magic.h>
#include <botan/tls_messages.h>
#include <botan/tls_exceptn.h>
#include <botan/internal/stl_util.h>

namespace Botan::TLS {

namespace Internal {
class BOTAN_TEST_API Handshake_State_13_Base
   {
   public:
      bool has_client_hello() const { return m_client_hello.has_value(); }
      bool has_server_hello() const { return m_server_hello.has_value(); }
      bool has_server_certificate_chain() const { return m_server_certificate.has_value(); }
      bool has_client_certificate_chain() const { return m_client_certificate.has_value(); }
      bool has_hello_retry_request() const { return m_hello_retry_request.has_value(); }
      bool has_certificate_request() const { return m_certificate_request.has_value(); }
      bool has_server_finished() const { return m_server_finished.has_value(); }
      bool has_client_finished() const { return m_client_finished.has_value(); }

      bool handshake_finished() const { return has_server_finished() && has_client_finished(); }

      // Client_Hello_13 cannot be const because it might need modification due to a Hello_Retry_Request
      Client_Hello_13&              client_hello() { return get(m_client_hello); }
      const Client_Hello_13&        client_hello() const { return get(m_client_hello); }
      const Server_Hello_13&        server_hello() const { return get(m_server_hello); }
      const Hello_Retry_Request&    hello_retry_request() const { return get(m_hello_retry_request); }
      const Encrypted_Extensions&   encrypted_extensions() const { return get(m_encrypted_extensions); }
      const Certificate_Request_13& certificate_request() const { return get(m_certificate_request); }
      const Certificate_13&         server_certificate() const { return get(m_server_certificate); }
      const Certificate_13&         client_certificate() const { return get(m_client_certificate); }
      const Certificate_Verify_13&  server_certificate_verify() const { return get(m_server_certificate_verify); }
      const Certificate_Verify_13&  client_certificate_verify() const { return get(m_client_certificate_verify); }
      const Finished_13&            server_finished() const { return get(m_server_finished); }
      const Finished_13&            client_finished() const { return get(m_client_finished); }

   protected:
      Handshake_State_13_Base(Connection_Side whoami) : m_side(whoami) {}

      Client_Hello_13&        store(Client_Hello_13 client_hello, const bool from_peer);
      Client_Hello_12&        store(Client_Hello_12 client_hello, const bool from_peer);
      Server_Hello_13&        store(Server_Hello_13 server_hello, const bool from_peer);
      Server_Hello_12&        store(Server_Hello_12 server_hello, const bool from_peer);
      Hello_Retry_Request&    store(Hello_Retry_Request hello_retry_request, const bool from_peer);
      Encrypted_Extensions&   store(Encrypted_Extensions encrypted_extensions, const bool from_peer);
      Certificate_Request_13& store(Certificate_Request_13 certificate_request, const bool from_peer);
      Certificate_13&         store(Certificate_13 certificate, const bool from_peer);
      Certificate_Verify_13&  store(Certificate_Verify_13 certificate_verify, const bool from_peer);
      Finished_13&            store(Finished_13 finished, const bool from_peer);

   private:
      template<typename MessageT>
      const MessageT& get(const Botan::optional<MessageT>& opt) const
         {
         if(!opt.has_value())
            { throw Invalid_State("TLS handshake message not set"); }
         return opt.value();
         }

      template<typename MessageT>
      MessageT& get(Botan::optional<MessageT>& opt)
         {
         if(!opt.has_value())
            { throw Invalid_State("TLS handshake message not set"); }
         return opt.value();
         }

      Connection_Side m_side;

      Botan::optional<Client_Hello_13> m_client_hello;
      Botan::optional<Client_Hello_12> m_client_hello_12;
      Botan::optional<Server_Hello_13> m_server_hello;
      Botan::optional<Server_Hello_12> m_server_hello_12;
      Botan::optional<Hello_Retry_Request> m_hello_retry_request;
      Botan::optional<Encrypted_Extensions> m_encrypted_extensions;
      Botan::optional<Certificate_Request_13> m_certificate_request;
      Botan::optional<Certificate_13> m_server_certificate;
      Botan::optional<Certificate_13> m_client_certificate;
      Botan::optional<Certificate_Verify_13> m_server_certificate_verify;
      Botan::optional<Certificate_Verify_13> m_client_certificate_verify;
      Botan::optional<Finished_13> m_server_finished;
      Botan::optional<Finished_13> m_client_finished;
   };
}

/**
 * Place to store TLS handshake messages
 *
 * This class is used to keep all handshake messages that have been received from and sent to
 * the peer as part of the TLS 1.3 handshake. Getters are provided for all message types.
 * Specializations for the client and server side provide specific setters in the form of
 * `sent` and `received` that only allow those types of handshake messages that are sensible
 * for the respective connection side.
 *
 * The handshake state machine as described in RFC 8446 Appendix A is NOT validated here.
 */
template <Connection_Side whoami,
          typename Outbound_Message_T,
          typename Inbound_Message_T,
          typename Inbound_Post_Handshake_Message_T>
class BOTAN_TEST_API Handshake_State_13 : public Internal::Handshake_State_13_Base
   {
   public:
      Handshake_State_13() : Handshake_State_13_Base(whoami) {}

      template<typename MsgT>
      std::reference_wrapper<MsgT> sending(MsgT msg)
         {
         static_assert(std::is_constructible<Outbound_Message_T, MsgT>::value,
                       "Cannot send handshake message of type MsgT");

         return std::reference_wrapper<decltype(msg)>(store(std::move(msg), false));
         }

      template<typename... MsgTs>
      decltype(auto) sending(Botan::variant<MsgTs...> message)
         {
         static_assert(is_generalizable_to<Outbound_Message_T>(message),
                       "Cannot send handshake message of types MsgTs...");

         return std::visit([&](auto msg) -> as_wrapped_references_t<Botan::variant<MsgTs...>>
            {
            return sending(std::move(msg));
            }, std::move(message));
         }

      decltype(auto) received(Handshake_Message_13 message)
         {
         return std::visit([&](auto msg) -> as_wrapped_references_t<Inbound_Message_T>
            {
            if constexpr(std::is_constructible<Inbound_Message_T, decltype(msg)>::value)
               {
               return std::reference_wrapper<decltype(msg)>(store(std::move(msg), true));
               }

            throw TLS_Exception(AlertType::UnexpectedMessage, "received an illegal handshake message");
            }, std::move(message));
         }

      decltype(auto) received(Post_Handshake_Message_13 message)
         {
         return std::visit([](auto msg) -> Inbound_Post_Handshake_Message_T
            {
            if constexpr(std::is_constructible<Inbound_Post_Handshake_Message_T, decltype(msg)>::value)
               {
               return msg;
               }

            throw TLS_Exception(AlertType::UnexpectedMessage, "received an unexpected post-handshake message");
            }, std::move(message));
         }
   };

using Client_Handshake_State_13 = Handshake_State_13<Connection_Side::Client,
      Client_Handshake_13_Message,
      Server_Handshake_13_Message,
      Server_Post_Handshake_13_Message>;

using Server_Handshake_State_13 = Handshake_State_13<Connection_Side::Server,
      Server_Handshake_13_Message,
      Client_Handshake_13_Message,
      Client_Post_Handshake_13_Message>;
}

#endif
