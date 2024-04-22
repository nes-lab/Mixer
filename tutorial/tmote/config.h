
// mixer configuration file
// Adapt the settings to the needs of your application.

#include "gpi/platform_spec.h"		// GPI_ARCH_IS_...
#include "gpi/tools.h"				// NUM_ELEMENTS()

// testbed-dependent settings
#if GPI_ARCH_IS_BOARD(TMOTE_FLOCKLAB)

	#include "config_flocklab.h"

	static const uint8_t nodes[] = { FLOCKLAB_NODES };
	// static const uint8_t payload_distribution[] = { PAYLOAD_DISTRIBUTION_1_to_all };
	static const uint8_t payload_distribution[] = { PAYLOAD_DISTRIBUTION_all_to_all };

	#define MX_ROUND_LENGTH		300		// in #slots

/*
#elif GPI_ARCH_IS_BOARD(TMOTE_INDRIYA)

	#include "config_indriya.h"

	static const uint8_t nodes[] = { INDRIYA_NODES };

	#define MX_ROUND_LENGTH		1000	// in #slots
*/

// tiny test on developer's desk
#else

	static const uint8_t nodes[] = { 1, 2, 3 };
	static const uint8_t payload_distribution[] = { 1, 2, 3 };

	#define MX_ROUND_LENGTH		10		// in #slots

#endif

// basic settings
#define MX_NUM_NODES			NUM_ELEMENTS(nodes)
#define MX_GENERATION_SIZE		NUM_ELEMENTS(payload_distribution)
#define MX_PAYLOAD_SIZE			16
#define MX_INITIATOR_ID			1

// choose a slot length according to your settings
// NOTE: Measurement unit is hybrid ticks.
// NOTE: The Mixer paper section 4 gives some orientation on how to choose this value.
// For the tutorial project we use quiet long slots to be able to trace all messages
// on the UART-based console.
// #define MX_SLOT_LENGTH			GPI_TICK_US_TO_HYBRID2(1000000)
#define MX_SLOT_LENGTH			GPI_TICK_US_TO_HYBRID2(2000)

// turn verbose log messages on or off
#define MX_VERBOSE_CONFIG		1
#define MX_VERBOSE_STATISTICS	1
#define MX_VERBOSE_PACKETS		0

#define START_ROUND_VIA_GPIO	1