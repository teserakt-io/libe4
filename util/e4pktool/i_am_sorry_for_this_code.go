package main

import (
	"bytes"
	"crypto/rand"
	"encoding/binary"
	//"encoding/hex"
	//"encoding/json"
	"fmt"
	"io"
	"os"
	//"sync"
	"errors"
	"strings"
	"time"

	//"golang.org/x/crypto/argon2"
	//miscreant "github.com/miscreant/miscreant.go"
	"github.com/agl/ed25519/extra25519"
	"golang.org/x/crypto/curve25519"
	"golang.org/x/crypto/ed25519"

	e4 "github.com/teserakt-io/e4go"
	e4crypto "github.com/teserakt-io/e4go/crypto"
)

var (
	ErrInvalidSignature    = errors.New("Invalid Signature")
	ErrInvalidProtectedLen = errors.New("Invalid Protection Length")
)

func printBinaryAsCArray(w io.Writer, lovelyslice []byte) {

	slicelen := len(lovelyslice)
	fmt.Fprintf(w, "{")
	for i, b := range lovelyslice {
		fmt.Fprintf(w, "0x%02x", b)
		if i != slicelen-1 {
			fmt.Fprintf(w, ", ")
		}
	}
	fmt.Fprintf(w, "}")
}

func ProtectPubMessage(payload []byte, symkey []byte, signerid []byte, EdPrivateKey ed25519.PrivateKey) ([]byte, error) {
	timestamp := make([]byte, e4crypto.TimestampLen)
	binary.LittleEndian.PutUint64(timestamp, uint64(time.Now().Unix()))

	ct, err := e4crypto.Encrypt(symkey, timestamp, payload)
	if err != nil {
		return nil, err
	}

	protected := append(timestamp, signerid...)
	protected = append(protected, ct...)

	// sig should always be ed25519.SignatureSize=64 bytes
	sig := ed25519.Sign(EdPrivateKey, protected)
	if len(sig) != ed25519.SignatureSize {
		return nil, ErrInvalidSignature
	}

	protected = append(protected, sig...)
	fmt.Println(len(sig))
	fmt.Println(protected)
	protectedLen := e4crypto.TimestampLen + e4crypto.IDLen + e4crypto.TagLen + len(payload) + ed25519.SignatureSize
	if protectedLen != len(protected) {
		return nil, e4crypto.ErrInvalidProtectedLen
	}
	fmt.Println(len(protected))

	/* self decryption test */
	if err = DecryptPubMessageTest(payload, protected, symkey); err != nil {
		return nil, err
	}

	return protected, nil
}

func DecryptPubMessageTest(payload []byte, ct []byte, symkey []byte) error {

	// message format: Timestamp(8)|ID(16)|TAG(16)|CT(n)|Sig(64)

	if len(ct) < e4crypto.TimestampLen+e4crypto.IDLen+e4crypto.TagLen+ed25519.SignatureSize {
		return errors.New("Decrypt: ct too small")
	}

	ctlen := len(ct)
	sivct := ct[:ctlen-ed25519.SignatureSize]
	headerlen := e4crypto.TimestampLen + e4crypto.IDLen
	adlen := e4crypto.TimestampLen

	recovered, err := e4crypto.Decrypt(symkey, sivct[:adlen], sivct[headerlen:])
	if err != nil {
		return fmt.Errorf("DecryptPubMessage: %v", err)
	}
	if !bytes.Equal(recovered, payload) {
		return errors.New("Decrypt: plaintext did not equal recovered value")
	}
	return nil
}

func DecryptSymMessageTest(payload []byte, ct []byte, symkey []byte) error {

	// message format: Timestamp(8)|TAG(16)|CT(n)

	if len(ct) < e4crypto.TimestampLen+e4crypto.TagLen {
		return errors.New("Decrypt: ct too small")
	}

	headerlen := e4crypto.TimestampLen

	recovered, err := e4crypto.Decrypt(symkey, ct[:headerlen], ct[headerlen:])
	if err != nil {
		return fmt.Errorf("DecryptSymMessage: %v", err)
	}
	if !bytes.Equal(recovered, payload) {
		return errors.New("Decrypt: plaintext did not equal recovered value")
	}
	return nil
}

func ProtectSymMessage(payload, key []byte) ([]byte, error) {
	timestamp := make([]byte, e4crypto.TimestampLen)
	binary.LittleEndian.PutUint64(timestamp, uint64(time.Now().Unix()))

	ct, err := e4crypto.Encrypt(key, timestamp, payload)
	if err != nil {
		return nil, err
	}
	protected := append(timestamp, ct...)

	protectedLen := e4crypto.TimestampLen + len(payload) + e4crypto.TagLen
	if protectedLen != len(protected) {
		return nil, ErrInvalidProtectedLen
	}

	if err = DecryptSymMessageTest(payload, protected, key); err != nil {
		panic(err)
	}

	return protected, nil
}

func ProtectPubCommand(payload []byte, deviceEdPublicKey ed25519.PublicKey, C2EdPrivateKey ed25519.PrivateKey, sharedkeycheck []byte) ([]byte, error) {

	var c2Ed25519sk_bytes [64]byte
	var deviceEd25519pk_bytes [32]byte

	var c2c25519sk_bytes [32]byte
	var devicec25519pk_bytes [32]byte
	var sharedpoint [32]byte

	//
	copy(c2Ed25519sk_bytes[:], C2EdPrivateKey)
	copy(deviceEd25519pk_bytes[:], deviceEdPublicKey)

	extra25519.PrivateKeyToCurve25519(&c2c25519sk_bytes, &c2Ed25519sk_bytes)
	extra25519.PublicKeyToCurve25519(&devicec25519pk_bytes, &deviceEd25519pk_bytes)

	// C2 knows its sk, sends to dv pk
	curve25519.ScalarMult(&sharedpoint, &c2c25519sk_bytes, &devicec25519pk_bytes)
	derivedSharedkey := e4crypto.Sha3Sum256(sharedpoint[:])[:e4crypto.KeyLen]

	if !bytes.Equal(derivedSharedkey, sharedkeycheck) {
		return nil, errors.New("Derived shared key is wrong")
	}
	payload, err := ProtectSymMessage(payload, derivedSharedkey)
	return payload, err
}

// RemoveTopic + topichash
// ResetTopics
// SetIDKey + key
// SetTopicKey + key + topichash
// RemovePubKey + id
// ResetPubKeys
// SetPubKey + key + id

func MakeCmdRemoveTopic(topichash [16]byte) []byte {
	cmd := e4.RemoveTopic
	payload := append([]byte{cmd.ToByte()}, topichash[:]...)
	return payload
}

func MakeCmdResetTopics() []byte {
	cmd := e4.ResetTopics
	payload := []byte{cmd.ToByte()}
	return payload
}

func MakeCmdSetIDKey(key [32]byte) []byte {
	cmd := e4.SetIDKey
	payload := append([]byte{cmd.ToByte()}, key[:]...)
	return payload
}

func MakeCmdSetTopicKey(key [32]byte, topichash [16]byte) []byte {
	cmd := e4.SetTopicKey
	payload := append(append([]byte{cmd.ToByte()}, key[:]...), topichash[:]...)
	return payload
}

func MakeCmdRemovePubKey(id [e4crypto.IDLen]byte) []byte {
	cmd := e4.RemovePubKey
	payload := append([]byte{cmd.ToByte()}, id[:]...)
	return payload
}

func MakeCmdResetPubKeys() []byte {
	cmd := e4.ResetPubKeys
	payload := []byte{cmd.ToByte()}
	return payload
}

func MakeCmdSetPubKey(key [32]byte, id [16]byte) []byte {
	cmd := e4.SetPubKey
	payload := append(append([]byte{cmd.ToByte()}, key[:]...), id[:]...)
	return payload
}

func WriteHeaderPrologue(w io.Writer, numpkkats int) {

	prologue := `/* This file is autogenerated. Please do not modify it directly. */
#ifndef E4_PK_KAT
#define E4_PK_KAT

typedef struct _e4pk_cmd_kat {
    const uint8_t c2_edwards_pubkey[32];
    const uint8_t c2_edwards_seckey[64];
    const uint8_t c2_montgom_pubkey[32];
    const uint8_t c2_montgom_seckey[32];
    
    const uint8_t dev_edwards_pubkey[32];
    const uint8_t dev_edwards_seckey[64];
    const uint8_t dev_montgom_pubkey[32];
    const uint8_t dev_montgom_seckey[32];

    const uint8_t c2_sharedkey[32];
    const uint8_t dev_sharedkey[32];

    const uint8_t deviceid[16];
    const uint8_t otherdeviceid[16];
    const uint8_t otherdevicepk[32];

	const uint8_t* cmd_resettopics;
	const size_t cmd_resettopics_len;
	const uint8_t* cmd_removetopic;
	const size_t cmd_removetopic_len;
	const uint8_t* cmd_settopickey;
	const size_t cmd_settopickey_len;
	const uint8_t* cmd_resetpubkeys;
	const size_t cmd_resetpubkeys_len;
	const uint8_t* cmd_removepubkey;
	const size_t cmd_removepubkey_len;
	const uint8_t* cmd_setpubkey;
	const size_t cmd_setpubkey_len;
} e4pk_cmd_kat;

typedef struct _e4pk_topicmsg_kat {
    const uint8_t otherdevice_seckey[64];
    const uint8_t otherdevice_pubkey[32];
    const uint8_t otherdevice_id[16];
    const uint8_t plaintext[32];
    const uint8_t e4_ciphertext[136];
    const uint8_t topickey[32];
    const char topicname[21];
} e4pk_topicmsg_kat;

`
	fmt.Fprint(w, prologue)
	fmt.Fprint(w, "\n")

	fmt.Fprint(w, "\n")
}

func WriteHeaderEpilogue(w io.Writer) {

	epilogue := `

#endif /*E4_PK_KAT*/

`
	fmt.Fprint(w, epilogue)
	fmt.Fprint(w, "\n")
}

func StartC2CommandTranscript(w io.Writer, numpkkats int) {
	fmt.Fprintf(w, "e4pk_cmd_kat pkkat[%d] = {\n", numpkkats)
}

func EndC2CommandTranscript(w io.Writer) {
	fmt.Fprint(w, "};\n")
}

func StartTopicMsgTranscript(w io.Writer, numpkkats int) {
	fmt.Fprintf(w, "e4pk_topicmsg_kat topickat[%d] = {\n", numpkkats)
}

func EndTopicMsgTranscript(w io.Writer) {
	fmt.Fprint(w, "};\n")
}

// defines: anything that comes first

// defines: anything that comes first
// w: anything else to be written.
func GenerateC2CommandTranscript(defines io.Writer, w io.Writer, instance int) {

	var c2Ed25519pk_bytes [32]byte
	var c2Ed25519sk_bytes [64]byte
	var c2c25519pk_bytes [32]byte
	var c2c25519sk_bytes [32]byte

	var deviceEd25519pk_bytes [32]byte
	var deviceEd25519sk_bytes [64]byte
	var devicec25519pk_bytes [32]byte
	var devicec25519sk_bytes [32]byte

	// Generate a C2 Key
	c2Ed25519pk, c2Ed25519sk, _ := ed25519.GenerateKey(nil)

	// C2 key: extract byte repr
	copy(c2Ed25519pk_bytes[:], c2Ed25519pk)
	copy(c2Ed25519sk_bytes[:], c2Ed25519sk)

	// C2 key: montgomery form:
	extra25519.PublicKeyToCurve25519(&c2c25519pk_bytes, &c2Ed25519pk_bytes)
	extra25519.PrivateKeyToCurve25519(&c2c25519sk_bytes, &c2Ed25519sk_bytes)

	// Device key:
	deviceEd25519pk, deviceEd25519sk, _ := ed25519.GenerateKey(nil)

	// Device key: extract byte repr
	copy(deviceEd25519pk_bytes[:], deviceEd25519pk)
	copy(deviceEd25519sk_bytes[:], deviceEd25519sk)

	// Device key: montgomery form:
	extra25519.PublicKeyToCurve25519(&devicec25519pk_bytes, &deviceEd25519pk_bytes)
	extra25519.PrivateKeyToCurve25519(&devicec25519sk_bytes, &deviceEd25519sk_bytes)

	// crypto that takes place on the C2:
	var onc2_sharedpoint [32]byte

	curve25519.ScalarMult(&onc2_sharedpoint, &c2c25519sk_bytes, &devicec25519pk_bytes)
	onc2_sharedkey := e4crypto.Sha3Sum256(onc2_sharedpoint[:])

	// crypto that takes place on the device:
	var ondev_sharedpoint [32]byte
	curve25519.ScalarMult(&ondev_sharedpoint, &devicec25519sk_bytes, &c2c25519pk_bytes)
	ondev_sharedkey := e4crypto.Sha3Sum256(onc2_sharedpoint[:])

	if !bytes.Equal(ondev_sharedkey, onc2_sharedkey) {
		panic("ECDH doesn't work... or rather we're stupid.")
	}

	// device the topic hash:
	devicename := fmt.Sprintf("pubkey_testdevice_%d", instance-1)
	deviceid := e4crypto.Sha3Sum256([]byte(devicename))[:e4crypto.IDLen]

	otherdevicename := fmt.Sprintf("pubkey_testanotherdevice_%d", instance-1)
	otherdeviceidslice := e4crypto.Sha3Sum256([]byte(otherdevicename))
	var otherdeviceid [e4crypto.IDLen]byte
	copy(otherdeviceid[:], otherdeviceidslice)

	var otherdevice_ed25519_pk [32]byte
	otherdevicepk, _, _ := ed25519.GenerateKey(nil)
	copy(otherdevice_ed25519_pk[:], otherdevicepk)

	var topickey [32]byte
	var topichash [16]byte
	topic := fmt.Sprintf("pubkey_testtopic_%d", instance-1)
	topichashslice := e4crypto.Sha3Sum256([]byte(topic))[:16]
	copy(topichash[:], topichashslice)

	topickeyslice := e4crypto.RandomKey()
	copy(topickey[:], topickeyslice)

	// now we make one of every command:
	cmdresettopics := MakeCmdResetTopics()
	cmdremovetopic := MakeCmdRemoveTopic(topichash)
	cmdsettopickey := MakeCmdSetTopicKey(topickey, topichash)
	cmdresetpubkeys := MakeCmdResetPubKeys()
	cmdremovepubkey := MakeCmdRemovePubKey(otherdeviceid)
	cmdsetpubkey := MakeCmdSetPubKey(otherdevice_ed25519_pk, otherdeviceid)
	//cmdsetidkey := MakeCmdSetIDKey(deviceEd25519sk_bytes)

	// now we generate encrypted versions of each command for testing purposes:
	e4resettopics, err := ProtectPubCommand(cmdresettopics, deviceEd25519pk, c2Ed25519sk, ondev_sharedkey)
	if err != nil {
		fmt.Println(err)
		return
	}

	e4removetopic, err := ProtectPubCommand(cmdremovetopic, deviceEd25519pk, c2Ed25519sk, ondev_sharedkey)
	if err != nil {
		fmt.Println(err)
		return
	}
	e4settopickey, err := ProtectPubCommand(cmdsettopickey, deviceEd25519pk, c2Ed25519sk, ondev_sharedkey)
	if err != nil {
		fmt.Println(err)
		return
	}
	e4resetpubkeys, err := ProtectPubCommand(cmdresetpubkeys, deviceEd25519pk, c2Ed25519sk, ondev_sharedkey)
	if err != nil {
		fmt.Println(err)
		return
	}
	e4removepubkey, err := ProtectPubCommand(cmdremovepubkey, deviceEd25519pk, c2Ed25519sk, ondev_sharedkey)
	if err != nil {
		fmt.Println(err)
		return
	}
	e4setpubkey, err := ProtectPubCommand(cmdsetpubkey, deviceEd25519pk, c2Ed25519sk, ondev_sharedkey)
	if err != nil {
		fmt.Println(err)
		return
	}

	//controltopic := fmt.Sprintf("e4/%02x", controltopichash)
	//fmt.Println(controltopic)

	fmt.Fprintf(w, "{\n")
	// let's make one of every comman

	fmt.Fprintf(w, ".c2_edwards_pubkey = ")
	printBinaryAsCArray(w, c2Ed25519pk_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".c2_edwards_seckey = ")
	printBinaryAsCArray(w, c2Ed25519sk_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".c2_montgom_pubkey = ")
	printBinaryAsCArray(w, c2c25519pk_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".c2_montgom_seckey = ")
	printBinaryAsCArray(w, c2c25519sk_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".dev_edwards_pubkey = ")
	printBinaryAsCArray(w, deviceEd25519pk_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".dev_edwards_seckey = ")
	printBinaryAsCArray(w, deviceEd25519sk_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".dev_montgom_pubkey = ")
	printBinaryAsCArray(w, devicec25519pk_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".dev_montgom_seckey = ")
	printBinaryAsCArray(w, devicec25519sk_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".c2_sharedkey = ")
	printBinaryAsCArray(w, onc2_sharedkey[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".dev_sharedkey = ")
	printBinaryAsCArray(w, ondev_sharedkey[:])
	fmt.Fprintf(w, ",\n")

	fmt.Fprintf(w, ".deviceid = ")
	printBinaryAsCArray(w, deviceid[:])
	fmt.Fprintf(w, ",\n")

	fmt.Fprintf(w, ".otherdeviceid = ")
	printBinaryAsCArray(w, otherdeviceid[:])
	fmt.Fprintf(w, ",\n")

	fmt.Fprintf(w, ".otherdevicepk = ")
	printBinaryAsCArray(w, otherdevice_ed25519_pk[:])
	fmt.Fprintf(w, ",\n")

	fmt.Fprintf(w, ".cmd_resettopics = E4_CMD_ENC_RESETTOPICS_%d,\n", instance-1)
	fmt.Fprintf(w, ".cmd_resettopics_len = %d,\n", len(e4resettopics))
	fmt.Fprintf(w, ".cmd_removetopic = E4_CMD_ENC_REMOVETOPIC_%d,\n", instance-1)
	fmt.Fprintf(w, ".cmd_removetopic_len = %d,\n", len(e4removetopic))
	fmt.Fprintf(w, ".cmd_settopickey = E4_CMD_ENC_SETTOPICKEY_%d,\n", instance-1)
	fmt.Fprintf(w, ".cmd_settopickey_len = %d,\n", len(e4settopickey))
	fmt.Fprintf(w, ".cmd_resetpubkeys = E4_CMD_ENC_RESETPUBKEYS_%d,\n", instance-1)
	fmt.Fprintf(w, ".cmd_resetpubkeys_len = %d,\n", len(e4resetpubkeys))
	fmt.Fprintf(w, ".cmd_removepubkey = E4_CMD_ENC_REMOVEPUBKEY_%d,\n", instance-1)
	fmt.Fprintf(w, ".cmd_removepubkey_len = %d,\n", len(e4removepubkey))
	fmt.Fprintf(w, ".cmd_setpubkey = E4_CMD_ENC_SETPUBKEY_%d,\n", instance-1)
	fmt.Fprintf(w, ".cmd_setpubkey_len = %d\n", len(e4setpubkey))
	fmt.Fprintf(w, "}")
	/* command declarations. For ease and laziness these stay as variables */

	fmt.Fprintf(defines, "const uint8_t E4_CMD_ENC_RESETTOPICS_%d[%d] = ", instance-1, len(e4resettopics))
	printBinaryAsCArray(defines, e4resettopics[:])
	fmt.Fprintf(defines, ";\n")
	fmt.Fprintf(defines, "const uint8_t E4_CMD_ENC_REMOVETOPIC_%d[%d] = ", instance-1, len(e4removetopic))
	printBinaryAsCArray(defines, e4removetopic[:])
	fmt.Fprintf(defines, ";\n")
	fmt.Fprintf(defines, "const uint8_t E4_CMD_ENC_SETTOPICKEY_%d[%d] = ", instance-1, len(e4settopickey))
	printBinaryAsCArray(defines, e4settopickey[:])
	fmt.Fprintf(defines, ";\n")
	fmt.Fprintf(defines, "const uint8_t E4_CMD_ENC_RESETPUBKEYS_%d[%d] = ", instance-1, len(e4resetpubkeys))
	printBinaryAsCArray(defines, e4resetpubkeys[:])
	fmt.Fprintf(defines, ";\n")
	fmt.Fprintf(defines, "const uint8_t E4_CMD_ENC_REMOVEPUBKEY_%d[%d] = ", instance-1, len(e4removepubkey))
	printBinaryAsCArray(defines, e4removepubkey[:])
	fmt.Fprintf(defines, ";\n")
	fmt.Fprintf(defines, "const uint8_t E4_CMD_ENC_SETPUBKEY_%d[%d] = ", instance-1, len(e4setpubkey))
	printBinaryAsCArray(defines, e4setpubkey[:])
	fmt.Fprintf(defines, ";\n")
}

func GenerateTopicTranscript(w io.Writer, instance int) {

	/*

		   Here's what we need to generate.
		   typedef struct _e4pk_topicmsg_kat {
		       const uint8_t otherdevice_seckey[64];
		       const uint8_t otherdevice_pubkey[32];
		   	   const uint8_t otherdevice_id[16];
		       const uint8_t plaintext[32];
			   const uint8_t e4_ciphertext[136];
		       const uint8_t topickey[32];
		   	   const string topicname;
		   } ed25519_topicmsg_kat;
	*/
	var otherdevice_edpk_bytes [32]byte
	var otherdevice_edsk_bytes [64]byte
	/* pubkey messages are: Timestamp (8) | id (16) | IV (16) | Ciphertext (n) | sig (64) */
	/* so 8+16+16+32+64*/
	var e4_ciphertext [136]byte
	var topicname = "publickey_test_topic"
	plaintext_bytes := make([]byte, 32)
	otherdevice_id_bytes := make([]byte, 16)

	topickey := e4crypto.RandomKey()
	fmt.Println("topickey_len=", len(topickey))
	_, err := rand.Read(otherdevice_id_bytes)
	if err != nil {
		fmt.Println(err)
		panic("Random failed")
	}
	_, err = rand.Read(plaintext_bytes)
	if err != nil {
		fmt.Println(err)
		panic("Random failed")
	}

	otherdevice_edpk, otherdevice_edsk, _ := ed25519.GenerateKey(nil)

	copy(otherdevice_edsk_bytes[:], otherdevice_edsk)
	copy(otherdevice_edpk_bytes[:], otherdevice_edpk)

	protected_message, err := ProtectPubMessage(plaintext_bytes, topickey, otherdevice_id_bytes, otherdevice_edsk)
	if err != nil {
		fmt.Println(err)
		panic("crypto failed")
	}
	if len(protected_message) != len(e4_ciphertext) {
		fmt.Println(err)
		panic("invalid ciphertext length")
	}

	copy(e4_ciphertext[:], protected_message)

	fmt.Fprintf(w, "{\n")
	fmt.Fprintf(w, ".otherdevice_seckey = ")
	printBinaryAsCArray(w, otherdevice_edsk_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".otherdevice_pubkey = ")
	printBinaryAsCArray(w, otherdevice_edpk_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".otherdevice_id = ")
	printBinaryAsCArray(w, otherdevice_id_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".plaintext = ")
	printBinaryAsCArray(w, plaintext_bytes[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".e4_ciphertext = ")
	printBinaryAsCArray(w, e4_ciphertext[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".topickey = ")
	printBinaryAsCArray(w, topickey[:])
	fmt.Fprintf(w, ",\n")
	fmt.Fprintf(w, ".topicname = \"%s\"", topicname)
	fmt.Fprintf(w, "}")

	return
}

func main() {

	const NUMKAT_PK int = 1024
	const NUMKAT_TOPIC int = 1024

	pubkey_cmd_vectors_h, err := os.OpenFile("pubkey_kat.h", os.O_RDWR|os.O_CREATE, 0644)
	if err != nil {
		fmt.Println(err)
		return
	}

	frontmatter := strings.Builder{}
	mdefines := strings.Builder{}
	inits := strings.Builder{}
	backmatter := strings.Builder{}

	WriteHeaderPrologue(&frontmatter, NUMKAT_PK)
	fmt.Fprintf(&mdefines, "#define NUM_PKCATS %d\n\n", NUMKAT_PK)
	fmt.Fprintf(&mdefines, "#define NUM_TOPICCATS %d\n\n", NUMKAT_TOPIC)
	StartC2CommandTranscript(&inits, NUMKAT_PK)
	for i := 1; i <= NUMKAT_PK; i++ {
		GenerateC2CommandTranscript(&mdefines, &inits, i)
		if i != NUMKAT_PK {
			fmt.Fprint(&inits, ",")
		}
		fmt.Fprint(&inits, "\n")
	}
	EndC2CommandTranscript(&inits)
	fmt.Fprint(&inits, "\n\n/* Topic Messages */\n\n")
	StartTopicMsgTranscript(&inits, NUMKAT_TOPIC)
	for i := 1; i <= NUMKAT_TOPIC; i++ {
		GenerateTopicTranscript(&inits, i)
		if i != NUMKAT_TOPIC {
			fmt.Fprint(&inits, ",")
		}
		fmt.Fprint(&inits, "\n")
	}
	EndTopicMsgTranscript(&inits)

	WriteHeaderEpilogue(&backmatter)

	fmt.Fprintf(pubkey_cmd_vectors_h, "%s", frontmatter.String())
	fmt.Fprintf(pubkey_cmd_vectors_h, "%s\n", mdefines.String())
	fmt.Fprintf(pubkey_cmd_vectors_h, "%s\n", inits.String())
	fmt.Fprintf(pubkey_cmd_vectors_h, backmatter.String())

	pubkey_cmd_vectors_h.Close()
}
