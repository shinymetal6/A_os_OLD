/*
 * logger.h
 *
 *  Created on: Aug 22, 2023
 *      Author: fil
 */

#ifndef APPLICATION_LOGGER_H_
#define APPLICATION_LOGGER_H_

/* packet offset */
#define	PKT_OPEN_FLAG			0
#define	PKT_LABEL_FLAG			1
#define	PKT_DATAPRESENT_FLAG	2
#define	DATALEN_OFFSET			3
#define	DATA_COUNTER_OFFSET		4
#define	DATA_OFFSET				5

#define	MAX_DATA_LEN			32
typedef struct  _PacketTypeDef
{
	uint8_t			opening_flag;	//<
	uint8_t			cmd_flag;		// X
	uint8_t			data_flag;		// :
	uint8_t			pkt_counter;	// counter
	uint8_t			temperature;	// temperature
	uint8_t			data[MAX_DATA_LEN*6];
	uint8_t			bottom[128];
}PacketTypeDef;

extern	void log_Init(void);
extern	void log_data(uint8_t pktlen);


#endif /* APPLICATION_LOGGER_H_ */
