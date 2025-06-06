#include "process_manager.h"
#include "opcode.h"
#include "byte_op.h"
#include "setting.h"
#include <cstring>
#include <iostream>
#include <ctime>
using namespace std;

ProcessManager::ProcessManager()
{
  this->num = 0;
}

void ProcessManager::init()
{
}

// 2차원 벡터: (불쾌지수, 평균 전력)
uint8_t *ProcessManager::processData(DataSet *ds, int *dlen)
{
  uint8_t *ret, *p;
  int num;
  HouseData *house;
  TemperatureData *tdata;
  HumidityData *hdata;
  PowerData *pdata;
  char buf[BUFLEN];
  ret = (uint8_t *)malloc(BUFLEN);
  int avg_humid, avg_temp, discomfort_index, avg_power, sum_power;

  tdata = ds->getTemperatureData();
  hdata = ds->getHumidityData();
  num = ds->getNumHouseData();

  avg_temp = tdata->getValue();
  avg_humid = hdata->getValue();
  double heat_index = 0.5 * (avg_temp + 61.0 + ((avg_temp - 68.0) * 1.2) + (avg_humid * 0.094));
  discomfort_index = static_cast<int>(heat_index);
  
  sum_power = 0;
  for (int i=0; i<num; i++)
  {
    house = ds->getHouseData(i);
    pdata = house->getPowerData();
    sum_power += pdata->getValue();
  }
  avg_power = sum_power / num;

  memset(ret, 0, BUFLEN);
  *dlen = 0;
  p = ret;

  VAR_TO_MEM_1BYTE_BIG_ENDIAN(discomfort_index, p);
  *dlen += 1;
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(avg_power, p);
  *dlen += 2;

  return ret;
}
