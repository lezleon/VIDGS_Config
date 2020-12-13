#include "StdAfx.h"
#include "Paramini.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CParamini g_ParamIni;
CString CParamini::g_strSection_Station = _T("Station");
CString CParamini::g_str_StationNum = _T("StationNum");
CString CParamini::g_str_PlacementNum = _T("PlacementNum");


CString CParamini::g_strSection = _T("CaptureRect");
CString CParamini::g_str_search_rect_x = _T("search_rect_x");
CString CParamini::g_str_search_rect_y = _T("search_rect_y");
CString CParamini::g_str_search_rect_width = _T("search_rect_width");
CString CParamini::g_str_search_rect_height = _T("search_rect_height");

CString CParamini::g_strSection_Track = _T("TrackRect");
CString CParamini::g_str_track_rect_x = _T("track_rect_x");
CString CParamini::g_str_track_rect_y = _T("track_rect_y");
CString CParamini::g_str_track_rect_width = _T("track_rect_width");
CString CParamini::g_str_track_rect_height = _T("track_rect_height");

CString CParamini::g_strSection_Bridge = _T("BridgeRect");
CString CParamini::g_str_bridge_rect_x = _T("bridge_rect_x");
CString CParamini::g_str_bridge_rect_y = _T("bridge_rect_y");
CString CParamini::g_str_bridge_rect_width = _T("bridge_rect_width");
CString CParamini::g_str_bridge_rect_height = _T("bridge_rect_height");

CString CParamini::g_strSection_Identify = _T("IdentifyRect");
CString CParamini::g_str_Identify_rect_x = _T("Identify_rect_x");
CString CParamini::g_str_Identify_rect_y = _T("Identify_rect_y");
CString CParamini::g_str_Identify_rect_width = _T("Identify_rect_width");
CString CParamini::g_str_Identify_rect_height = _T("Identify_rect_height");


CString CParamini::g_strSection_line = _T("RoadLine");
CString CParamini::g_str_line_start_x = _T("line1_start_x");
CString CParamini::g_str_line_start_y= _T("line1_start_y");
CString CParamini::g_str_line_end_x = _T("line1_end_x");
CString CParamini::g_str_line_end_y = _T("line1_end_y");

CString CParamini::g_str_line2_start_x = _T("line2_start_x");
CString CParamini::g_str_line2_start_y= _T("line2_start_y");
CString CParamini::g_str_line2_end_x = _T("line2_end_x");
CString CParamini::g_str_line2_end_y = _T("line2_end_y");

CString CParamini::g_str_line3_start_x = _T("line3_start_x");
CString CParamini::g_str_line3_start_y= _T("line3_start_y");
CString CParamini::g_str_line3_end_x = _T("line3_end_x");
CString CParamini::g_str_line3_end_y = _T("line3_end_y");

CString CParamini::g_str_line4_start_x = _T("line4_start_x");
CString CParamini::g_str_line4_start_y= _T("line4_start_y");
CString CParamini::g_str_line4_end_x = _T("line4_end_x");
CString CParamini::g_str_line4_end_y = _T("line4_end_y");

CString CParamini::g_str_line5_start_x = _T("line5_start_x");
CString CParamini::g_str_line5_start_y= _T("line5_start_y");
CString CParamini::g_str_line5_end_x = _T("line5_end_x");
CString CParamini::g_str_line5_end_y = _T("line5_end_y");

CString CParamini::g_strSection_Stopline = _T("StopLine");
CString CParamini::g_str_stopline11_start_x = _T("stopline11_start_x");
CString CParamini::g_str_stopline11_start_y= _T("stopline11_start_y");
CString CParamini::g_str_stopline11_end_x = _T("stopline11_end_x");
CString CParamini::g_str_stopline11_end_y = _T("stopline11_end_y");

CString CParamini::g_str_stopline12_start_x = _T("stopline12_start_x");
CString CParamini::g_str_stopline12_start_y= _T("stopline12_start_y");
CString CParamini::g_str_stopline12_end_x = _T("stopline12_end_x");
CString CParamini::g_str_stopline12_end_y = _T("stopline12_end_y");

CString CParamini::g_str_stopline13_start_x = _T("stopline13_start_x");
CString CParamini::g_str_stopline13_start_y= _T("stopline13_start_y");
CString CParamini::g_str_stopline13_end_x = _T("stopline13_end_x");
CString CParamini::g_str_stopline13_end_y = _T("stopline13_end_y");

CString CParamini::g_str_stopline14_start_x = _T("stopline14_start_x");
CString CParamini::g_str_stopline14_start_y= _T("stopline14_start_y");
CString CParamini::g_str_stopline14_end_x = _T("stopline14_end_x");
CString CParamini::g_str_stopline14_end_y = _T("stopline14_end_y");

CString CParamini::g_str_stopline15_start_x = _T("stopline15_start_x");
CString CParamini::g_str_stopline15_start_y= _T("stopline15_start_y");
CString CParamini::g_str_stopline15_end_x = _T("stopline15_end_x");
CString CParamini::g_str_stopline15_end_y = _T("stopline15_end_y");

CString CParamini::g_str_stopline16_start_x = _T("stopline16_start_x");
CString CParamini::g_str_stopline16_start_y= _T("stopline16_start_y");
CString CParamini::g_str_stopline16_end_x = _T("stopline16_end_x");
CString CParamini::g_str_stopline16_end_y = _T("stopline16_end_y");

CString CParamini::g_str_stopline21_start_x = _T("stopline21_start_x");
CString CParamini::g_str_stopline21_start_y= _T("stopline21_start_y");
CString CParamini::g_str_stopline21_end_x = _T("stopline21_end_x");
CString CParamini::g_str_stopline21_end_y = _T("stopline21_end_y");

CString CParamini::g_str_stopline22_start_x = _T("stopline22_start_x");
CString CParamini::g_str_stopline22_start_y= _T("stopline22_start_y");
CString CParamini::g_str_stopline22_end_x = _T("stopline22_end_x");
CString CParamini::g_str_stopline22_end_y = _T("stopline22_end_y");

CString CParamini::g_str_stopline23_start_x = _T("stopline23_start_x");
CString CParamini::g_str_stopline23_start_y= _T("stopline23_start_y");
CString CParamini::g_str_stopline23_end_x = _T("stopline23_end_x");
CString CParamini::g_str_stopline23_end_y = _T("stopline23_end_y");

CString CParamini::g_str_stopline24_start_x = _T("stopline24_start_x");
CString CParamini::g_str_stopline24_start_y= _T("stopline24_start_y");
CString CParamini::g_str_stopline24_end_x = _T("stopline24_end_x");
CString CParamini::g_str_stopline24_end_y = _T("stopline24_end_y");

CString CParamini::g_str_stopline25_start_x = _T("stopline25_start_x");
CString CParamini::g_str_stopline25_start_y= _T("stopline25_start_y");
CString CParamini::g_str_stopline25_end_x = _T("stopline25_end_x");
CString CParamini::g_str_stopline25_end_y = _T("stopline25_end_y");

CString CParamini::g_str_stopline26_start_x = _T("stopline26_start_x");
CString CParamini::g_str_stopline26_start_y= _T("stopline26_start_y");
CString CParamini::g_str_stopline26_end_x = _T("stopline26_end_x");
CString CParamini::g_str_stopline26_end_y = _T("stopline26_end_y");

CString CParamini::g_str_stopline31_start_x = _T("stopline31_start_x");
CString CParamini::g_str_stopline31_start_y= _T("stopline31_start_y");
CString CParamini::g_str_stopline31_end_x = _T("stopline31_end_x");
CString CParamini::g_str_stopline31_end_y = _T("stopline31_end_y");

CString CParamini::g_str_stopline32_start_x = _T("stopline32_start_x");
CString CParamini::g_str_stopline32_start_y= _T("stopline32_start_y");
CString CParamini::g_str_stopline32_end_x = _T("stopline32_end_x");
CString CParamini::g_str_stopline32_end_y = _T("stopline32_end_y");

CString CParamini::g_str_stopline33_start_x = _T("stopline33_start_x");
CString CParamini::g_str_stopline33_start_y= _T("stopline33_start_y");
CString CParamini::g_str_stopline33_end_x = _T("stopline33_end_x");
CString CParamini::g_str_stopline33_end_y = _T("stopline33_end_y");

CString CParamini::g_str_stopline34_start_x = _T("stopline34_start_x");
CString CParamini::g_str_stopline34_start_y= _T("stopline34_start_y");
CString CParamini::g_str_stopline34_end_x = _T("stopline34_end_x");
CString CParamini::g_str_stopline34_end_y = _T("stopline34_end_y");

CString CParamini::g_str_stopline35_start_x = _T("stopline35_start_x");
CString CParamini::g_str_stopline35_start_y= _T("stopline35_start_y");
CString CParamini::g_str_stopline35_end_x = _T("stopline35_end_x");
CString CParamini::g_str_stopline35_end_y = _T("stopline35_end_y");

CString CParamini::g_str_stopline36_start_x = _T("stopline36_start_x");
CString CParamini::g_str_stopline36_start_y= _T("stopline36_start_y");
CString CParamini::g_str_stopline36_end_x = _T("stopline36_end_x");
CString CParamini::g_str_stopline36_end_y = _T("stopline36_end_y");

CString CParamini::g_str_stopline41_start_x = _T("stopline41_start_x");
CString CParamini::g_str_stopline41_start_y= _T("stopline41_start_y");
CString CParamini::g_str_stopline41_end_x = _T("stopline41_end_x");
CString CParamini::g_str_stopline41_end_y = _T("stopline41_end_y");

CString CParamini::g_str_stopline42_start_x = _T("stopline42_start_x");
CString CParamini::g_str_stopline42_start_y= _T("stopline42_start_y");
CString CParamini::g_str_stopline42_end_x = _T("stopline42_end_x");
CString CParamini::g_str_stopline42_end_y = _T("stopline42_end_y");

CString CParamini::g_str_stopline43_start_x = _T("stopline43_start_x");
CString CParamini::g_str_stopline43_start_y= _T("stopline43_start_y");
CString CParamini::g_str_stopline43_end_x = _T("stopline43_end_x");
CString CParamini::g_str_stopline43_end_y = _T("stopline43_end_y");

CString CParamini::g_str_stopline44_start_x = _T("stopline44_start_x");
CString CParamini::g_str_stopline44_start_y= _T("stopline44_start_y");
CString CParamini::g_str_stopline44_end_x = _T("stopline44_end_x");
CString CParamini::g_str_stopline44_end_y = _T("stopline44_end_y");

CString CParamini::g_str_stopline45_start_x = _T("stopline45_start_x");
CString CParamini::g_str_stopline45_start_y= _T("stopline45_start_y");
CString CParamini::g_str_stopline45_end_x = _T("stopline45_end_x");
CString CParamini::g_str_stopline45_end_y = _T("stopline45_end_y");

CString CParamini::g_str_stopline46_start_x = _T("stopline46_start_x");
CString CParamini::g_str_stopline46_start_y= _T("stopline46_start_y");
CString CParamini::g_str_stopline46_end_x = _T("stopline46_end_x");
CString CParamini::g_str_stopline46_end_y = _T("stopline46_end_y");

CString CParamini::g_str_stopline51_start_x = _T("stopline51_start_x");
CString CParamini::g_str_stopline51_start_y= _T("stopline51_start_y");
CString CParamini::g_str_stopline51_end_x = _T("stopline51_end_x");
CString CParamini::g_str_stopline51_end_y = _T("stopline51_end_y");

CString CParamini::g_str_stopline52_start_x = _T("stopline52_start_x");
CString CParamini::g_str_stopline52_start_y= _T("stopline52_start_y");
CString CParamini::g_str_stopline52_end_x = _T("stopline52_end_x");
CString CParamini::g_str_stopline52_end_y = _T("stopline52_end_y");

CString CParamini::g_str_stopline53_start_x = _T("stopline53_start_x");
CString CParamini::g_str_stopline53_start_y= _T("stopline53_start_y");
CString CParamini::g_str_stopline53_end_x = _T("stopline53_end_x");
CString CParamini::g_str_stopline53_end_y = _T("stopline53_end_y");

CString CParamini::g_str_stopline54_start_x = _T("stopline54_start_x");
CString CParamini::g_str_stopline54_start_y= _T("stopline54_start_y");
CString CParamini::g_str_stopline54_end_x = _T("stopline54_end_x");
CString CParamini::g_str_stopline54_end_y = _T("stopline54_end_y");

CString CParamini::g_str_stopline55_start_x = _T("stopline55_start_x");
CString CParamini::g_str_stopline55_start_y= _T("stopline55_start_y");
CString CParamini::g_str_stopline55_end_x = _T("stopline55_end_x");
CString CParamini::g_str_stopline55_end_y = _T("stopline55_end_y");

CString CParamini::g_str_stopline56_start_x = _T("stopline56_start_x");
CString CParamini::g_str_stopline56_start_y= _T("stopline56_start_y");
CString CParamini::g_str_stopline56_end_x = _T("stopline56_end_x");
CString CParamini::g_str_stopline56_end_y = _T("stopline56_end_y");

CString CParamini::g_strSection_PlaneType = _T("PlaneType");
CString CParamini::g_str_planetype11_1 = _T("planetype11_1");
CString CParamini::g_str_planetype11_2 = _T("planetype11_2");
CString CParamini::g_str_planetype11_3 = _T("planetype11_3");
CString CParamini::g_str_planetype11_4 = _T("planetype11_4");
CString CParamini::g_str_planetype11_5 = _T("planetype11_5");
CString CParamini::g_str_planetype11_6 = _T("planetype11_6");
CString CParamini::g_str_planetype11_7 = _T("planetype11_7");
CString CParamini::g_str_planetype11_8 = _T("planetype11_8");
CString CParamini::g_str_planetype11_9 = _T("planetype11_9");
CString CParamini::g_str_planetype11_10 = _T("planetype11_10");
CString CParamini::g_str_planetype11_11 = _T("planetype11_11");
CString CParamini::g_str_planetype11_12 = _T("planetype11_12");
CString CParamini::g_str_planetype11_13 = _T("planetype11_13");
CString CParamini::g_str_planetype11_14 = _T("planetype11_14");
CString CParamini::g_str_planetype11_15 = _T("planetype11_15");
CString CParamini::g_str_planetype11_16 = _T("planetype11_16");

CString CParamini::g_str_planetype12_1 = _T("planetype12_1");
CString CParamini::g_str_planetype12_2 = _T("planetype12_2");
CString CParamini::g_str_planetype12_3 = _T("planetype12_3");
CString CParamini::g_str_planetype12_4 = _T("planetype12_4");
CString CParamini::g_str_planetype12_5 = _T("planetype12_5");
CString CParamini::g_str_planetype12_6 = _T("planetype12_6");
CString CParamini::g_str_planetype12_7 = _T("planetype12_7");
CString CParamini::g_str_planetype12_8 = _T("planetype12_8");
CString CParamini::g_str_planetype12_9 = _T("planetype12_9");
CString CParamini::g_str_planetype12_10 = _T("planetype12_10");
CString CParamini::g_str_planetype12_11 = _T("planetype12_11");
CString CParamini::g_str_planetype12_12 = _T("planetype12_12");
CString CParamini::g_str_planetype12_13 = _T("planetype12_13");
CString CParamini::g_str_planetype12_14 = _T("planetype12_14");
CString CParamini::g_str_planetype12_15 = _T("planetype12_15");
CString CParamini::g_str_planetype12_16 = _T("planetype12_16");

CString CParamini::g_str_planetype13_1 = _T("planetype13_1");
CString CParamini::g_str_planetype13_2 = _T("planetype13_2");
CString CParamini::g_str_planetype13_3 = _T("planetype13_3");
CString CParamini::g_str_planetype13_4 = _T("planetype13_4");
CString CParamini::g_str_planetype13_5 = _T("planetype13_5");
CString CParamini::g_str_planetype13_6 = _T("planetype13_6");
CString CParamini::g_str_planetype13_7 = _T("planetype13_7");
CString CParamini::g_str_planetype13_8 = _T("planetype13_8");
CString CParamini::g_str_planetype13_9 = _T("planetype13_9");
CString CParamini::g_str_planetype13_10 = _T("planetype13_10");
CString CParamini::g_str_planetype13_11 = _T("planetype13_11");
CString CParamini::g_str_planetype13_12 = _T("planetype13_12");
CString CParamini::g_str_planetype13_13 = _T("planetype13_13");
CString CParamini::g_str_planetype13_14 = _T("planetype13_14");
CString CParamini::g_str_planetype13_15 = _T("planetype13_15");
CString CParamini::g_str_planetype13_16 = _T("planetype13_16");

CString CParamini::g_str_planetype21_1 = _T("planetype21_1");
CString CParamini::g_str_planetype21_2 = _T("planetype21_2");
CString CParamini::g_str_planetype21_3 = _T("planetype21_3");
CString CParamini::g_str_planetype21_4 = _T("planetype21_4");
CString CParamini::g_str_planetype21_5 = _T("planetype21_5");
CString CParamini::g_str_planetype21_6 = _T("planetype21_6");
CString CParamini::g_str_planetype21_7 = _T("planetype21_7");
CString CParamini::g_str_planetype21_8 = _T("planetype21_8");
CString CParamini::g_str_planetype21_9 = _T("planetype21_9");
CString CParamini::g_str_planetype21_10 = _T("planetype21_10");
CString CParamini::g_str_planetype21_11 = _T("planetype21_11");
CString CParamini::g_str_planetype21_12 = _T("planetype21_12");
CString CParamini::g_str_planetype21_13 = _T("planetype21_13");
CString CParamini::g_str_planetype21_14 = _T("planetype21_14");
CString CParamini::g_str_planetype21_15 = _T("planetype21_15");
CString CParamini::g_str_planetype21_16 = _T("planetype21_16");

CString CParamini::g_str_planetype22_1 = _T("planetype22_1");
CString CParamini::g_str_planetype22_2 = _T("planetype22_2");
CString CParamini::g_str_planetype22_3 = _T("planetype22_3");
CString CParamini::g_str_planetype22_4 = _T("planetype22_4");
CString CParamini::g_str_planetype22_5 = _T("planetype22_5");
CString CParamini::g_str_planetype22_6 = _T("planetype22_6");
CString CParamini::g_str_planetype22_7 = _T("planetype22_7");
CString CParamini::g_str_planetype22_8 = _T("planetype22_8");
CString CParamini::g_str_planetype22_9 = _T("planetype22_9");
CString CParamini::g_str_planetype22_10 = _T("planetype22_10");
CString CParamini::g_str_planetype22_11 = _T("planetype22_11");
CString CParamini::g_str_planetype22_12 = _T("planetype22_12");
CString CParamini::g_str_planetype22_13 = _T("planetype22_13");
CString CParamini::g_str_planetype22_14 = _T("planetype22_14");
CString CParamini::g_str_planetype22_15 = _T("planetype22_15");
CString CParamini::g_str_planetype22_16 = _T("planetype22_16");

CString CParamini::g_str_planetype23_1 = _T("planetype23_1");
CString CParamini::g_str_planetype23_2 = _T("planetype23_2");
CString CParamini::g_str_planetype23_3 = _T("planetype23_3");
CString CParamini::g_str_planetype23_4 = _T("planetype23_4");
CString CParamini::g_str_planetype23_5 = _T("planetype23_5");
CString CParamini::g_str_planetype23_6 = _T("planetype23_6");
CString CParamini::g_str_planetype23_7 = _T("planetype23_7");
CString CParamini::g_str_planetype23_8 = _T("planetype23_8");
CString CParamini::g_str_planetype23_9 = _T("planetype23_9");
CString CParamini::g_str_planetype23_10 = _T("planetype23_10");
CString CParamini::g_str_planetype23_11 = _T("planetype23_11");
CString CParamini::g_str_planetype23_12 = _T("planetype23_12");
CString CParamini::g_str_planetype23_13 = _T("planetype23_13");
CString CParamini::g_str_planetype23_14 = _T("planetype23_14");
CString CParamini::g_str_planetype23_15 = _T("planetype23_15");
CString CParamini::g_str_planetype23_16 = _T("planetype23_16");

CString CParamini::g_strSection_Point = _T("MarkPoint");
CString CParamini::g_str_point1_x = _T("point1_x");
CString CParamini::g_str_point1_y = _T("point1_y");
CString CParamini::g_str_point2_x = _T("point2_x");
CString CParamini::g_str_point2_y = _T("point2_y");
CString CParamini::g_str_point3_x = _T("point3_x");
CString CParamini::g_str_point3_y = _T("point3_y");
CString CParamini::g_str_point4_x = _T("point4_x");
CString CParamini::g_str_point4_y = _T("point4_y");
CString CParamini::g_str_point5_x = _T("point5_x");
CString CParamini::g_str_point5_y = _T("point5_y");
CString CParamini::g_str_point6_x = _T("point6_x");
CString CParamini::g_str_point6_y = _T("point6_y");
CString CParamini::g_str_point7_x = _T("point7_x");
CString CParamini::g_str_point7_y = _T("point7_y");
CString CParamini::g_str_point8_x = _T("point8_x");
CString CParamini::g_str_point8_y = _T("point8_y");
CString CParamini::g_str_point9_x = _T("point9_x");
CString CParamini::g_str_point9_y = _T("point9_y");
CString CParamini::g_str_point10_x = _T("point10_x");
CString CParamini::g_str_point10_y = _T("point10_y");
CString CParamini::g_str_point11_x = _T("point11_x");
CString CParamini::g_str_point11_y = _T("point11_y");
CString CParamini::g_str_point12_x = _T("point12_x");
CString CParamini::g_str_point12_y = _T("point12_y");
CString CParamini::g_str_point13_x = _T("point13_x");
CString CParamini::g_str_point13_y = _T("point13_y");
CString CParamini::g_str_point14_x = _T("point14_x");
CString CParamini::g_str_point14_y = _T("point14_y");
CString CParamini::g_str_point15_x = _T("point15_x");
CString CParamini::g_str_point15_y = _T("point15_y");
CString CParamini::g_str_point16_x = _T("point16_x");
CString CParamini::g_str_point16_y = _T("point16_y");
CString CParamini::g_str_point17_x = _T("point17_x");
CString CParamini::g_str_point17_y = _T("point17_y");
CString CParamini::g_str_point18_x = _T("point18_x");
CString CParamini::g_str_point18_y = _T("point18_y");
CString CParamini::g_str_point19_x = _T("point19_x");
CString CParamini::g_str_point19_y = _T("point19_y");
CString CParamini::g_str_point20_x = _T("point20_x");
CString CParamini::g_str_point20_y = _T("point20_y");

CString CParamini::g_strSection_StopDistance = _T("StopDistance");
CString CParamini::g_str_stopdistance= _T("stopdistance");
CString CParamini::g_strSection_ToofarDistance = _T("ToofarDistance");
CString CParamini::g_str_toofardistance=_T("toofardistance");
CString CParamini::g_strSection_Speed = _T("Speed");
CString CParamini::g_str_speed= _T("speed");
CString CParamini::g_strSection_PCTime= _T("PCTime");
CString CParamini::g_str_pctime= _T("pctime");


CParamini::CParamini(void)
{
}

CParamini::~CParamini(void)
{
}

void CParamini::Init(CString filename)
{
	CString strFile = ::GetAppPath(AfxGetApp()->m_hInstance);
	strFile +=filename;

	SetPathName(strFile);

}