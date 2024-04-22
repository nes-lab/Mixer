
// This file contains the basic FlockLab setup. It is used in config.h (i.e., C code) as well as
// config_flocklab.xml (i.e., XML config file).

#ifdef _XML_
	#define comma
#else
	#define comma	,
#endif

#define FLOCKLAB_OBSERVERS	\
	1		comma	\
	2		comma	\
	3		comma	\
	4		comma	\
	5		comma	\
	6		comma	\
	7		comma	\
	8		comma	\
	9		comma	\
	10		comma	\
	11		comma	\
	12		comma	\
	13		comma	\
	16		comma	\
	19		comma	\
	20		comma	\
	21		comma	\
	22		comma	\
	24		comma	\
	26		comma	\
	27		comma	\
	29		comma	\
	31		comma	\
/*
	23		comma	\
	28		comma	\
	32		comma	\
*/

#define FLOCKLAB_NODES		\
	1		comma	\
	2		comma	\
	3		comma	\
	4		comma	\
	5		comma	\
	6		comma	\
	7		comma	\
	8		comma	\
	9		comma	\
	10		comma	\
	11		comma	\
	12		comma	\
	13		comma	\
	16		comma	\
	19		comma	\
	20		comma	\
	21		comma	\
	22		comma	\
	24		comma	\
	26		comma	\
	27		comma	\
	29		comma	\
	31		comma	\
/*
	23		comma	\
	28		comma	\
	32		comma	\
*/

#define PAYLOAD_DISTRIBUTION_all_to_all			FLOCKLAB_NODES

#define PAYLOAD_DISTRIBUTION_1_to_all			\
	1,  1,  1,  1,  1,  1,  1,  1,  1,  1,		\
	1,  1,  1,  1,  1,  1,  1,  1,  1,  1,		\
	1,  1,  1,  1,  1,  1,  1
