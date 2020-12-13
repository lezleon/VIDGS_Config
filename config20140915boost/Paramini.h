#pragma once
#include "ini.h"

class CParamini :
	public CIni
{
public:
	CParamini(void);
	~CParamini(void);

	virtual void Init(CString filename);
public:	
	static CString 
		g_strSection_Station,
		g_str_StationNum,
		g_str_PlacementNum;

public:	
	static CString 
		g_strSection,
		g_str_search_rect_x,
		g_str_search_rect_y,
		g_str_search_rect_width,
		g_str_search_rect_height;
	static CString 
		g_strSection_Track,
		g_str_track_rect_x,
		g_str_track_rect_y,
		g_str_track_rect_width,
		g_str_track_rect_height;

public:	
	static CString 
		g_strSection_Bridge,
		g_str_bridge_rect_x,
		g_str_bridge_rect_y,
		g_str_bridge_rect_width,
		g_str_bridge_rect_height;

public:	
	static CString 
		g_strSection_Identify,
		g_str_Identify_rect_x,
		g_str_Identify_rect_y,
		g_str_Identify_rect_width,
		g_str_Identify_rect_height;
public:	
	static CString 
		g_strSection_line,
		g_str_line_start_x,
		g_str_line_start_y,
		g_str_line_end_x,
		g_str_line_end_y,

		g_str_line2_start_x,
		g_str_line2_start_y,
		g_str_line2_end_x,
		g_str_line2_end_y,
		
		g_str_line3_start_x,
		g_str_line3_start_y,
		g_str_line3_end_x,
		g_str_line3_end_y,
		
		g_str_line4_start_x,
		g_str_line4_start_y,
		g_str_line4_end_x,
		g_str_line4_end_y,

		g_str_line5_start_x,
		g_str_line5_start_y,
		g_str_line5_end_x,
		g_str_line5_end_y;

public:	
	static CString 
		g_strSection_Stopline,
		g_str_stopline11_start_x,
		g_str_stopline11_start_y,
		g_str_stopline11_end_x,
		g_str_stopline11_end_y,

		g_str_stopline12_start_x,
		g_str_stopline12_start_y,
		g_str_stopline12_end_x,
		g_str_stopline12_end_y,

		g_str_stopline13_start_x,
		g_str_stopline13_start_y,
		g_str_stopline13_end_x,
		g_str_stopline13_end_y,

		g_str_stopline14_start_x,
		g_str_stopline14_start_y,
		g_str_stopline14_end_x,
		g_str_stopline14_end_y,

		g_str_stopline15_start_x,
		g_str_stopline15_start_y,
		g_str_stopline15_end_x,
		g_str_stopline15_end_y,

		g_str_stopline16_start_x,
		g_str_stopline16_start_y,
		g_str_stopline16_end_x,
		g_str_stopline16_end_y,

		g_str_stopline21_start_x,
		g_str_stopline21_start_y,
		g_str_stopline21_end_x,
		g_str_stopline21_end_y,

		g_str_stopline22_start_x,
		g_str_stopline22_start_y,
		g_str_stopline22_end_x,
		g_str_stopline22_end_y,

		g_str_stopline23_start_x,
		g_str_stopline23_start_y,
		g_str_stopline23_end_x,
		g_str_stopline23_end_y,
		
		g_str_stopline24_start_x,
		g_str_stopline24_start_y,
		g_str_stopline24_end_x,
		g_str_stopline24_end_y,

		g_str_stopline25_start_x,
		g_str_stopline25_start_y,
		g_str_stopline25_end_x,
		g_str_stopline25_end_y,

		g_str_stopline26_start_x,
		g_str_stopline26_start_y,
		g_str_stopline26_end_x,
		g_str_stopline26_end_y,

		g_str_stopline31_start_x,
		g_str_stopline31_start_y,
		g_str_stopline31_end_x,
		g_str_stopline31_end_y,

		g_str_stopline32_start_x,
		g_str_stopline32_start_y,
		g_str_stopline32_end_x,
		g_str_stopline32_end_y,

		g_str_stopline33_start_x,
		g_str_stopline33_start_y,
		g_str_stopline33_end_x,
		g_str_stopline33_end_y,

		g_str_stopline34_start_x,
		g_str_stopline34_start_y,
		g_str_stopline34_end_x,
		g_str_stopline34_end_y,

		g_str_stopline35_start_x,
		g_str_stopline35_start_y,
		g_str_stopline35_end_x,
		g_str_stopline35_end_y,

		g_str_stopline36_start_x,
		g_str_stopline36_start_y,
		g_str_stopline36_end_x,
		g_str_stopline36_end_y,

		g_str_stopline41_start_x,
		g_str_stopline41_start_y,
		g_str_stopline41_end_x,
		g_str_stopline41_end_y,

		g_str_stopline42_start_x,
		g_str_stopline42_start_y,
		g_str_stopline42_end_x,
		g_str_stopline42_end_y,

		g_str_stopline43_start_x,
		g_str_stopline43_start_y,
		g_str_stopline43_end_x,
		g_str_stopline43_end_y,

		g_str_stopline44_start_x,
		g_str_stopline44_start_y,
		g_str_stopline44_end_x,
		g_str_stopline44_end_y,

		g_str_stopline45_start_x,
		g_str_stopline45_start_y,
		g_str_stopline45_end_x,
		g_str_stopline45_end_y,

		g_str_stopline46_start_x,
		g_str_stopline46_start_y,
		g_str_stopline46_end_x,
		g_str_stopline46_end_y,

		g_str_stopline51_start_x,
		g_str_stopline51_start_y,
		g_str_stopline51_end_x,
		g_str_stopline51_end_y,

		g_str_stopline52_start_x,
		g_str_stopline52_start_y,
		g_str_stopline52_end_x,
		g_str_stopline52_end_y,

		g_str_stopline53_start_x,
		g_str_stopline53_start_y,
		g_str_stopline53_end_x,
		g_str_stopline53_end_y,

		g_str_stopline54_start_x,
		g_str_stopline54_start_y,
		g_str_stopline54_end_x,
		g_str_stopline54_end_y,

		g_str_stopline55_start_x,
		g_str_stopline55_start_y,
		g_str_stopline55_end_x,
		g_str_stopline55_end_y,

		g_str_stopline56_start_x,
		g_str_stopline56_start_y,
		g_str_stopline56_end_x,
		g_str_stopline56_end_y;


public:	
	static CString 
		g_strSection_PlaneType,
		g_str_planetype11_1,
		g_str_planetype11_2,
		g_str_planetype11_3,
		g_str_planetype11_4,
		g_str_planetype11_5,
		g_str_planetype11_6,
		g_str_planetype11_7,
		g_str_planetype11_8,
		g_str_planetype11_9,
		g_str_planetype11_10,
		g_str_planetype11_11,
		g_str_planetype11_12,
		g_str_planetype11_13,
		g_str_planetype11_14,
		g_str_planetype11_15,
		g_str_planetype11_16,

		g_str_planetype12_1,
		g_str_planetype12_2,
		g_str_planetype12_3,
		g_str_planetype12_4,
		g_str_planetype12_5,
		g_str_planetype12_6,
		g_str_planetype12_7,
		g_str_planetype12_8,
		g_str_planetype12_9,
		g_str_planetype12_10,
		g_str_planetype12_11,
		g_str_planetype12_12,
		g_str_planetype12_13,
		g_str_planetype12_14,
		g_str_planetype12_15,
		g_str_planetype12_16,

		g_str_planetype13_1,
		g_str_planetype13_2,
		g_str_planetype13_3,
		g_str_planetype13_4,
		g_str_planetype13_5,
		g_str_planetype13_6,
		g_str_planetype13_7,
		g_str_planetype13_8,
		g_str_planetype13_9,
		g_str_planetype13_10,
		g_str_planetype13_11,
		g_str_planetype13_12,
		g_str_planetype13_13,
		g_str_planetype13_14,
		g_str_planetype13_15,
		g_str_planetype13_16,

		g_str_planetype21_1,
		g_str_planetype21_2,
		g_str_planetype21_3,
		g_str_planetype21_4,
		g_str_planetype21_5,
		g_str_planetype21_6,
		g_str_planetype21_7,
		g_str_planetype21_8,
		g_str_planetype21_9,
		g_str_planetype21_10,
		g_str_planetype21_11,
		g_str_planetype21_12,
		g_str_planetype21_13,
		g_str_planetype21_14,
		g_str_planetype21_15,
		g_str_planetype21_16,

		g_str_planetype22_1,
		g_str_planetype22_2,
		g_str_planetype22_3,
		g_str_planetype22_4,
		g_str_planetype22_5,
		g_str_planetype22_6,
		g_str_planetype22_7,
		g_str_planetype22_8,
		g_str_planetype22_9,
		g_str_planetype22_10,
		g_str_planetype22_11,
		g_str_planetype22_12,
		g_str_planetype22_13,
		g_str_planetype22_14,
		g_str_planetype22_15,
		g_str_planetype22_16,

		g_str_planetype23_1,
		g_str_planetype23_2,
		g_str_planetype23_3,
		g_str_planetype23_4,
		g_str_planetype23_5,
		g_str_planetype23_6,
		g_str_planetype23_7,
		g_str_planetype23_8,
		g_str_planetype23_9,
		g_str_planetype23_10,
		g_str_planetype23_11,
		g_str_planetype23_12,
		g_str_planetype23_13,
		g_str_planetype23_14,
		g_str_planetype23_15,
		g_str_planetype23_16;

		/*g_str_planetype1,
		g_str_planetype2,
		g_str_planetype3,
		g_str_planetype4,
		g_str_planetype5,
		g_str_planetype6,
		g_str_planetype7,
		g_str_planetype8,
		g_str_planetype9,
		g_str_planetype10,
		g_str_planetype11,
		g_str_planetype12,
		g_str_planetype13,
		g_str_planetype14,
		g_str_planetype15,
		g_str_planetype16,*/
public:	
	static CString 
		g_strSection_Point,
		g_str_point1_x,
		g_str_point1_y,
		g_str_point2_x,
		g_str_point2_y,
		g_str_point3_x,
		g_str_point3_y,
		g_str_point4_x,
		g_str_point4_y,
		g_str_point5_x,
		g_str_point5_y,
		g_str_point6_x,
		g_str_point6_y,
		g_str_point7_x,
		g_str_point7_y,
		g_str_point8_x,
		g_str_point8_y,
		g_str_point9_x,
		g_str_point9_y,
		g_str_point10_x,
		g_str_point10_y,
		g_str_point11_x,
		g_str_point11_y,
		g_str_point12_x,
		g_str_point12_y,
		g_str_point13_x,
		g_str_point13_y,
		g_str_point14_x,
		g_str_point14_y,
		g_str_point15_x,
		g_str_point15_y,
		g_str_point16_x,
		g_str_point16_y,
		g_str_point17_x,
		g_str_point17_y,
		g_str_point18_x,
		g_str_point18_y,
		g_str_point19_x,
		g_str_point19_y,
		g_str_point20_x,
		g_str_point20_y;

public:
	static CString 
		g_strSection_StopDistance,
		g_str_stopdistance;
public:
	static CString 
		g_strSection_ToofarDistance,
		g_str_toofardistance;
public:
	static CString 
		g_strSection_Speed,
		g_str_speed;
public:
	static CString 
		g_strSection_PCTime,
		g_str_pctime;

};

extern CParamini g_ParamIni;

