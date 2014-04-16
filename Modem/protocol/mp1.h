#ifndef PROTOCOL_MP1
#define PROTOCOL_MP1

#include <cfg/compiler.h>
#include <io/kfile.h>

// Frame sizing & checksum
#define MP1_MIN_FRAME_LENGTH 3
#define MP1_MAX_FRAME_LENGTH 250
#define MP1_INTERLEAVE_SIZE 3
#define MP1_CHECKSUM_INIT 0xAA

// We need to know some basic HDLC flag bytes
#define HDLC_FLAG  0x7E
#define HDLC_RESET 0x7F
#define AX25_ESC   0x1B

// We also define a few header flags and what
// to send as padding if we need to pad a
// packet. Due to forward error correction,
// packets must have an even number of bytes.
#define MP1_PADDING 			0x55
#define MP1_HEADER_PADDED		0x01
#define MP1_HEADER_COMPRESSION 	0x02

// Just a forward declaration that this struct exists
struct MP1Packet;

// The type of a callback function for passing
// back a decoded packet
typedef void (*mp1_callback_t)(struct MP1Packet *packet);

// Struct for a protocol context
typedef struct MP1 {
	uint8_t buffer[MP1_MAX_FRAME_LENGTH];		// A buffer for incoming packets
	uint8_t fecBuffer[3];						// FEC buffer
	KFile *modem;								// KFile access to the modem
	size_t packetLength;						// Counter for received packet length
	size_t readLength;							// This is the full read length, including parity bytes
	uint8_t calculatedParity;					// Calculated parity for incoming data block
	mp1_callback_t callback;					// The function to call when a packet has been received
	uint8_t checksum_in;						// Rolling checksum for incoming packets
	uint8_t checksum_out;						// Rolling checksum for outgoing packets
	bool reading;								// True when we have seen a HDLC flag
	bool escape;								// We need to know if we are in an escape sequence
	long correctionsMade;						// A counter for how many corrections were made to a packet
	uint8_t interleaveCounter;					// Keeps track of when we have received an entire interleaved block
	uint8_t interleaveOut[MP1_INTERLEAVE_SIZE]; // A buffer for interleaving bytes before they are sent
	uint8_t interleaveIn[MP1_INTERLEAVE_SIZE];	// A buffer for storing interleaved bytes before they are deinterleaved
} MP1;

// A struct encapsulating a network packet
typedef struct MP1Packet {
	const uint8_t *data;					// Pointer to the actual data in the packet
	size_t dataLength;						// The length of the received data
} MP1Packet;

// Declarations of functions
void mp1Init(MP1 *mp1, KFile *modem, mp1_callback_t callback);
void mp1Read(MP1 *mp1, int byte);
void mp1Poll(MP1 *mp1);
void mp1Send(MP1 *mp1, void *_buffer, size_t length);

int freeRam(void);
size_t compress(uint8_t *input, size_t length);
size_t decompress(uint8_t *input, size_t length);
void mp1Deinterleave(MP1 *mp1);
void mp1Interleave(MP1 *mp1, uint8_t byte);

#endif