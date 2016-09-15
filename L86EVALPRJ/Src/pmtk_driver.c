#include "pmtk_driver.h"

uint32_t HexToAscii(uint32_t _hex)
{
	uint32_t ret;
	
	if(_hex<0x0A)
	{
		ret=_hex+48;
	}
	else
	{
		ret=_hex+55;
	}
	
	return ret;
}

uint32_t AsciiToHex(uint8_t * _c, uint8_t _size)
{
	uint32_t i;
	uint32_t ret;
	
	ret=0;
	for(i=_size;i>0;i--)
	{
		if(_c[_size-i]>=48 && _c[_size-i]<=57)
		{
			ret+=(_c[_size-i]-48)*pow((double)10,(double)(i-1));
		}
		else
		{
			ret+=0;
		}
	}

	return ret;
}

uint32_t Get_Timestamp(date _date)
{
	uint32_t i=0;
	uint32_t leap=0;
	uint32_t ret=0;
	
	//Check for leap year
	if((_date.Year)%4==0)
	{
		leap=1;
	}
	else
	{
		leap=0;
	}
	
	//Years
	for(i=1970;i<_date.Year;i++)
	{
		if(i%4==0)
		{
			ret+=31622400;
		}
		else
		{
			ret+=31536000;
		}
	}
	
	//Months
	for(i=1;i<_date.Month;i++)
	{
		if(i==1 || i==3 || i==5 || i==7 || i==8 || i==10 || i==12)
		{
			//31days months
			ret+=2678400;
		}
		else if(i==4 || i==6 || i==9 || i==11)
		{
			//30days months
			ret+=2592000;
		}
		else
		{
			//February
			ret+=(28+leap)*86400;
		}
	}
		
	//Days
	ret+=(_date.Day-1)*86400;
	
	
	//Hours,minutes,seconds
	ret+=_date.Hour*3600;
	ret+=_date.Minute*60;
	ret+=_date.Second;
	
	return ret;
}

//Extract an element from a string
void ExtractStr(uint8_t *_inframe, uint8_t *_outframe, uint8_t _separator, uint8_t _position)
{
	uint8_t i=0;
	uint8_t sep_cnt=0;
	uint8_t sep_start=0;
	uint8_t sep_stop=0;
	
	for(i=0;i<strlen((char *)_inframe);i++)
	{
		if(_inframe[i]==_separator)
		{
			sep_start=sep_stop;
			sep_stop=i;
			sep_cnt++;
		}
		
		//Match
		if(sep_cnt==_position)
		{
			break;
		}
	}
	
	//Extraction
	if(sep_cnt==_position)
	{
		for(i=sep_start+1;i<sep_stop;i++)
		{
			_outframe[i-sep_start-1]=_inframe[i];
		}
	}
	
	return;
}


uint8_t RMC_Convert(uint8_t *_frame, RMChex *_hexframe)
{
	RMCtxt txtframe;
	date tmstmp;
	uint8_t ValidFrame;
	uint8_t ret;	
	
	uint8_t lathex[4];
	uint8_t lonhex[4];
	float lat=0;
	float lon=0;
	
	ExtractStr(_frame,&ValidFrame,',',3);
	
	//Check if frame is valid
	if(ValidFrame != 'A')
	{
		_hexframe->FixFlag=0;
		ret=1;
	}
	else
	{
		
		//Extract arguments
		ExtractStr(_frame,txtframe.Date,',',10);
		ExtractStr(_frame,txtframe.UTC_Time,',',2);
		ExtractStr(_frame,txtframe.Latitute,',',4);
		ExtractStr(_frame,&txtframe.NS,',',5);
		ExtractStr(_frame,txtframe.Longitude,',',6);
		ExtractStr(_frame,&txtframe.EW,',',7);
		ExtractStr(_frame,txtframe.Speed,',',8);
		
		//Generate timestamp
		tmstmp.Day=AsciiToHex(txtframe.Date,2);
		tmstmp.Month=AsciiToHex(txtframe.Date+2,2);
		tmstmp.Year=AsciiToHex(txtframe.Date+4,2)+2000;
		tmstmp.Hour=AsciiToHex(txtframe.UTC_Time,2);
		tmstmp.Minute=AsciiToHex(txtframe.UTC_Time+2,2);
		tmstmp.Second=AsciiToHex(txtframe.UTC_Time+4,2);
		_hexframe->UnixTime=Get_Timestamp(tmstmp);
		
		//Convert latitude
		lat=(float)AsciiToHex(txtframe.Latitute,2)+(50000*(float)AsciiToHex(txtframe.Latitute+2,2)+5*(float)AsciiToHex(txtframe.Latitute+5,4))/3000000;
			
		//Check for latitude sign
		if(txtframe.NS=='S')
		{
			lat*=(-1);
		}
		
		//Convert longitude
		lon=(float)AsciiToHex(txtframe.Longitude,3)+(50000*(float)AsciiToHex(txtframe.Longitude+3,2)+5*(float)AsciiToHex(txtframe.Longitude+6,4))/3000000;
			
		//Check for latitude sign
		if(txtframe.EW=='W')
		{
			lon*=(-1);
		}
		
		//Float-to-bit conversion
		memcpy(lathex,(float *)&lat,4);
		memcpy(lonhex,(float *)&lon,4);	
		_hexframe->Latitude=lathex[0] | lathex[1]<<8 | lathex[2]<<16 | lathex[3]<<24;
		_hexframe->Longitude=lonhex[0] | lonhex[1]<<8 | lonhex[2]<<16 | lonhex[3]<<24;
	
		//Speed (not implemented yet)
		_hexframe->Speed=0;

		//Compute checksum
		_hexframe->Checksum=0;
		_hexframe->Checksum^=(_hexframe->UnixTime & 0xFF000000)>>24;
		_hexframe->Checksum^=(_hexframe->UnixTime & 0x00FF0000)>>16;
		_hexframe->Checksum^=(_hexframe->UnixTime & 0x0000FF00)>>8;
		_hexframe->Checksum^=(_hexframe->UnixTime & 0x000000FF);
		
		_hexframe->Checksum^=_hexframe->FixFlag;
		
		_hexframe->Checksum^=((uint32_t)_hexframe->Latitude & 0xFF000000)>>24;
		_hexframe->Checksum^=((uint32_t)_hexframe->Latitude & 0x00FF0000)>>16;
		_hexframe->Checksum^=((uint32_t)_hexframe->Latitude & 0x0000FF00)>>8;
		_hexframe->Checksum^=((uint32_t)_hexframe->Latitude & 0x000000FF);
		
		_hexframe->Checksum^=((uint32_t)_hexframe->Longitude & 0xFF000000)>>24;
		_hexframe->Checksum^=((uint32_t)_hexframe->Longitude & 0x00FF0000)>>16;
		_hexframe->Checksum^=((uint32_t)_hexframe->Longitude & 0x0000FF00)>>8;
		_hexframe->Checksum^=((uint32_t)_hexframe->Longitude & 0x000000FF);
		
		_hexframe->Checksum^=((uint32_t)_hexframe->Speed & 0xFF00)>>8;
		_hexframe->Checksum^=((uint32_t)_hexframe->Speed & 0x00FF);
		
		ret=0;
	}
	
	return ret;
}

void BuildFrame(uint8_t *_inframe, uint8_t *_outframe, uint8_t _size)
{
	uint8_t i=0;
	uint8_t checksum=0;
	
	/* Copy _infram to outframe */
	memcpy(_outframe, _inframe, _size);
	
	/* Compute checksum */
	i=1;
	while(_outframe[i]!=0 && i<_size)
	{
		checksum^=_outframe[i];
		i++;
	}
	
	_outframe[i]='*';
	_outframe[i+1]=HexToAscii(checksum>>4);
	_outframe[i+2]=HexToAscii(checksum & 0x0F);
	
	/* CRLF */
	_outframe[i+3]='\r';
	_outframe[i+4]='\n';
	
	return;
}

uint8_t L86_Send(char *string)
{
	uint8_t ret;
	uint8_t inframe[128];
	uint8_t outframe[128];
	
	memset(inframe,0,128);
	memset(outframe,0,128);
	strcpy((char*)inframe,string);
	
	BuildFrame(inframe,outframe,strlen((char*)inframe));
	UART_Write(outframe,strlen((char*)outframe));
	ret=UART_WaitForAck();
	
	return ret;
}




