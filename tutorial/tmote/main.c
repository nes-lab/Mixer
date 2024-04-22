/***************************************************************************************************
 ***************************************************************************************************
 *
 *	Copyright (c) 2018, Networked Embedded Systems Lab, TU Dresden
 *	All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or without
 *	modification, are permitted provided that the following conditions are met:
 *		* Redistributions of source code must retain the above copyright
 *		  notice, this list of conditions and the following disclaimer.
 *		* Redistributions in binary form must reproduce the above copyright
 *		  notice, this list of conditions and the following disclaimer in the
 *		  documentation and/or other materials provided with the distribution.
 *		* Neither the name of the NES Lab or TU Dresden nor the
 *		  names of its contributors may be used to endorse or promote products
 *		  derived from this software without specific prior written permission.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***********************************************************************************************//**
 *
 *	@file					main.c
 *
 *	@brief					main entry point
 *
 *	@version				$Id$
 *	@date					TODO
 *
 *	@author					Carsten Herrmann
 *
 ***************************************************************************************************

 	@details

	TODO

 **************************************************************************************************/
//***** Trace Settings *****************************************************************************

#include "gpi/trace.h"

// message groups for TRACE messages (used in GPI_TRACE_MSG() calls)
// define groups appropriate for your needs, assign one bit per group
// values > GPI_TRACE_LOG_USER (i.e. upper 8 bits) are reserved
#define TRACE_INFO		0x00000001

// select active message groups, i.e., the messages to be printed (others will be dropped)
#ifndef GPI_TRACE_BASE_SELECTION
	#define GPI_TRACE_BASE_SELECTION	GPI_TRACE_LOG_STANDARD | GPI_TRACE_LOG_PROGRAM_FLOW
#endif
GPI_TRACE_CONFIG(main, GPI_TRACE_BASE_SELECTION | GPI_TRACE_LOG_USER);

//**************************************************************************************************
//**** Includes ************************************************************************************

#include "mixer/mixer.h"

#include "gpi/platform.h"
#include "gpi/interrupts.h"
#include "gpi/clocks.h"
#include "gpi/tools.h"
#include GPI_PLATFORM_PATH(radio.h)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//**************************************************************************************************
//***** Local Defines and Consts *******************************************************************

// important for the log parser
#if (GPI_ARCH_IS_BOARD(TMOTE_FLOCKLAB) || GPI_ARCH_IS_BOARD(TMOTE_INDRIYA))
	#define PRINT_HEADER()	while (0)
#else
	#define PRINT_HEADER()	printf("# ID:%u ", node_id + 1)
#endif

//**************************************************************************************************
//***** Local Typedefs and Class Declarations ******************************************************



//**************************************************************************************************
//***** Forward Declarations ***********************************************************************



//**************************************************************************************************
//***** Local (Static) Variables *******************************************************************

static uint32_t		msgs_decoded;
static uint32_t		msgs_not_decoded;
static uint32_t		msgs_wrong;

//**************************************************************************************************
//***** Global Variables ***************************************************************************

// TOS_NODE_ID is a variable with very special handling: on FLOCKLAB and INDRIYA, its init value
// gets overridden with the id of the node in the testbed during device programming (by calling
// tos-set-symbol (a script) on the elf file). Thus, it is well suited as a node id variable.
// ATTENTION: it is important to have TOS_NODE_ID in .data (not in .bss), otherwise tos-set-symbol
// will not work
// uint16_t __attribute__((section(".data")))	TOS_NODE_ID = 0;
uint16_t __attribute__((section(".data")))	FLOCKLAB_NODE_ID = 0;
#define TOS_NODE_ID FLOCKLAB_NODE_ID

//**************************************************************************************************
//***** Local Functions ****************************************************************************

// print results for the log parser
static void print_results(int node_id, uint32_t round)
{
	uint16_t	slot, slot_min, rank, i;

	mixer_print_statistics();

	rank = 0;
	for (i = 0; i < MX_GENERATION_SIZE; ++i)
	{
		if (mixer_stat_slot(i) >= 0)
			rank++;
	}

	PRINT_HEADER();
	printf("round=%" PRIu32 " rank=%" PRIu16 " dec=%" PRIu32 " notDec=%" PRIu32 " wrong=%" PRIu32 "\n", round, rank, msgs_decoded, msgs_not_decoded, msgs_wrong);
	// printf("INFOS (%lu): rank=%u\n", round, rank);

	msgs_decoded = 0;
	msgs_not_decoded = 0;
	msgs_wrong = 0;

	PRINT_HEADER();
	printf("rank_up_slot=[");
	// printf("DEBUG (%lu): rank up slot [", round);

	for (slot_min = 0; 1; )
	{
		slot = -1u;
		for (i = 0; i < MX_GENERATION_SIZE; ++i)
		{
			if (mixer_stat_slot(i) < slot_min)
				continue;

			if (slot > (uint16_t)mixer_stat_slot(i))
				slot = mixer_stat_slot(i);
		}

		if (-1u == slot)
			break;

		for (i = 0; i < MX_GENERATION_SIZE; ++i)
		{
			if (mixer_stat_slot(i) == slot)
				printf("%u;", slot);
		}

//		printf("%u, ", slot);
		slot_min = slot + 1;
	}

	printf("]\n");

	PRINT_HEADER();
	printf("rank_up_row=[");
	// printf("DEBUG (%lu): rank up row [", round);

	for (slot_min = 0; 1; )
	{
		slot = -1u;
		for (i = 0; i < MX_GENERATION_SIZE; ++i)
		{
			if (mixer_stat_slot(i) < slot_min)
				continue;

			if (slot > (uint16_t)mixer_stat_slot(i))
			{
				slot = mixer_stat_slot(i);
//				k = i;
			}
		}

		if (-1u == slot)
			break;

		for (i = 0; i < MX_GENERATION_SIZE; ++i)
		{
			if (mixer_stat_slot(i) == slot)
				printf("%u;", i);
		}

//		printf("%u, ", k);
		slot_min = slot + 1;
	}

	printf("]\n");
}

//**************************************************************************************************
//***** Global Functions ***************************************************************************

int main()
{
	// don't TRACE before gpi_platform_init()
	// GPI_TRACE_FUNCTION();

	uint8_t				node_id;
	uint32_t			round;
	Gpi_Hybrid_Tick		deadline;
	unsigned int		i;

	// init platform
	gpi_platform_init();
	gpi_int_enable();

	// init RF transceiver
	gpi_radio_init();
	gpi_radio_set_tx_power(gpi_radio_dbm_to_power_level(0));
	gpi_radio_set_channel(26);		// TODO: adapt if necessary

	printf("System started.\n");
	printf("DCO deviation: %+d\n", gpi_msp430_dco_resync(GPI_TICK_MS_TO_SLOW(10)));

	// get TOS_NODE_ID
	// if not set by programming toolchain on testbed
	if (0 == TOS_NODE_ID)
	{
		uint16_t	flash[2];

		// read from info flash
		gpi_msp430_flash_read(INFO_A, &flash, 0, sizeof(flash));

		// check signature
		if (0x55AA == flash[0])
		{
			GPI_TRACE_MSG(TRACE_INFO, "info flash is valid");
			TOS_NODE_ID = flash[1];
		}

		// if signature is invalid
		while (0 == TOS_NODE_ID)
		{
			printf("TOS_NODE_ID not set. enter value: ");

			// read from console
			// scanf("%u", &TOS_NODE_ID);
			char s[8];
			TOS_NODE_ID = atoi(getsn(s, sizeof(s)));

			printf("\nTOS_NODE_ID set to %u\n", TOS_NODE_ID);

			// until input value is valid
			if (0 == TOS_NODE_ID)
				continue;

			// store new value in info flash

			flash[0] = 0x55AA;
			flash[1] = TOS_NODE_ID;

			gpi_msp430_flash_erase(INFO_A);
			gpi_msp430_flash_write(INFO_A, 0, &flash, sizeof(flash));

			break;
		}

		// realign DCO because console input can take long time
		gpi_msp430_dco_realign();
	}

	printf("starting node %u ...\n", TOS_NODE_ID);

	// init RNG with randomized seed
	mixer_rand_seed(gpi_mulu_16x16(TOS_NODE_ID, gpi_tick_fast_native()));

	// translate TOS_NODE_ID to logical node id used with mixer
	for (node_id = 0; node_id < NUM_ELEMENTS(nodes); ++node_id)
	{
		if (nodes[node_id] == TOS_NODE_ID)
			break;
	}
	if (node_id >= NUM_ELEMENTS(nodes))
	{
		printf("!!! PANIC: node mapping not found for node %u !!!\n", TOS_NODE_ID);
		while (1);
	}
	printf("mapped physical node %u to logical id %u\n", TOS_NODE_ID, node_id);

	// print setup (for log parser)
	PRINT_HEADER();
	// printf("SETUP: nodes=%u, payload=%u, slots=%u\n", MX_NUM_NODES, MX_PAYLOAD_SIZE, MX_ROUND_LENGTH);
	mixer_print_config();

	// deadline for first round is now (-> start as soon as possible)
	deadline = gpi_tick_hybrid();

	// run
	for (round = 1; 1; round++)
	{
		uint8_t	data[7];

		printf("preparing round %lu ...\n", round);

		// init mixer
		mixer_init(node_id);

		// provide some test data messages
		{
			data[1] = node_id;
			data[2] = TOS_NODE_ID;
			data[3] = round;
			data[4] = round >> 8;
			data[5] = round >> 16;
			data[6] = round >> 24;

			for (i = 0; i < MX_GENERATION_SIZE; i++)
			{
				data[0] = i;

				if (payload_distribution[i] == TOS_NODE_ID)
					mixer_write(i, data, MIN(sizeof(data), MX_PAYLOAD_SIZE));
			}
		}

		// arm mixer, node 0 = initiator
		#if START_ROUND_VIA_GPIO
			unsigned int is_initiator = 0;
			// if (1 == TOS_NODE_ID | 31 == TOS_NODE_ID | 10 == TOS_NODE_ID | 29 == TOS_NODE_ID | 9 == TOS_NODE_ID)
			if (1 == TOS_NODE_ID | 31 == TOS_NODE_ID)
			{
				is_initiator = 1;
			}

			// arm mixer
			if (is_initiator)
			{
				mixer_arm(MX_ARM_INITIATOR);
			}
			else
			{
				mixer_arm(0);
			}

			printf("DCO deviation: %+d\n", gpi_msp430_dco_resync(GPI_TICK_MS_TO_SLOW(10)));

			// Wait for GPIO pin to start the round.
			while ((P2IN & BV(7)) == 0) ;

			// Delay initiator(s) a little bit.
			// if (is_initiator)
			// {
			// 	gpi_micro_sleep(20);
			// }

			// Initiators with different delays.
			if (1 == TOS_NODE_ID)
			{
				gpi_micro_sleep(20);
			} else if (31 == TOS_NODE_ID)
			{
				gpi_micro_sleep(180);
			}
		#else
			#if GPI_ARCH_IS_BOARD(TMOTE_INDRIYA)
				mixer_arm(((!node_id) ? MX_ARM_INITIATOR : 0) | MX_ARM_INFINITE_SCAN);

				if (!node_id)
					deadline += MAX(3 * MX_SLOT_LENGTH, GPI_TICK_MS_TO_HYBRID(200));
			#else
				// start first round with infinite scan
				// -> nodes join next available round, does not require simultaneous boot-up
				mixer_arm(((!node_id) ? MX_ARM_INITIATOR : 0) | ((1 == round) ? MX_ARM_INFINITE_SCAN : 0));

				// delay initiator a bit
				// -> increase probability that all nodes are ready when initiator starts the round
				// -> avoid problems in view of limited deadline accuracy
				if (!node_id)
					deadline += 3 * MX_SLOT_LENGTH;
			#endif

			// resync DCO (short) before every round
			// -> among other things, make sure that the UART works right
			while (gpi_tick_compare_hybrid(gpi_tick_hybrid(), deadline - GPI_TICK_MS_TO_HYBRID(100)) < 0);
			printf("DCO deviation: %+d\n", gpi_msp430_dco_resync(GPI_TICK_MS_TO_SLOW(10)));

			// start when deadline reached
			// attention: don't delay after the polling loop (-> print before)
			// printf("starting round %lu ...\n", round);
			while (gpi_tick_compare_hybrid(gpi_tick_hybrid(), deadline) < 0);
		#endif

		gpi_led_on(GPI_LED_3);

		deadline = mixer_start();

		// wait until deadline
		while (gpi_tick_compare_hybrid(gpi_tick_hybrid(), deadline) < 0);
		gpi_led_off(GPI_LED_3);

		// evaluate received data
		for (i = 0; i < MX_GENERATION_SIZE; i++)
		{
			void *p = mixer_read(i);
			if (NULL == p)
			{
				// printf("message %u not decoded\n", i);
				msgs_not_decoded++;
			}
			else
			{
				memcpy(data, p, sizeof(data));
				if ((data[0] == i) && (data[2] == payload_distribution[i]))
				{
					// don't print too much on Indriya
					// (it is very easy to overload the observer and lose a lot of messages)
					// #if !GPI_ARCH_IS_BOARD(TMOTE_INDRIYA)
					// 	printf("message %u ok\n", i);
					// #endif
					msgs_decoded++;
				}
				else
				{
					// printf("message %u wrong\n", i);
					msgs_wrong++;
				}

				// use message 0 to check/adapt round number
				if ((0 == i) && (MX_PAYLOAD_SIZE >= 7))
				{
					Generic32	r;

					r.u8_ll = data[3];
					r.u8_lh = data[4];
					r.u8_hl = data[5];
					r.u8_hh = data[6];

					if (1 == round)
					{
						round = r.u32;
						printf("synchronized to round %lu\n", r.u32);
					}
					else if (r.u32 != round)
					{
						printf("round mismatch: received %lu <> local %lu! trying resync ...\n", r.u32, round);
						round = 0;	// increments to 1 with next round loop iteration
					}
				}
			}
		}

		// print some log data
		print_results(node_id, round);

		#if !START_ROUND_VIA_GPIO
			// set start time for next round
			// note: we choose a high enough time to be able to setup the next round and to print
			// out all debug messages. Besides that, consider the documentation on the deadline
			// accuracy returned by mixer_start().
			deadline += MAX(10 * MX_SLOT_LENGTH, GPI_TICK_MS_TO_HYBRID2(2000));
		#endif
/*
		#if GPI_ARCH_IS_BOARD(TMOTE_INDRIYA)
			deadline += S_TO_HYBRID(100);
		#endif
*/		
		// printf("next round starts in %luus\n", 
		// 	(uint32_t)gpi_tick_hybrid_to_us(deadline - gpi_tick_hybrid()));
	}

	GPI_TRACE_RETURN(0);
}

//**************************************************************************************************
//**************************************************************************************************
