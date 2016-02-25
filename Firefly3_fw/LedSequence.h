/*
 * LedSequence.h
 *
 *  Created on: 21 окт. 2014 г.
 *      Author: g.kruglov
 */

#ifndef LEDSEQUENCE_H_
#define LEDSEQUENCE_H_

#include "color.h"
#include "uart.h"

enum LedChunkSort_t {csSetColor, csWait, csGoto, csEnd};

struct LedChunk_t {
    LedChunkSort_t ChunkSort;
    union {
        int32_t MorphMS;       // 0 means immediately. Up to several thounds
        int32_t Time_ms;
        int32_t ChunkToJumpTo;
    };
    Color_t Color;
} __attribute__((packed));
#define LED_CHUNK_SZ    sizeof(LedChunk_t)  // 8 bytes

#define LED_CHUNK_CNT   54
struct LedSequence_t {
    uint32_t Cnt;
    LedChunk_t Chunk[LED_CHUNK_CNT];
    void Reset() { Cnt = 0; }
    void Print() const {
        for(uint32_t i=0; i < Cnt; i++) {
            switch(Chunk[i].ChunkSort) {
                case csSetColor:
                    Uart.Printf("RGB,%u,%u,%u,%u", Chunk[i].Color.R, Chunk[i].Color.G, Chunk[i].Color.B, Chunk[i].MorphMS);
                    break;
                case csWait:
                    Uart.Printf("Wait,%u",Chunk[i].Time_ms);
                    break;
                case csGoto:
                    Uart.Printf("Goto,%u",Chunk[i].ChunkToJumpTo);
                    break;
                case csEnd:
//                    Uart.Printf(",End");
                    break;
            }
            if(i != (Cnt-1)) Uart.Printf(",");
        }
        Uart.Printf("\r\n");
    }
};
#define LED_SEQ_SZ      sizeof(LedSequence_t)


#endif /* LEDSEQUENCE_H_ */
