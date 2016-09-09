/*
	run on arduino and esp8266
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <eepromMode.h>   //  to define some common values

typedef struct {int parameterLen; char parameterValue[maxParameterLen];} parameter;
typedef struct {int parameterLen; unsigned int parameterNumericValue;} numericParameter;
class ManageParamEeprom
{
public:
ManageParamEeprom(uint8_t numberParam, int offset, char keyword[6]);
void Init();            // initialize the eeprom
void Erase(boolean paramtersOnly);            // initialize the eeprom
void PrintVersion();
void Dump(unsigned int dumpOffset,unsigned int nbByte);
uint8_t GetVersion();   // 0xff means eeprom does not match with the keyword
int GetParameterNumber();
parameter GetParameter(uint8_t number);
numericParameter GetNumericParameter(uint8_t number);
int SetParameter(uint8_t number, uint8_t len, byte value[]);
uint8_t _numberParam;
int _offset;
#define _sizeKey 6
char _keyword[_sizeKey+1];
int maxEepromSize;
};
