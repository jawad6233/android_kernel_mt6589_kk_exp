//[Sensor]
//name = ov5647mipiraw
//
//[Preview]
//read_freq = 56000000
//pixel_line = 1896
//column_length = 923
//
//[ZSD]
//read_freq = 80000000
//pixel_line = 2852
//column_length = 1871
//
//[vPreview]
//read_freq = 80000000
//pixel_line = 2416
//column_length = 1043

#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>

#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_ov5647mipiraw.h"
#include "camera_info_ov5647mipiraw.h"
#include "camera_custom_AEPlinetable.h"
//#include "camera_custom_flicker_table.h"
#include "camera_custom_flicker_para.h"
#include <cutils/xlog.h>


static void get_flicker_para_by_preview(FLICKER_CUST_PARA* para)
{
  int i;
  int freq[9] = { 70, 80, 90, 100, 120, 130, 140, 160, 170};
  FLICKER_CUST_STATISTICS EV50_L50 = {-194, 4721, 381, -766};
  FLICKER_CUST_STATISTICS EV50_L60 = {1072, 433, 809, -404};
  FLICKER_CUST_STATISTICS EV60_L50 = {1279, 456, 1205, -478};
  FLICKER_CUST_STATISTICS EV60_L60 = {-162, 2898, 247, -642};
  for(i=0;i<9;i++)
  {
    para->flickerFreq[i]=freq[i];
  }
  para->flickerGradThreshold=30;
  para->flickerSearchRange=12;
  para->minPastFrames=3;
  para->maxPastFrames=14;
  para->EV50_L50=EV50_L50;
  para->EV50_L60=EV50_L60;
  para->EV60_L50=EV60_L50;
  para->EV60_L60=EV60_L60;
  para->EV50_thresholds[0]=-30;
  para->EV50_thresholds[1]=12;
  para->EV60_thresholds[0]=-30;
  para->EV60_thresholds[1]=12;
  para->freq_feature_index[0]=4;
  para->freq_feature_index[1]=3;
}

static void get_flicker_para_by_ZSD(FLICKER_CUST_PARA* para)
{
  int i;
  int freq[9] =  { 70, 80, 90, 100, 110, 120, 130, 140, 170};
  FLICKER_CUST_STATISTICS EV50_L50 = {-194, 4721, 381, -766};
  FLICKER_CUST_STATISTICS EV50_L60 = {3490, 133, 248, -102};
  FLICKER_CUST_STATISTICS EV60_L50 = {4162, 140, 370, -176};
  FLICKER_CUST_STATISTICS EV60_L60 = {-162, 2898, 247, -642};

  for(i=0;i<9;i++)
  {
    para->flickerFreq[i]=freq[i];
  }
  para->flickerGradThreshold=31;
  para->flickerSearchRange=28;
  para->minPastFrames=3;
  para->maxPastFrames=14;
  para->EV50_L50=EV50_L50;
  para->EV50_L60=EV50_L60;
  para->EV60_L50=EV60_L50;
  para->EV60_L60=EV60_L60;
  para->EV50_thresholds[0]=-30;
  para->EV50_thresholds[1]=12;
  para->EV60_thresholds[0]=-30;
  para->EV60_thresholds[1]=12;
  para->freq_feature_index[0]=5;
  para->freq_feature_index[1]=3;
}

static void get_flicker_para_by_vPreview(FLICKER_CUST_PARA* para)
{
  int i;
  int freq[9] =  { 70, 80, 90, 100, 120, 130, 140, 160, 170};
  FLICKER_CUST_STATISTICS EV50_L50 = {-194, 4721, 381, -766};
  FLICKER_CUST_STATISTICS EV50_L60 = {1106, 420, 785, -396};
  FLICKER_CUST_STATISTICS EV60_L50 = {1319, 442, 1168, -470};
  FLICKER_CUST_STATISTICS EV60_L60 = {-162, 2898, 247, -642};

  for(i=0;i<9;i++)
  {
    para->flickerFreq[i]=freq[i];
  }
  para->flickerGradThreshold=28;
  para->flickerSearchRange=16;
  para->minPastFrames=3;
  para->maxPastFrames=14;
  para->EV50_L50=EV50_L50;
  para->EV50_L60=EV50_L60;
  para->EV60_L50=EV60_L50;
  para->EV60_L60=EV60_L60;
  para->EV50_thresholds[0]=-30;
  para->EV50_thresholds[1]=12;
  para->EV60_thresholds[0]=-30;
  para->EV60_thresholds[1]=12;
  para->freq_feature_index[0]=4;
  para->freq_feature_index[1]=3;
}


typedef NSFeature::RAWSensorInfo<SENSOR_ID> SensorInfoSingleton_T;
namespace NSFeature {
template <>
UINT32
SensorInfoSingleton_T::
impGetFlickerPara(MINT32 sensorMode, MVOID*const pDataBuf) const
{
	XLOGD("impGetFlickerPara+ mode=%d", sensorMode);
	XLOGD("prv=%d, vdo=%d, cap=%d, zsd=%d",
	    (int)e_sensorModePreview, (int)e_sensorModeVideoPreview, (int)e_sensorModeCapture, (int)e_sensorModeZsd );
	FLICKER_CUST_PARA* para;
	para =  (FLICKER_CUST_PARA*)pDataBuf;
	if(sensorMode==e_sensorModePreview)
		get_flicker_para_by_preview(para);
	else if(sensorMode==e_sensorModeZsd||
	   sensorMode==e_sensorModeCapture)
	{
		get_flicker_para_by_ZSD(para);
	}
	else if(sensorMode==e_sensorModeVideoPreview)
	{
		get_flicker_para_by_vPreview(para);
	}
	else
	{
		XLOGD("impGetFlickerPara ERROR ln=%d", __LINE__);
		return -1;
	}
	XLOGD("impGetFlickerPara-");
	return 0;
}
}

