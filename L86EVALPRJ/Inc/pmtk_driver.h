#include "stm32l1xx_hal.h"

#ifndef PMTK_DRIVER
#define PMTK_DRIVER

#include "string.h"
#include "math.h"
#include "pmtk_hw_config.h"

typedef struct _date {
	uint32_t Year;
	uint32_t Month;
	uint32_t Day;
	uint32_t Hour;
	uint32_t Minute;
	uint32_t Second;
}date;

typedef struct _RMCtxt {
	uint8_t Date[6];
	uint8_t UTC_Time[10];
	uint8_t Latitute[9];
	uint8_t Longitude[10];
	uint8_t Speed[5];
	uint8_t NS;
	uint8_t EW;
}RMCtxt;

typedef struct _RMChex {
	uint32_t UnixTime;
	uint32_t FixFlag;
	uint32_t Latitude;
	uint32_t Longitude;
	uint32_t Speed;
	uint32_t Checksum;
}RMChex;

void ExtractStr(uint8_t *_inframe, uint8_t *_outframe, uint8_t _separator, uint8_t _position);
void BuildFrame(uint8_t *_inframe, uint8_t *_outframe, uint8_t _size);

uint8_t RMC_Convert(uint8_t * _frame, RMChex * _hexframe);
 
uint32_t HexToAscii(uint32_t _hex);
uint32_t AsciiToHex(uint8_t * _c, uint8_t _size);

uint32_t Get_Timestamp(date _date);
uint8_t L86_Send(char *string);
#endif
