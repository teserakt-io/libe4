
#ifndef E4_PUBKEY_H
#define E4_PUBKEY_H

int e4c_pubkey_protect_message(uint8_t *ciphertext,
                        size_t ciphertext_max_len,
                        size_t *ciphertext_len,
                        const uint8_t *message,
                        size_t message_len,
                        const char *topic_name,
                        e4storage *storage,
                        const uint32_t proto_opts);

int e4c_pubkey_unprotect_message(uint8_t *message,
                          size_t message_max_len,
                          size_t *message_len,
                          const uint8_t *ciphertext,
                          size_t ciphertext_len,
                          const char *topic_name,
                          e4storage *storage,
                          const uint32_t proto_opts);

int e4c_pubkey_c2sharedsecret_derivestore(e4storage* storage);

#endif
