
#ifndef E4_ERROR_H
#define E4_ERROR_H

/* E4 Library Error codes */
/* Operation succeeded without error  */
#define E4_RESULT_OK 0
/* A control message was handled. Applications may discard the resulting buffer */
#define E4_RESULT_OK_CONTROL -1
/* Internal error: for exception conditions that indicate the code has a bug */
#define E4_ERROR_INTERNAL -100
/* Invalid authentication tag indicates corrupted ciphertext  */
#define E4_ERROR_INVALID_TAG -101
/* Message received outside of error window for clock.  */
#define E4_ERROR_TIMESTAMP_IN_FUTURE -102
/* Message received outside of error window for clock.  */
#define E4_ERROR_TIMESTAMP_TOO_OLD -103
/* Ciphertext too short.  */
#define E4_ERROR_CIPHERTEXT_TOO_SHORT -104
/* Unable to find key for topic; could not decrypt. */
#define E4_ERROR_TOPICKEY_MISSING -105
/* Message has already been seen.  */
#define E4_ERROR_MESSAGE_REPLAYED -106
/* E4 Protocol command invalid. Internal error.  */
#define E4_ERROR_INVALID_COMMAND -107
/* E4 Persistence layer reported an error.  */
#define E4_ERROR_PERSISTENCE_ERROR -108
/* Unable to find public key for device;e4 */
#define E4_ERROR_DEVICEPK_MISSING -109
/* Signature verification failed */
#define E4_ERROR_PK_SIGVERIF_FAILED -110
/* Overflow detected */
#define E4_ERROR_PARAMETER_OVERFLOW -111
/* Invalid parameters, e.g. NULL pointers */
#define E4_ERROR_PARAMETER_INVALID -112
/* Incompatible storage type */
#define E4_ERROR_PERSISTENCE_INCOMPATIBLE -113

#endif /* include guard */
