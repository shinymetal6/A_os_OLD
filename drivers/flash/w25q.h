/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : fy-201023-00 
*/
/*
 * w25q.h
 *
 *  Created on: Dec 27, 2023
 *      Author: fil
 */

#ifndef DRIVERS_FLASH_W25Q_H_
#define DRIVERS_FLASH_W25Q_H_

// Memory settings
// Mem size in M-bit
#define MEM_FLASH_SIZE 		128U 				// 128 MB-bit
#define MEM_BLOCK_SIZE 		64U					// 64 KB: 256 pages
#define MEM_SBLOCK_SIZE 	32U					// 32 KB: 128 pages
#define MEM_SECTOR_SIZE 	4U					// 4 KB : 16 pages
#define MEM_PAGE_SIZE  		256U				// 256 byte : 1 page
#define BLOCK_COUNT 		(MEM_FLASH_SIZE * 2)// 512 blocks
#define SECTOR_COUNT 		(BLOCK_COUNT * 16)  // 8192 sectors
#define PAGE_COUNT 			(SECTOR_COUNT * 16)	// 131'072 pages

#define	W25Q_OK 			0  	/* Chip OK - Execution fine */
#define	W25Q_BUSY 			1	/* Chip busy */
#define	W25Q_PARAM_ERR		2 	/* Function parameters error */
#define	W25Q_CHIP_ERR		3	/* Chip error */
#define	W25Q_SPI_ERR		4 	/* SPI Bus err */
#define	W25Q_CHIP_IGNORE	5 	/* Chip ignore state */

#define W25Q_WRITE_ENABLE 0x06U			///< sets WEL bit, must be set before any write/program/erase
#define W25Q_WRITE_DISABLE 0x04U		///< resets WEL bit (state after power-up)
#define W25Q_ENABLE_VOLATILE_SR 0x50U	///< check 7.1 in datasheet
#define W25Q_READ_SR1 0x05U				///< read status-register 1
#define W25Q_READ_SR2 0x35U				///< read status-register 2
#define W25Q_READ_SR3 0x15U				///< read ststus-register 3
#define W25Q_WRITE_SR1 0x01U			///< write status-register 1 (8.2.5)
#define W25Q_WRITE_SR2 0x31U			///< write status-register 2 (8.2.5)
#define W25Q_WRITE_SR3 0x11U			///< write status-register 3 (8.2.5)
#define W25Q_READ_EXT_ADDR_REG 0xC8U	///< read extended addr reg (only in 3-byte mode)
#define W25Q_WRITE_EXT_ADDR_REG 0xC8U	///< write extended addr reg (only in 3-byte mode)
#define W25Q_ENABLE_4B_MODE 0xB7U			///< enable 4-byte mode (128+ MB address)
#define W25Q_DISABLE_4B_MODE 0xE9U			///< disable 4-byte mode (<=128MB)
#define W25Q_READ_DATA 0x03U				///< read data by standard SPI
#define W25Q_READ_DATA_4B 0x13U				///< read data by standard SPI in 4-byte mode
#define W25Q_FAST_READ 0x0BU				///< highest FR speed (8.2.12)
#define W25Q_FAST_READ_4B 0x0CU				///< fast read in 4-byte mode
#define W25Q_FAST_READ_DUAL_OUT 0x3BU		///< fast read in dual-SPI OUTPUT (8.2.14)
#define W25Q_FAST_READ_DUAL_OUT_4B 0x3CU	///< fast read in dual-SPI OUTPUT in 4-byte mode
#define W25Q_FAST_READ_QUAD_OUT 0x6BU		///< fast read in quad-SPI OUTPUT (8.2.16)
#define W25Q_FAST_READ_QUAD_OUT_4B 0x6CU	///< fast read in quad-SPI OUTPUT in 4-byte mode
#define W25Q_FAST_READ_DUAL_IO 0xBBU		///< fast read in dual-SPI I/O (address transmits by both lines)
#define W25Q_FAST_READ_DUAL_IO_4B 0xBCU		///< fast read in dual-SPI I/O in 4-byte mode
#define W25Q_FAST_READ_QUAD_IO 0xEBU		///< fast read in quad-SPI I/O (address transmits by quad lines)
#define W25Q_FAST_READ_QUAD_IO_4B 0xECU		///< fast read in quad-SPI I/O in 4-byte mode
#define W25Q_SET_BURST_WRAP 0x77U			///< use with quad-I/O (8.2.22)
#define W25Q_PAGE_PROGRAM 0x02U				///< program page (256bytes) by single SPI line
#define W25Q_PAGE_PROGRAM_4B 0x12U			///< program page by single SPI in 4-byte mode
#define W25Q_PAGE_PROGRAM_QUAD_INP 0x32U	///< program page (256bytes) by quad SPI lines
#define W25Q_PAGE_PROGRAM_QUAD_INP_4B 0x34U ///< program page by quad SPI in 4-byte mode
#define W25Q_SECTOR_ERASE 0x20U				///< sets all 4Kbyte sector with 0xFF (erases it)
#define W25Q_SECTOR_ERASE_4B 0x21U			///< sets all 4Kbyte sector with 0xFF in 4-byte mode
#define W25Q_32KB_BLOCK_ERASE 0x52U			///< sets all 32Kbyte block with 0xFF
#define W25Q_64KB_BLOCK_ERASE 0xD8U			///< sets all 64Kbyte block with 0xFF
#define W25Q_64KB_BLOCK_ERASE_4B 0xDCU		///< sets all 64Kbyte sector with 0xFF in 4-byte mode
#define W25Q_CHIP_ERASE 0xC7U				///< fill all the chip with 0xFF
//#define W25Q_CHIP_ERASE 0x60U				///< another way to erase chip
#define W25Q_ERASEPROG_SUSPEND 0x75U		///< suspend erase/program operation (can be applied only when SUS=0, BYSY=1)
#define W25Q_ERASEPROG_RESUME 0x7AU			///< resume erase/program operation (if SUS=1, BUSY=0)
#define W25Q_POWERDOWN 0xB9U				///< powers down the chip (power-up by reading ID)
#define W25Q_POWERUP 0xABU					///< release power-down
#define W25Q_DEVID 0xABU					///< read Device ID (same as powerup)
#define W25Q_FULLID 0x90U					///< read Manufacturer ID & Device ID
#define W25Q_FULLID_DUAL_IO 0x92U			///< read Manufacturer ID & Device ID by dual I/O
#define W25Q_FULLID_QUAD_IO 0x94U			///< read Manufacturer ID & Device ID by quad I/O
#define W25Q_READ_UID 0x4BU					///< read unique chip 64-bit ID
#define W25Q_READ_JEDEC_ID 0x9FU			///< read JEDEC-standard ID
#define W25Q_READ_SFDP 0x5AU				///< read SFDP register parameters
#define W25Q_ERASE_SECURITY_REG 0x44U		///< erase security registers
#define W25Q_PROG_SECURITY_REG 0x42U		///< program security registers
#define W25Q_READ_SECURITY_REG 0x48U		///< read security registers
#define W25Q_IND_BLOCK_LOCK 0x36U			///< make block/sector read-only
#define W25Q_IND_BLOCK_UNLOCK 0x39U			///< disable block/sector protection
#define W25Q_READ_BLOCK_LOCK 0x3DU			///< check block/sector protection
#define W25Q_GLOBAL_LOCK 0x7EU				///< global read-only protection enable
#define W25Q_GLOBAL_UNLOCK 0x98U			///< global read-only protection disable
#define W25Q_ENABLE_RST 0x66U				///< enable software-reset ability
#define W25Q_RESET 0x99U					///< make software reset

extern	uint8_t w25q_Init(void);
extern	uint8_t w25q_ReadRaw(uint8_t *buf, uint16_t data_len, uint32_t rawAddr);
extern	uint8_t w25q_WriteEnable(uint8_t enable);
extern	uint8_t w25q_ProgramRaw(uint8_t *buf, uint16_t data_len, uint32_t rawAddr);
extern	uint8_t w25q_EraseSector(uint32_t SectAddr);
extern	uint8_t w25q_EraseBlock(uint32_t BlockAddr, uint8_t size);
extern	uint8_t w25q_EraseChip(void);


#endif /* DRIVERS_FLASH_W25Q_H_ */
