/*----------------------------------------------------------------------------------------------*/
/*! \file messages.h
//
// FILENAME: messages.h
//
// DESCRIPTION: Define messages used for RS422 interface
//
// DEVELOPERS: Gregory W. Heckler (2003-2009)
//
// LICENSE TERMS: Copyright (c) Gregory W. Heckler 2009
//
// This file is part of the GPS Software Defined Radio (GPS-SDR)
//
// The GPS-SDR is free software; you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version. The GPS-SDR is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// Note:  Comments within this file follow a syntax that is compatible with
//        DOXYGEN and are utilized for automated document extraction
//
// Reference:
*/
/*----------------------------------------------------------------------------------------------*/

#ifndef MESSAGES_H
#define MESSAGES_H

#define CCSDS_APID_BASE	(0x0)	//!< The CCSDS APID Base number for our receiver

/*! @ingroup MESSAGES
 *  @brief Enum the packet ID #s, DO NOT REORDER */
enum CCSDS_MESSAGES_IDS
{
	FIRST_M_ID,
	FIRST_PERIODIC_M_ID,
	BOARD_HEALTH_M_ID,
	TASK_HEALTH_M_ID,
	SPS_M_ID,
	TOT_M_ID,
	PPS_M_ID,
	CLOCK_M_ID,
	CHANNEL_M_ID,
	SV_POSITION_M_ID,
	MEASUREMENT_M_ID,
	PSEUDORANGE_M_ID,
	SV_PREDICTION_M_ID,
	LAST_PERIODIC_M_ID,
	EKF_STATE_M_ID,
	EKF_COVARIANCE_M_ID,
	EKF_RESIDUAL_M_ID,
	COMMAND_M_ID,
	COMMAND_ACK_M_ID,
	EPHEMERIS_M_ID,
	ALMANAC_M_ID,
	EPHEMERIS_STATUS_M_ID,
	SV_SELECT_STATUS_M_ID,
	EEPROM_M_ID,
	EEPROM_CHKSUM_M_ID,
	MEMORY_M_ID,
	MEMORY_CHKSUM_M_ID,
	LAST_M_ID
};


/*! @ingroup MESSAGES
 *  @brief Packet dumped to telemetry and to disk to keep track of each channel */
typedef struct CCSDS_Packet_Header
{

  uint16_t pid; //!< Packet ID
  uint16_t psc; //!< Packet sequence control
  uint16_t pdl; //!< Packet data length

} CCSDS_Packet_Header;


/*! @ingroup MESSAGES
 *  @brief Decoded header
 */
typedef struct CCSDS_Decoded_Header
{
  uint32_t id;
  uint32_t type;
  uint32_t tic;
  uint32_t length;
} CCSDS_Decoded_Header;

/*! @ingroup MESSAGES
 *  @brief Packet format
 */
typedef struct CCSDS_Packet
{
  uint32_t preamble;      //!< Preamble, should ALWAYS be 0xAAAAAAAA
	CCSDS_Packet_Header header; //!< CCSDS header
  uint8_t payload[502];     //!< Payload, up to 502 bytes
} CCSDS_Packet;

/*! @ingroup MESSAGES
 *  @brief Packet that contains board hardware health parameters
 */
typedef struct Board_Health_M
{

	/* Grab FPGA versions */
  uint32_t trk_version;   //!< Tracking FPGA version
  uint32_t acq_version;   //!< Acquisition FPGA version
  uint32_t fft_version;   //!< FFT FPGA version
  uint32_t sft_version;   //!< Software version;
  uint32_t dsa0;      //!< Current state of DSA 0
  uint32_t dsa1;      //!< Current state of DSA 1
  uint32_t dsa2;      //!< Current state of DSA 2
  uint32_t dsa3;      //!< Current state of DSA 3
  uint32_t ovrflw0;     //!< Overflow counter on A/D 0
  uint32_t ovrflw1;     //!< Overflow counter on A/D 1
  uint32_t ovrflw2;     //!< Overflow counter on A/D 2
  uint32_t ovrflw3;     //!< Overflow counter on A/D 3
  uint32_t lo_locked;   //!< Is the LO locked to the synthesizer
  uint32_t sram_bad_mem;  //!< Debug info from Steve's POST
  uint32_t sram_bad_hi;   //!< Debug info from Steve's POST
  uint32_t sram_bad_lo;   //!< Debug info from Steve's POST
  uint32_t adc_values[SHU_SIGNALS]; //!< A/D values from SHU
  uint32_t tic;       //!< Global_tic associated with this data

} Board_Health_M;


/*! @ingroup MESSAGES
 *  @brief Message that contains status and health of the execution tasks
 */
typedef struct Task_Health_M
{
  uint32_t execution_tic[MAX_TASKS];  //!< Execution counters
  uint32_t start_tic[MAX_TASKS];    //!< Nucleus tic at function entry
  uint32_t stop_tic[MAX_TASKS];     //!< Nucleus tic at function exit
  uint32_t missed_interrupts;     //!< Missed interrupts
  uint32_t tic_fpu_mul;         //!< FPU multiplies
  uint32_t tic_fpu_div;         //!< FPU divides
  uint32_t tic;             //!< Corresponds to this receiver tic
} Task_Health_M;


/*! @ingroup MESSAGES
 *  @brief Packet that contains the tracking status and health of each channel
 */
typedef struct Channel_M
{

  uint32_t tic;     //!< Corresponds to this receiver tic
  int32_t chan;     //!< The channel number
  int32_t sv;     //!< SV/PRN number the channel is tracking
  int32_t state;    //!< channel's state
  int32_t antenna;    //!< Antenna channel is tracking off of
  int32_t len;      //!< acummulation length (1 or 20 ms)
  int32_t w;      //!< 3rd order PLL state
  int32_t x;      //!< 3rd order PLL state
  int32_t z;      //!< 3rd order PLL state
  int32_t code_nco;   //!< State of code_nco
  int32_t carrier_nco;  //!< State of carrier_nco
  int32_t cn0;      //!< CN0 estimate
  int32_t p_avg;    //!< Filtered version of I^2+Q^2
  int32_t bit_lock;   //!< Bit lock?
  int32_t frame_lock; //!< Frame lock?
  int32_t navigate;   //!< Navigate on this channel flag
  int32_t count;    //!< Number of accumulations that have been processed
  int32_t subframe;   //!< Current subframe number
  int32_t best_epoch; //!< Best estimate of bit edge position

} Channel_M;


/*! @ingroup MESSAGES
 *  @brief Raw PVT navigation solution
*/
typedef struct SPS_M
 {

	double x;			//!< ECEF x coordinate (meters)
	double y;			//!< ECEF y coordinate (meters)
	double z;			//!< ECEF z coordinate (meters)
	double vx;			//!< ECEF x velocity (meters/sec)
	double vy;			//!< ECEF x velocity (meters/sec)
	double vz;			//!< ECEF x velocity (meters/sec)
	double time;		//!< time in seconds
	double clock_bias;	//!< clock bias in seconds
	double clock_rate;  //!< clock rate in meters/second
	double latitude;	//!< latitude in decimal radians
	double longitude;	//!< longitude in decimal radians
	double altitude;	//!< height in meters
	double gdop;		//!< geometric dilution of precision
  uint32_t nsvs;    //!< This is a mask, not a number!
  uint32_t converged; //!< declare convergence
  uint32_t iterations;      //!< Iterations
  uint32_t stale_ticks;     //!< count the number of ticks since the last good sltn
  uint32_t converged_ticks;   //!< count number of converged ticks
  uint32_t chanmap[MAX_CHANNELS]; //!< Map of channels->sv
  uint32_t tic;     //!< Corresponds to this receiver tic

} SPS_M;

/*! @ingroup MESSAGES
 *  @brief Time of tone message (UTC & GPS Time of PPS)
*/
typedef struct TOT_M
{
	double second;
  uint32_t day;
  uint32_t week;
  uint32_t valid;
  uint32_t tic;
} TOT_M;

/*! @ingroup MESSAGES
 *  @brief Contains the clock state
*/
typedef struct Clock_M
{

	double receiver_time;	//!< Elapsed receiver time
	double rate;	 	//!< Clock rate
	double bias; 		//!< Clock bias
	double time0;		//!< Guess of gps second at initialization
	double time;		//!< Best estimate of GPS time
	double time_raw;	//!< Uncorrected time
  uint32_t week;    //!< GPS week
  uint32_t state;   //!< Clock state
  uint32_t pps_accum; //!< PPS accumulation value
  uint32_t tic;     //!< Corresponds to this receiver tic

} Clock_M;


/*! @ingroup MESSAGES
 *  @brief PPS State
*/
typedef struct PPS_M
{

	double err;			//!< Instantaneous error
	double err_lp;		//!< Instantaneous error, lowpassed
	double clock_rate;	//!< Clock rate estimate
	double feedback;	//!< NCO feedback command (seconds)
  uint32_t state;   //!< PPS state
  uint32_t tic;     //!< Corresponds to this receiver tic

} PPS_M;


/*! @ingroup MESSAGES
 *  @brief Contains the SV position used for the PVT solution
*/
typedef struct SV_Position_M
{

	double x;				//!< ECEF x coordinate (meters)
	double y;				//!< ECEF y coordinate (meters)
	double z;				//!< ECEF z coordinate (meters)
	double vx;				//!< ECEF x velocity (meters/sec)
	double vy;				//!< ECEF x velocity (meters/sec)
	double vz;				//!< ECEF x velocity (meters/sec)
	double elev;			//!< Satellite elevation (radians)
	double azim;			//!< Satellite azimuth (radians)
	double clock_bias;		//!< SV clock bias (seconds)
	double frequency_bias;	//!< SV clock rate bias (seconds/second)
	double transit_time;	//!< Time of flight from SV to receiver (seconds)
	double time;			//!< Time used in SV position calculation (seconds)
  uint32_t tic;       //!< Corresponds to this receiver tic
  uint32_t chan;      //!< Corresponding channel
  uint32_t sv;        //!< Corresponding sv

} SV_Position_M;


/*! @ingroup MESSAGES
 *  @brief EKF state solution
*/
typedef struct EKF_State_M
{

	/* The main filter states */
	double x;			//!< ECEF x coordinate (meters)
	double y;			//!< ECEF y coordinate (meters)
	double z;			//!< ECEF z coordinate (meters)
	double vx;			//!< ECEF x velocity (meters/sec)
	double vy;			//!< ECEF x velocity (meters/sec)
	double vz;			//!< ECEF x velocity (meters/sec)
	double solar;		//!< Solar radiation pressure
	double drag;		//!< Atmospheric drag
	double clock_bias;	//!< clock bias in seconds
	double clock_rate;  //!< clock rate in meters/second

	/* These are tags, not covariances */
	double time;		//!< Time in seconds (GPS)
  uint32_t nsvs;    //!< This is a mask, not a number!
  uint32_t week;    //!< Week (GPS)
  uint32_t status;    //!< Has this state failed any of the EKF error checking, convergence flag, etc
  uint32_t ekf_ticks; //!< Count the number of state update calls
  uint32_t period;    //!< Number of 500 us interrupts it took to do the state update
  uint32_t tic;     //!< This information is associated with the given receiver tic

} EKF_State_M;


/*! @ingroup MESSAGES
 *  @brief EKF covariance message
*/
typedef struct EKF_Covariance_M
{

	/* The main filter states */
	double x;			//!< ECEF x covariance (meters)
	double y;			//!< ECEF y covariance (meters)
	double z;			//!< ECEF z covariance (meters)
	double vx;			//!< ECEF x velocity covariance (meters/sec)
	double vy;			//!< ECEF x velocity covariance (meters/sec)
	double vz;			//!< ECEF x velocity covariance (meters/sec)
	double clock_bias;	//!< Clock bias covariance (seconds)
	double clock_rate;  //!< Clock rate covariance (meters/second)
	double solar;		//!< Solar radiation pressure covariance
	double drag;		//!< Atmospheric drag covariance
  uint32_t tic;     //!< This information is associated with the given receiver tic

} EKF_Covariance_M;


/*! @ingroup MESSAGES
 *  @brief EKF residual message
*/
typedef struct EKF_Residual_M
{

	double pseudorange_residuals[MAX_CHANNELS];		//!< Pseudorange measurement residuals
  uint32_t sv[MAX_CHANNELS];            //!< GPS PRN for residual
  uint32_t status[MAX_CHANNELS];          //!< Code for each residual
  uint32_t tic;                   //!< This information is associated with the given receiver tic

} EKF_Residual_M;


/*! @ingroup MESSAGES
 *  @brief The measurement dumped to the PVT object
 */
typedef struct Measurement_M
{

  uint32_t  tic;          //!< Measurement_tic associated with measurement
  int32_t chan;         //!< Channel
  int32_t sv;           //!< SV number
  int32_t navigate;       //!< Is the channel set to navigate
  int32_t power;          //!< Power
  int32_t   antenna;        //!< Antenna number
  int32_t subframe_sec;     //!< GPS second of the week
  int32_t _1ms_epoch;       //!< _1ms_epoch counter at time of measurement
  int32_t _20ms_epoch;      //!< _20ms_epoch counter at time of measurement
  uint32_t  frac_code_phase;    //!< Fine code phase at measurement snapshot
  uint32_t  code_phase;       //!< Coarse code phase at measurement snapshot
  uint32_t  code_rate;        //!< NCO_CODE_INCR at time of measurement snapshot
  uint32_t  frac_carrier_phase;   //!< Fine carrier phase at measurement snapshot
  uint32_t  frac_carrier_phase_prev;//!< Fine carrier phase at measurement snapshot
  uint32_t  carrier_phase;      //!< Coarse carrier phase at measurement snapshot
  uint32_t  carrier_phase_prev;   //!< Coarse carrier phase at measurement snapshot
  uint32_t  carrier_rate;     //!< NCO_CARR_INCR at time of measurement snapshot

} Measurement_M;


/*! @ingroup MESSAGES
 *  @brief Pseudoranges structure, holds both time and meters
*/
typedef struct Pseudorange_M
{

	double gpstime;			//!< Time tag associated with pseudorange
	double code_time;		//!< Time of transmission
	double meters;			//!< Pseudorange in meters
	double meters_rate;		//!< Pseudorange rate in meters/sec
	double residual;		//!< Residual in meters
	double residual_rate;	//!< Rate residual (m/s)
	double uncorrected;		//!< Raw pseudorange measurements
  uint32_t tic;       //!< Corresponds to this tic
  uint32_t chan;      //!< For this channel
  uint32_t sv;        //!< For this channel

} Pseudorange_M;


/*! @ingroup MESSAGES
 *  @brief Decoded ephemeris struct
*/
typedef struct Ephemeris_M
{
	double tgd;					//!< Group delay parameter.
	double toc;					//!< Reference time of clock data parameter set.
	double af0;					//!< Clock correction polynomial coefficient.
	double af1;					//!< Clock correction polynomial coefficient.
	double af2;					//!< Clock correction polynomial coefficient.
	double crs;					//!< Sine harmonic correction to orbital radius.
	double deltan;				//!< Mean motion delta from computed value.
	double m0;					//!< Mean anomaly at TOE.
	double cuc;					//!< Cosine harmonic correction to orbital radius.
	double ecc;					//!< Eccentricity.
	double cus;					//!< Sine harmonic corr to argument of latitude.
	double sqrta;				//!< Square root of semimajor axis.
	double toe;					//!< Reference time of ephemeris data set.
	double cic; 				//!< Cosine harmonic corr to inclination.
	double om0; 				//!< Right ascension at TOE.
	double cis;					//!< Sine harmonic corr to inclination.
	double in0; 				//!< Inclination at TOE.
	double crc; 				//!< Cosine harmonic correction to orbital radius.
	double argp; 				//!< Argument of perigee at TOE.
	double omd; 				//!< Rate of right ascension.
	double idot; 				//!< Rate of inclination.
	double a; 					//!< Derived qty: a = sqrta**2.
	double n0; 					//!< Derived qty: n0 = sqrt(GravConstant/(a*a*a)).
	double relativistic; 		//!< Relativistic correction

  uint32_t sv;          //!< SV #
  uint32_t zcount;        //!< zcount?
  uint32_t toewk;         //!< GPS week corresponding to toe.
  uint32_t fti;         //!< Fit interval.
  uint32_t iode;        //!< Issue of data, ephemeris.
  uint32_t tocwk;         //!< GPS week corresponding to toc.
  uint32_t valid;         //!< 0=No valid data.
  uint32_t tofxmission;     //!< Time of subframe 1 transmission, sec of week.
  uint32_t tow;         //!< Truncated TOW count
  uint32_t subframe_1_health;   //!< Subframe 1 health code.
  uint32_t code_on_L2;      //!< Code on L2 flag.
  uint32_t week_number;     //!< GPS week at time of subframe 1 reception.
  uint32_t L2pdata;       //!< L2 P data flag.
  uint32_t ura;         //!< Satellite's URA code.
  uint32_t iodc;        //!< Issue of data, clock.


} Ephemeris_M;


/*! @ingroup MESSAGES
 *  @brief Decoded almanac struct
*/
typedef struct Almanac_M
{

	double ecc;					//!< Eccentricity
	double toa; 				//!< Time of Almanac
	double in0; 				//!< Inclination
	double omd; 				//!< Rate of right ascention
	double sqrta; 				//!< Sqrt of Semi-Major Axis
	double om0; 				//!< Longitude of Ascending Node
	double argp; 				//!< Argument of perigee
	double m0; 					//!< Mean Anomaly
	double af0; 				//!< Clock parameter 0
	double af1; 				//!< Clock parameter 1

  uint32_t sv;          //!< SV #
  uint32_t week;        //!< Week number
  uint32_t valid;       //!< Has this been decoded yet
  uint32_t health;        //!< Health code

} Almanac_M;


/*! @ingroup MESSAGES
 *  @brief Contains the status of the ephemeris and almanac decoding
 */
typedef struct Ephemeris_Status_M
{

  uint32_t valid[MAX_SV]; //!< Valid ephemeris
  uint32_t iode[MAX_SV];  //!< Corresponding IODE
  uint32_t avalid[MAX_SV];  //!< Valid almanac

} Ephemeris_Status_M;


/*! @ingroup MESSAGES
 *  @brief The predicted state of an SV via the almanac
 */
typedef struct SV_Prediction_M
{

	double time;				//!< GPS time of predicition
	double elev;				//!< Predicted elev
	double azim;				//!< Predicted azim
	double v_elev;				//!< Elevation of vehicle relative to SV
	double v_azim;				//!< Azimuth of vehicle relative to SV
	double delay;				//!< Predicted delay (seconds)
	double doppler;				//!< Predicted Doppler (Hz)
	double doppler_rate;			//!< Predicted Doppler rate (Hz/sec)
  int32_t sv;         //!< SV number
  int32_t visible;        //!< Should the SV be visible?
  int32_t mode;         //!< Cold/warm/hot
  int32_t tracked;        //!< Is it being tracked?
  int32_t predicted;      //!< Has it been predicted?

} SV_Prediction_M;


/*! @ingroup MESSAGES
 *  @brief State of SV select task
 */
typedef struct SV_Select_Status_M
{

	float mask_angle;			//!< Mask angle
  uint32_t mode;        //!< Mode
  uint32_t state[MAX_SV];   //!< Has the SV been predicted, and if so visible?

} SV_Select_Status_M;


/*! @ingroup MESSAGES
 *  @brief Acknowledge processing of command
*/
typedef struct Command_Ack_M
{

  uint32_t command_id;      //!< ID of command received
  uint32_t command_tic;     //!< Command sequence count
  uint32_t command_status;    //!< Status (0 for FAIL, 1 for SUCCESS)

} Command_Ack_M;


/*! @ingroup MESSAGES
 *  @brief Return a block of EEPROM
*/
typedef struct EEPROM_M
{
  int32_t bank;       //!< 0 for EEPROM 0, 1 for EEPROM 1
  int32_t offset;     //!< Offset from base (dwords)
  int32_t dwords;     //!< Number of 32 bit dwords
  uint32_t payload[64];   //!< Data
} EEPROM_M;


/*! @ingroup MESSAGES
 *  @brief Return a checksum of EEPROM
*/
typedef struct EEPROM_Chksum_M
{
  int32_t bank;       //!< 0 for EEPROM0, 1 for EEPROM 1
  int32_t offset;     //!< Offset from base (dwords)
  int32_t dwords;     //!< Number of dwords
  uint32_t checksum;    //!< Checksum of EEPROM
} EEPROM_Chksum_M;


/*! @ingroup MESSAGES
 *  @brief Return a block of SRAM
*/
typedef struct Memory_M
{
  int32_t address;      //!< Raw address
  int32_t bytes;      //!< Number of bytes
  uint8_t payload[256];   //!< Data
} Memory_M;


/*! @ingroup MESSAGES
 *  @brief Return a checksum of Memory
*/
typedef struct Memory_Chksum_M
{
  int32_t address;      //!< Raw address
  int32_t bytes;      //!< Number of bytes
  uint32_t checksum;    //!< Calculated checksum
} Memory_Chksum_M;



/*! @ingroup MESSAGES
 *  @brief Holds all the relevant messages in a single container */
typedef struct Message_Struct
{
	/* Data gets stored here! */
	Board_Health_M 		board_health;					//!< Board health message
	Task_Health_M		task_health;					//!< Task health message
	SPS_M				sps;							//!< SPS message
	TOT_M				tot;							//!< UTC information
	PPS_M				pps;							//!< PPS status
	Clock_M				clock;							//!< Clock message
	EKF_State_M			ekf_state;						//!< EKF state
	EKF_Covariance_M	ekf_covariance;					//!< EKF covariance
	EKF_Residual_M		ekf_residual;					//!< EKF residual
	Channel_M 			channel[MAX_CHANNELS+1]; 		//!< Channel health message, last element is used as a buffer
	SV_Position_M		sv_positions[MAX_CHANNELS+1];	//!< SV Positions, last element is used as a buffer
	Measurement_M 		measurements[MAX_CHANNELS+1];	//!< Measurements, last element is used as a buffer
	Pseudorange_M 		pseudoranges[MAX_CHANNELS+1];	//!< Pseudoranges, last element is used as a buffer
	Command_Ack_M		command_ack;					//!< Command acknowledgement
	Ephemeris_M			ephemerides[MAX_SV+1];			//!< Ephemeris message, last element is used as a buffer
	Almanac_M			almanacs[MAX_SV+1];				//!< Almanac message, last element is used as a buffer
	Ephemeris_Status_M	ephemeris_status;				//!< Status of ephemeris
	SV_Select_Status_M	sv_select_status;				//!< SV Select status
	SV_Prediction_M		sv_predictions[MAX_SV+1];		//!< SV state prediction
	EEPROM_M			eeprom;							//!< EEPROM dump
	EEPROM_Chksum_M		eeprom_chksum;					//!< EEPROM checksum
	Memory_M			memory;							//!< Memory dump
	Memory_Chksum_M		memory_chksum;					//!< Memory checksum
} Message_Struct;


/*! @ingroup MESSAGES
 *  @brief Union containing all the individual messages */
typedef union Message_Union
{
	Board_Health_M		board_health;
	Task_Health_M		task_health;
	SPS_M				sps;
	TOT_M				tot;
	PPS_M				pps;
	Clock_M				clock;
	EKF_State_M			ekf_state;
	EKF_Covariance_M	ekf_covariance;
	EKF_Residual_M		ekf_residual;
	Channel_M			channel;
	SV_Position_M		sv_position;
	Measurement_M		measurement;
	Pseudorange_M		pseudorange;
	//Command_M			command;
	Command_Ack_M		command_ack;
	Ephemeris_M			ephemeris;
	Almanac_M			almanac;
	Ephemeris_Status_M	ephemeris_status;
	SV_Select_Status_M	sv_select_status;
	SV_Prediction_M		sv_prediction;
	EEPROM_M			eeprom;
	EEPROM_Chksum_M		eeprom_chksum;
	Memory_M			memory;
	Memory_Chksum_M		memory_chksum;

} Message_Union;

#endif /* MESSAGES_H */

