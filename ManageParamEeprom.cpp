/*
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Written by Jean Cuiller
*/
/*
EEPROM structure:
	byte 0: version number (for future use)
	byte 1-x: keyword that is used to check the eeprom structure
	byte x+1: number of to be stored parameter
	byte x+2: length of the first parameter (number 1)
	byte x+2+1-x+2+1+length of the first parameter: length of second parameter
		and so on

set parameter must be done in increasing order 	
parameter length can not be modified

*/
#include <Arduino.h>
#include <EEPROM.h>
#include <ManageParamEeprom.h>
uint8_t eepromVersion=0x01;




ManageParamEeprom::ManageParamEeprom ( uint8_t numberParam, int offset,char keyword[6])
	{
		_numberParam=numberParam;
		_offset=offset;
		for (int i=0;i<_sizeKey;i++)
		{	
			_keyword[i]=keyword[i];
		}
			Serial.println();
//		_sizeKey=sizeof(keyword);
		 maxEepromSize = maxParameterLen*maxParameterNumber + _sizeKey +2;
	}
	
void ManageParamEeprom::Init()
	{
	Serial.print("EEPROM initialization");
#if defined ESP_H              // means ESP8266
	EEPROM.begin(_sizeKey+2);
#endif
	Serial.println();
	if (EEPROM.read(_offset) != eepromVersion)
		{
			EEPROM.write(_offset, eepromVersion);
			Serial.print(".");
		}
	for (int i=0;i<_sizeKey;i++)
		{
			if (EEPROM.read(_offset+i+1) != _keyword[i])
			{
				EEPROM.write(_offset+i+1, _keyword[i]);
				Serial.print(".");
			}
		}
	if (EEPROM.read(_sizeKey + 1) != _numberParam)
		{
			EEPROM.write(_sizeKey + 1, _numberParam);
			Serial.print(".");
		}
#if defined ESP_H
	EEPROM.commit();
	delay(1000);
#endif
	Serial.println("Initialization completed");
	}
	
void ManageParamEeprom::PrintVersion()
	{
#if defined ESP_H
		EEPROM.begin(maxEepromSize);
#endif
#if defined ARDUINOMode
//	Serial.println("compile mode arduino");
#endif	
#if defined ESP_H 
	Serial.println("mode compile esp8266");
#endif
		Serial.print("Version:");
		Serial.println(EEPROM.read(_offset) ,HEX);
		Serial.print("keyword:");
	for (int i=0;i<_sizeKey;i++)
		{
			Serial.print(EEPROM.read(_offset+i+1),HEX) ;
			Serial.print("-");
		}
	Serial.println();	
	Serial.print("Paramters number:");
	Serial.println(EEPROM.read(_sizeKey+1));
	}
	
uint8_t ManageParamEeprom::GetVersion()
	{
#if defined ESP_H
	EEPROM.begin(_sizeKey+2);
#endif
	boolean keyOk=true;
	uint8_t rc=EEPROM.read(_offset);
	for (int i=0;i<_sizeKey;i++)
		{
			if(EEPROM.read(_offset+i+1) != _keyword[i]) 
			{
				keyOk=false;
				break;
			}
		}
	if(EEPROM.read(_sizeKey+1) != _numberParam) 
		{
			keyOk=false;
		}
	if (keyOk == false)
		{
			rc=0xff;
		}
	return(rc);
	}
	
int ManageParamEeprom::GetParameterNumber()
	{
#if defined ESP_H
	EEPROM.begin(_sizeKey+2);
#endif
		return (EEPROM.read(_sizeKey+1)) ;
	}
parameter ManageParamEeprom::GetParameter(uint8_t number)
	{
#if defined ESP_H
		EEPROM.begin(maxEepromSize);
#endif
		parameter rc = { -1, "ko"};
		int shift=_sizeKey+2;
		if(number > maxParameterNumber || number<0)
		{
			return(rc);
		}
		for (int i=0;i<number;i++)
		{
//			Serial.print(EEPROM.read(shift),HEX);
//			Serial.print("-");
			if(EEPROM.read(shift)==0x00 || EEPROM.read(shift) > maxParameterLen)
			{
				return(rc);
			}
			shift=shift+1+EEPROM.read(shift);	
		}
//		Serial.println();
		rc.parameterLen=EEPROM.read(shift);
		for (int i=0; i < rc.parameterLen ; i++)
		{
			rc.parameterValue[i]=EEPROM.read(shift+1+i);
//			Serial.print(rc.parameterValue[i],HEX);
//			Serial.print("+");
		}
//		Serial.println();
		return (rc);
	}
numericParameter ManageParamEeprom::GetNumericParameter(uint8_t number)
	{
#if defined ESP_H
		EEPROM.begin(maxEepromSize);
#endif
		numericParameter  rc = { -1, 0};
		int shift=_sizeKey+2;
		if(number > maxParameterNumber || number<0)
		{
			return(rc);
		}
		for (int i=0;i<number;i++)
		{
//			Serial.print(EEPROM.read(shift),HEX);
//			Serial.print("-");
			if(EEPROM.read(shift)==0x00 || EEPROM.read(shift) > maxParameterLen)
			{
				return(rc);
			}
			shift=shift+1+EEPROM.read(shift);	
		}
//		Serial.println();
		rc.parameterLen=EEPROM.read(shift);

		for (int i=0; i < rc.parameterLen ; i++)
		{
			int pwr=1;
	      for (int j = i; j < rc.parameterLen - 1; j++)
			{
				pwr = pwr * 256;
			}
			rc.parameterNumericValue=rc.parameterNumericValue+EEPROM.read(shift+1+i)*pwr;
//			Serial.print(EEPROM.read(shift+1+i),HEX);
//			Serial.print("=");
//			Serial.print(rc.parameterNumericValue);
//			Serial.print("+");
		}
//		Serial.println();
		return (rc);
	}
int ManageParamEeprom::SetParameter(uint8_t number, uint8_t len, byte value[maxParameterLen])
	{
#if defined ESP_H
		EEPROM.begin(maxEepromSize);
#endif
		int shift=_sizeKey+2;
		if (number > maxParameterNumber || number < 0)
		{
			return(-1);
		}
		if (len > maxParameterLen || len == 0)
		{
			return(-2);
		}
		for (int i=0;i<number;i++)
		{
			if(EEPROM.read(shift)==0x00 || EEPROM.read(shift) > maxParameterLen)
			{
				return(-3);
			}
			shift=shift+EEPROM.read(shift)+1;	
		}
		if (EEPROM.read(shift)!=len)
		{
			EEPROM.write(shift, len);
			Serial.print(".");
		}
		for (int i=0;i<len;i++)
		{
			if (EEPROM.read(shift+1+i)!=value[i])
			{
				EEPROM.write((shift+1+i), value[i]);
				Serial.print(".");
			}
		}
#if defined ESP_H
		EEPROM.commit();
		delay(1000);
#endif
//		Serial.println("eeprom updated");
		return(0);
	}
void ManageParamEeprom::Dump(unsigned int dumpOffset,unsigned int nbByte)
	{
#if defined ESP_H
		EEPROM.begin(maxEepromSize);
#endif
		for (int i = 0; i < nbByte; i++)
		{
			Serial.print(EEPROM.read(_offset+i+dumpOffset), HEX);
			Serial.print("-");
		}
		Serial.println();
	}
void ManageParamEeprom::Erase(boolean paramtersOnly)
	{
		int startByte=_offset;
		int endByte=maxEepromSize;
		if (paramtersOnly == true)
		{
			startByte=startByte+_sizeKey+2;
			endByte=endByte-(_sizeKey+2);
		}
#if defined ESP_H
		EEPROM.begin(maxEepromSize);
#endif
		Serial.print("Erase starting from:");
		Serial.print(startByte);
		Serial.print(" to:");
		Serial.println(endByte);
		for (int i = 0; i < endByte; i++)
		{
			if (EEPROM.read(startByte+i) != 0xff)
			{
				EEPROM.write((startByte+i), 0xff);
			}

		}
#if defined ESP_H
		EEPROM.commit();
		delay(2000);
#endif
		Serial.println("Erase completed");
	}