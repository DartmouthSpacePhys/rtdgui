/*
 * radio.h
 *
 * header file for radio library
 * Original by Pete Cacciopi
 *
 * Modifications:
 * 951005 RB Started commenting revisions.  Fixed AUXILIARY_CHANNEL keyword
 * and added CHAN_KEY=CHANNEL keyword
 *
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

/* support the t_stamp data type */
#define STMPBYTES	4			/* the number of bytes used to represnt a timestamp */
#define MAX_STAMP	316224000L	/* the number of deciseconds in a leap year	*/
#define BAD_STAMP	ULONG_MAX-5	/* a bad timestamp is one larger then MAX_STAMP	*/

/* used for for converting a timestamp into an hour of the day */
#define	HOURS_DAY	24			/* the number of hours in a day	*/

/*	support for decisecond converson */
#define DEC_DAY		864000		/* the number of deciseconds in a day */
#define DEC_HOUR	36000		/* the number of deciseconds in an hour	*/
#define	DEC_MIN		600			/* the number of deciseconds in minute */
#define DEC_SEC		10			/* the number of deciseconds in a second */

/* used for converting a timestamp into a time of day hr:mn:sec	*/
#define SEC_DAY		86400		/* the number of seconds in a day */
#define SEC_HOUR	3600		/* the number of seconds in an hour	*/
#define SEC_MIN		60			/* the number of seconds in a minute */

/* used for converting a timestamp into a date */
#define FEB         1           /* Feburary is month 1 (Jan is month 0) */
#define NUM_MONTHS  12          /* there are 12 months in a year */

/* used for reading/writing multi-byte frequencies */
#define BYTES_LONG  4	/* need the machine to represent a long with this manyt bytes */

/* used for restricting the size of the arrays in radio utility programs */
#define MEG		1048576			/* the size of 1 megabyte */
#define MAX_MEG	2				/* max number of megabytes that can be malloced */
#define MAX_SZ	10				/* the maximum number of bytes in a sample */
#define STR 	140				/* a good number of characters to allow in a string	*/
#define MAX_AUX 300				/* the maximum number of auxillary channels per file */

/* the environment variable to find the name of the configuration file */
#define CONFIG_ENV	"RADIO_CONFIG"	

/* if CONFIG_ENV not set, the configuration file for providing default header info */
#define CONFIG			"/usr/local/radio/lib/radio_config"


/*	keys for READING from the header */
/*	NOTE interdepencies - header_write should know about all of the KEY/WRT	*/
#define NUMKEYS			19	/* number of such KEY/WRT keys	*/
/*	format - equivalent keys separated by at least one white space	*/
#define START_KEY		"START_HEADER"
#define FILE_T_KEY      "FILE_TYPE"
#define LOCAT_KEY		"LOCATION"
#define FILE_N_KEY		"FILENAME"
#define BYTES_PER_KEY	"BYTES_PER_SPECTRA BYTES_PER_SPECTRUM BYTES_PER_BLOCK"
#define SAMPLE_SZ_KEY	"BYTES_PER_SAMPLE"
#define SPECTRA_KEY     "SPECTRA_COUNT BLOCK_COUNT"
#define METHOD_KEY		"AVERAGE_METHOD"
#define RATIO_KEY		"RATIO"
#define FREQ_KEY        "FREQUENCY_COUNT SAMPLE_COUNT"
#define CHAN_KEY		"CHANNEL"
#define AUX_KEY			"AUXILIARY_CHANNEL"
#define DELTA_KEY       "DELAY_PER_FREQUENCY DELAY_PER_SAMPLE"
#define INT_KEY			"SPECTRUM_INTERVAL"
#define DATE_KEY		"DATE"
#define YEAR_KEY        "YEAR"
#define PAGE_KEY		"PAGE_START"
#define BEGIN_L_KEY     "BEGIN_FREQUENCY_LIST"
#define END_L_KEY       "END_FREQUENCY_LIST"
#define END_KEY        	"END_HEADER"

/*		keys for WRITING to header							*/
#define START_WRT		START_KEY
#define FILE_T_WRT      FILE_T_KEY
#define LOCAT_WRT		LOCAT_KEY	
#define FILE_N_WRT		FILE_N_KEY
#define BYTES_PER_WRT  	"BYTES_PER_SPECTRUM" 		
#define SAMPLE_SZ_WRT	SAMPLE_SZ_KEY		
#define SPECTRA_WRT     "SPECTRA_COUNT" 
#define METHOD_WRT		METHOD_KEY
#define FREQ_WRT        "FREQUENCY_COUNT"
#define CHAN_WRT		CHAN_KEY
#define AUX_WRT			AUX_KEY		
#define RATIO_WRT		RATIO_KEY
#define DELTA_WRT      	"DELAY_PER_FREQUENCY"
#define INT_WRT			INT_KEY
#define YEAR_WRT       	YEAR_KEY          
#define PAGE_WRT		PAGE_KEY
#define BEGIN_L_WRT   	BEGIN_L_KEY      
#define END_L_WRT		END_L_KEY      
#define END_WRT			END_KEY           

/*		special keys as flags in the configuration file 	*/	
#define ABORT_KEY		"!!!ABORT IF MISSING!!!"	/* indicates mandatory element		*/
#define EXPR_KEY		"***USE EXPR***"			/* pass string that follows to expr	*/ 
#define COMMENT_KEY		"#"		/* remainder of line following this string is comment	*/

typedef unsigned long t_stamp;	/* support the t_stamp data type	*/

typedef struct rl_hd_struct rl_hd;
struct rl_hd_struct {
	char *key;
	char *info;
	long begin;
	long end;
	rl_hd *next;
};


/*			prototypes							*/
int wrt_timestamp (FILE *outfile, unsigned long stamp);
t_stamp rd_timestamp (FILE *infile);
int rd_time (t_stamp stamp, char *s);
int rd_timeinfo (t_stamp stamp, int year, char *s);
int rd_date (t_stamp stamp, int year, char *s);
float rd_hours (FILE *infile);
long header_long (FILE *infile, char *key);
long header_long_p (rl_hd *r_header, char *key);
int header_string_p (rl_hd *r_header, char *key, char *s);
int header_string (FILE *infile, char *key, char *s);
long header_tell_p (rl_hd *r_header, char *key);
long header_tell (FILE *infile, char *key);
int check_size (FILE *infile, long topdata, int numspec, int bytespec);
void diag_printf (int verbose_level, int verbose_value, char *format, ...);
int header_pass (FILE *infile, rl_hd *r_header, int f_list);
rl_hd *make_r_hd (int num_elem);
void free_r_hd (rl_hd *header);
int strmultcmp (char *s1, char *s2);
int header_write (char *inkey, char *outkey);
int rd_freq (FILE *infile, unsigned long *freq, int bytes);
int wrt_freq (FILE *outfile, unsigned long freq, int bytes);
rl_hd *header_find_p (rl_hd *header, char *key);
