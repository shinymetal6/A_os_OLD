/*
 * ftm.h
 *
 *  Created on: Aug 7, 2023
 *      Author: fil
 */

#ifndef APPLICATION_FTM_H_
#define APPLICATION_FTM_H_

#define ST25_PRODUCT_CODE_UID_INDEX (5)
#define ST25DV04K_SYSTEM_CONFIG_MB_MODE 0x0D
#define ST25DV04K_REG_MB_MODE_FTM_AUTHORIZED 1U
#define ST25DV04K_DYN_REG_MB_CTRL 0x0D
#define ST25DV04K_REG_MB_CTRL_DYN_MB_EN 1U

extern	uint16_t 	ftm_init( void );
extern	uint8_t 	ftm_cycle( void );
extern	uint8_t 	ftm_messages( void );

#endif /* APPLICATION_FTM_H_ */
