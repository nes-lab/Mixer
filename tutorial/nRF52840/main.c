/***************************************************************************************************
 ***************************************************************************************************
 *
 *	Copyright (c) 2019, Networked Embedded Systems Lab, TU Dresden
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
 *	@author					Fabian Mager
 *
 ***************************************************************************************************

 	@details

	TODO

 **************************************************************************************************/
//***** Trace Settings *****************************************************************************

#include "gpi/trace.h"

// message groups for TRACE messages (used in GPI_TRACE_MSG() calls)
// define groups appropriate for your needs, assign one bit per group
// values > GPI_TRACE_LOG_USER (i.e. upper bits) are reserved
#define TRACE_INFO		GPI_TRACE_MSG_TYPE_INFO

// select active message groups, i.e., the messages to be printed (others will be dropped)
#ifndef GPI_TRACE_BASE_SELECTION
	#define GPI_TRACE_BASE_SELECTION	GPI_TRACE_LOG_STANDARD | GPI_TRACE_LOG_PROGRAM_FLOW
#endif
GPI_TRACE_CONFIG(main, GPI_TRACE_BASE_SELECTION);

//**************************************************************************************************
//***** Includes ***********************************************************************************

#include "mixer/mixer.h"

#include "gpi/tools.h"
#include "gpi/platform.h"
#include "gpi/interrupts.h"
#include "gpi/clocks.h"
#include "gpi/olf.h"
#include GPI_PLATFORM_PATH(radio.h)

#include <nrf.h>

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>


//**************************************************************************************************
//***** Local Defines and Consts *******************************************************************

#define PRINT_HEADER()		printf("# ID:%u ", TOS_NODE_ID)

//**************************************************************************************************
//***** Local Typedefs and Class Declarations ******************************************************



//**************************************************************************************************
//***** Forward Declarations ***********************************************************************



//**************************************************************************************************
//***** Local (Static) Variables *******************************************************************

static uint8_t		node_id;
static uint32_t		round;
static uint32_t		msgs_decoded;
static uint32_t		msgs_not_decoded;
static uint32_t		msgs_weak;
static uint32_t		msgs_wrong;

//**************************************************************************************************
//***** Global Variables ***************************************************************************

// TOS_NODE_ID is a variable with very special handling: on FLOCKLAB and INDRIYA, its init value
// gets overridden with the id of the node in the testbed during device programming (by calling
// tos-set-symbol (a script) on the elf file). Thus, it is well suited as a node id variable.
// ATTENTION: it is important to have TOS_NODE_ID in .data (not in .bss), otherwise tos-set-symbol
// will not work
uint16_t __attribute__((section(".data")))	TOS_NODE_ID = 0;

//**************************************************************************************************
//***** Local Functions ****************************************************************************

// Print results of a Mixer round.
static void print_results(uint8_t log_id)
{
	unsigned int	slot, slot_min, i;
	uint32_t		rank = 0;

	// Mixer internal stats (enabled with MX_VERBOSE_STATISTICS)
	mixer_print_statistics();

	for (i = 0; i < MX_GENERATION_SIZE; i++)
	{
		if (mixer_stat_slot(i) >= 0) ++rank;
	}

	PRINT_HEADER();
	printf("round=%" PRIu32 " rank=%" PRIu32 " dec=%" PRIu32 " !dec=%" PRIu32 " weak=%" PRIu32
	       " wrong=%" PRIu32 "\n",
	       round, rank, msgs_decoded, msgs_not_decoded, msgs_weak, msgs_wrong);

	msgs_decoded = 0;
	msgs_not_decoded = 0;
	msgs_weak = 0;
	msgs_wrong = 0;

	PRINT_HEADER();
	printf("rank_up_slot=[");
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

		slot_min = slot + 1;
	}
	printf("]\n");

	PRINT_HEADER();
	printf("rank_up_row=[");
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
				printf("%u;", i);
		}

		slot_min = slot + 1;
	}
	printf("]\n");
}

//**************************************************************************************************

static void initialization(void)
{
	// init platform
	gpi_platform_init();
	gpi_int_enable();

	// Start random number generator (RNG) now so that we definitely have some random value as a seed later in the initialization.
	NRF_RNG->INTENCLR = BV_BY_NAME(RNG_INTENCLR_VALRDY, Clear);
	NRF_RNG->CONFIG = BV_BY_NAME(RNG_CONFIG_DERCEN, Enabled);
	NRF_RNG->TASKS_START = 1;

	// enable SysTick timer if needed
	//#if MX_VERBOSE_PROFILE
		SysTick->LOAD  = -1u;
		SysTick->VAL   = 0;
		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
	//#endif

	// init RF transceiver
	gpi_radio_init(MX_PHY_MODE);
	gpi_radio_set_tx_power(gpi_radio_dbm_to_power_level(MX_TX_PWR_DBM));
	switch (MX_PHY_MODE)
	{
		case BLE_1M:
		case BLE_2M:
		case BLE_125k:
		case BLE_500k:
			gpi_radio_set_channel(39);
			gpi_radio_ble_set_access_address(~0x8E89BED6);
			break;

		case IEEE_802_15_4:
			gpi_radio_set_channel(26);
			break;

		default:
			printf("ERROR: MX_PHY_MODE is invalid!\n");
			assert(0);
	}

	printf("Hardware initialized. Compiled at " __DATE__ " " __TIME__ "\n");

	// Check if TOS_NODE_ID is set. If not, request from stdin.
	#if GPI_ARCH_IS_BOARD(nRF_PCA10056)
		if (0 == TOS_NODE_ID)
		{
			uint16_t	data[2];

			// read from nRF UICR area
			gpi_nrf_uicr_read(&data, 0, sizeof(data));

			// check signature
			if (0x55AA == data[0])
			{
				GPI_TRACE_MSG(TRACE_INFO, "non-volatile config is valid");
				TOS_NODE_ID = data[1];
			}
			else GPI_TRACE_MSG(TRACE_INFO, "non-volatile config is invalid");

			// if signature is invalid
			while (0 == TOS_NODE_ID)
			{
				printf("Node ID not set. enter value: ");

				// read from console
				// scanf("%u", &TOS_NODE_ID);
				char s[8];
				TOS_NODE_ID = atoi(getsn(s, sizeof(s)));

				printf("\nNode ID set to %u\n", TOS_NODE_ID);

				// until input value is valid
				if (0 == TOS_NODE_ID)
					continue;

				// store new value in UICR area
				data[0] = 0x55AA;
				data[1] = TOS_NODE_ID;

				gpi_nrf_uicr_erase();
				gpi_nrf_uicr_write(0, &data, sizeof(data));

				// ATTENTION: Writing to UICR requires NVMC->CONFIG.WEN to be set which in turn
				// invalidates the instruction cache (permanently). Besides that, UICR updates take
				// effect only after reset (spec. 4413_417 v1.0 4.3.3 page 24). Therefore we do a soft
				// reset after the write procedure.
				printf("Restarting system...\n");
				gpi_milli_sleep(100);		// safety margin (e.g. to empty UART Tx FIFO)
				NVIC_SystemReset();

				break;
			}
		}
	#endif

	printf("starting node %u ...\n", TOS_NODE_ID);

	// Stop RNG because we only need one random number as seed.
	NRF_RNG->TASKS_STOP = 1;
	uint8_t rng_value = BV_BY_VALUE(RNG_VALUE_VALUE, NRF_RNG->VALUE);
	uint32_t rng_seed = rng_value * gpi_mulu_16x16(TOS_NODE_ID, gpi_tick_fast_native());
	printf("random seed for Mixer is %" PRIu32"\n", rng_seed);
	// init RNG with randomized seed
	mixer_rand_seed(rng_seed);

	// translate physical node ID to logical node ID (zero-based) used inside mixer
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

	// Print Mixer configuration.
	mixer_print_config();
}

//**************************************************************************************************
//***** Global Functions ***************************************************************************

int main()
{
	// don't TRACE before gpi_platform_init()
	// GPI_TRACE_FUNCTION();

	Gpi_Hybrid_Tick	t_ref; // time reference point at the end of a Mixer round
	unsigned int i;

	initialization();

	// t_ref for first round is now (-> start as soon as possible)
	t_ref = gpi_tick_hybrid();

	// run
	for (round = 1; 1; round++)
	{
		uint8_t	data[7];

		printf("preparing round %" PRIu32 " ...\n", round);

		// init mixer
		mixer_init(node_id);

		#if MX_WEAK_ZEROS
			mixer_set_weak_release_slot(WEAK_RELEASE_SLOT);
			mixer_set_weak_return_msg((void*)-1);
		#endif

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
				{
					// If data is set to NULL, the message is treated as weak message.
					mixer_write(i, data, MIN(sizeof(data), MX_PAYLOAD_SIZE));
				}
			}
		}

		// arm mixer

		// start first round with infinite scan
		// -> nodes join next available round, does not require simultaneous boot-up
		mixer_arm(((MX_INITIATOR_ID == TOS_NODE_ID) ? MX_ARM_INITIATOR : 0) | ((1 == round) ? MX_ARM_INFINITE_SCAN : 0));

		// delay initiator a bit
		// -> increase probability that all nodes are ready when initiator starts the round
		// -> avoid problems in view of limited deadline accuracy
		if (MX_INITIATOR_ID == TOS_NODE_ID)
		{
			t_ref += 3 * MX_SLOT_LENGTH;
		}

		// start when deadline reached
		// ATTENTION: don't delay after the polling loop (-> print before)
		printf("starting round %" PRIu32 " ...\n", round);
		while (gpi_tick_compare_hybrid(gpi_tick_hybrid(), t_ref) < 0);

		// Mixer round
		t_ref = mixer_start();

		// Wait until t_ref (nominal end of Mixer round). 
		while (gpi_tick_compare_hybrid(gpi_tick_hybrid(), t_ref) < 0);

		// evaluate received data
		for (i = 0; i < MX_GENERATION_SIZE; i++)
		{
			void *p = mixer_read(i);
			if (NULL == p)
			{
				msgs_not_decoded++;
			}
			else if ((void*)-1 == p)
			{
				msgs_weak++;
			}
			else
			{
				memcpy(data, p, sizeof(data));
				if ((data[0] == i) && (data[2] == payload_distribution[i]))
				{
					msgs_decoded++;
				}
				else
				{
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
						printf("synchronized to round %" PRIu32 "\n", r.u32);
					}
					else if (r.u32 != round)
					{
						printf("round mismatch: received %" PRIu32 " <> local %" PRIu32 "! trying resync ...\n", r.u32, round);
						round = 0;	// increments to 1 with next round loop iteration
					}
				}
			}
		}

		print_results(node_id);

		// Set start time for next round. Check that there is enough time to print all results!
		t_ref += MAX(10 * MX_SLOT_LENGTH, GPI_TICK_MS_TO_HYBRID2(1000));
	}

	GPI_TRACE_RETURN(0);
}

//**************************************************************************************************
//**************************************************************************************************
