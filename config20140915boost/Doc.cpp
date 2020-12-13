#include "stdafx.h"
#include "opencv2/opencv.hpp"
#include <stdio.h>   
#include <iostream>
#include <string>
//#include <ctype.h>
#include <afx.h>

using namespace cv;
using namespace std; 

#include "Doc.h"
#include "math.h"


Document::Document()
{	

}

Document::~Document(void)
{

}

//写数据，某些单数据
void Document::writedata(string routename,string name,float alonex)
{
	m_allnodedata alldata = readalldata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;
	VLaserParameters = alldata.VLPs;
	V_M_Datas = alldata.V_M_datas;
	V_S_Datas = alldata.V_S_datas;
	L_M_Datas = alldata.L_M_datas;

	ValoneXs(name,alonex);

	writealldata(routename);
	
	//int iter;
	//int it;
	//FileStorage fsw(routename, FileStorage::WRITE);

	//fsw << "VDisandPointslist"<<"[:";
	//for (iter = 0; iter< VorientationVPs.end()-VorientationVPs.begin(); iter++ )
	//{
	//	fsw<<"{"<<"Vectorname"<<VorientationVPs.at(iter).point_name<<"points"<<"[";
	//	for (it = 0; it< VorientationVPs.at(iter).VLinePoints.end() - VorientationVPs.at(iter).VLinePoints.begin(); it++)
	//	{
	//		fsw <<"{"<<"distance"<<VorientationVPs.at(iter).VLinePoints.at(it).first
	//			<<"x" <<VorientationVPs.at(iter).VLinePoints.at(it).second.x
	//			<< "y" << VorientationVPs.at(iter).VLinePoints.at(it).second.y<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";

	//fsw << "Vstringslist"<<"[:";
	//for (iter = 0; iter< VsceneVSs.end()-VsceneVSs.begin(); iter++ )
	//{
	//	stringstream str;
	//	string vPointnum;
	//	str<<iter;
	//	str>>vPointnum;

	//	fsw<<"{"<<"Vectorname"<<VsceneVSs.at(iter).m_Vsname<<"strings"<<"[";
	//	for (int it = 0; it< VsceneVSs.at(iter).Vstrings.end() - VsceneVSs.at(iter).Vstrings.begin(); it++)
	//	{
	//		fsw <<"{"<< "stringname" <<VsceneVSs.at(iter).Vstrings.at(it)<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";

	//fsw << "Vpointslist"<<"[:";
	//for (iter = 0; iter< VsceneVPs.end()-VsceneVPs.begin(); iter++ )
	//{
	//	stringstream str;
	//	string vPointnum;
	//	str<<iter;
	//	str>>vPointnum;

	//	fsw<<"{"<<"Vectorname"<<VsceneVPs.at(iter).m_Vpname<<"points"<<"[";
	//	//fsw<<"{"<<VsceneVPs.at(iter).m_Vpname<<"{";
	//	//fsw<<"{";
	//	for (it = 0; it< VsceneVPs.at(iter).Vpoints.end() - VsceneVPs.at(iter).Vpoints.begin(); it++)
	//	{
	//		//int f = Vpoints.end()- Vpoints.begin();
	//		stringstream str;
	//		string Pointnum;
	//		str<<it;
	//		str>>Pointnum;
	//		//fsw << Pointnum <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//		fsw <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//		//fsw <<"{"<< "pointnum"<< it+1<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";


	//fsw <<"alonexlist"<< "[:";
	//for (iter= 0; iter < VsceneXs.end()-VsceneXs.begin();iter++)//将数据全部写入文档
	//{

	//	fsw <<"{"<< "name" <<VsceneXs.at(iter).m_name<< "x" << VsceneXs.at(iter).m_x<< "}";

	//}
	//fsw << "]";

	//fsw <<"rectlist"<< "[:";
	//for (iter= 0; iter < VsceneRs.end()-VsceneRs.begin();iter++)//将矩形框数据全部写入文档
	//{

	//	fsw <<"{"<< "name" <<VsceneRs.at(iter).rect_name  << "x" << VsceneRs.at(iter).m_r.x<< "y" << VsceneRs.at(iter).m_r.y
	//		<<"height"<<VsceneRs.at(iter).m_r.height<<"width"<<VsceneRs.at(iter).m_r.width<< "}";

	//}
	//fsw << "]";

	//fsw <<"pointlist"<< "[:";
	//for (iter= 0; iter < VscenePs.end()-VscenePs.begin();iter++)//将点数据全部写入文档
	//{
	//	//contiwrite(routename,VscenePs.at(iter).point_name,VscenePs.at(iter));
	//	//fsw<<VscenePs.at(iter).point_name;
	//	fsw <<"{"<<"name" <<VscenePs.at(iter).point_name 
	//		<< "x" << VscenePs.at(iter).m_p.x<< "y" << VscenePs.at(iter).m_p.y << "}";

	//}
	//fsw << "]";

	//fsw.release();
}

float Document::readalonexdata(string routename,string name)//读数据
{
	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	float x;
	FileStorage fs;
	try
	{	
		fs.open(routename,0);//read
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		FileStorage fcopys;
		fcopys.open(newroutename,0);
		FileNode xt= fcopys["alonexlist"];
		int xiter;
		for (xiter= 0; xiter < xt.end()-xt.begin();xiter++)
		{
			string xname = (string)xt[xiter]["name"];
			if (name == xname)
			{
				x = (float)xt[xiter]["x"];
				break;
			}		
		}
		if (xiter == xt.end()-xt.begin()||xiter ==0)
		{
			//p = Point(0,0);
			cout<<"no this xdata!"<<endl;
			x = 0.0;
		}
		fcopys.release();
		return x;
	}
	FileNode xt= fs["alonexlist"];
	int xiter;
	for (xiter= 0; xiter < xt.end()-xt.begin();xiter++)
	{
		string xname = (string)xt[xiter]["name"];
		if (name == xname)
		{
			x = (float)xt[xiter]["x"];
			break;
		}		
	}
	if (xiter == xt.end()-xt.begin())
	{
		//p = Point(0,0);
		cout<<"no this xdata!"<<endl;
		x = 0.0;
	}
	fs.release();
	return x;
}

void Document::ValoneXs(string name,float alonex)
{
	vector<m_alonex>::iterator it = VsceneXs.begin();
	for ( ; it!= VsceneXs.end(); it++)
	{
		string xname =  VsceneXs.at(it-VsceneXs.begin()).m_name;
		if (xname == name)//如果有重复的节点，则修改其数据，然后全部数据重新写入文档中
		{
			VsceneXs.at(it-VsceneXs.begin()).m_x = alonex;
			//VscenePs.erase(it);
			break;
		}
	}

	if (it == VsceneXs.end())//如果不是重复的点则存入容器VscenePs,然后全部数据重新写入文档中
	{
		m_alonex newx(name,alonex);
		VsceneXs.push_back(newx);
	}
}

//写点数据
void Document::writedata(string routename,string pointname,Point2d m_p)
{
	m_allnodedata alldata = readalldata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;
	VLaserParameters = alldata.VLPs;
	V_M_Datas = alldata.V_M_datas;
	V_S_Datas = alldata.V_S_datas;
	L_M_Datas = alldata.L_M_datas;
	
	Vpoints(pointname,m_p);

	writealldata(routename);

	//FileStorage fsw(routename, FileStorage::WRITE);

	//int iter;
	//int it;

	//fsw << "VDisandPointslist"<<"[:";
	//for (iter = 0; iter< VorientationVPs.end()-VorientationVPs.begin(); iter++ )
	//{
	//	fsw<<"{"<<"Vectorname"<<VorientationVPs.at(iter).point_name<<"points"<<"[";
	//	for (it = 0; it< VorientationVPs.at(iter).VLinePoints.end() - VorientationVPs.at(iter).VLinePoints.begin(); it++)
	//	{
	//		fsw <<"{"<<"distance"<<VorientationVPs.at(iter).VLinePoints.at(it).first
	//			<<"x" <<VorientationVPs.at(iter).VLinePoints.at(it).second.x
	//			<< "y" << VorientationVPs.at(iter).VLinePoints.at(it).second.y<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";

	//fsw << "Vstringslist"<<"[:";
	//for (iter = 0; iter< VsceneVSs.end()-VsceneVSs.begin(); iter++ )
	//{
	//	stringstream str;
	//	string vPointnum;
	//	str<<iter;
	//	str>>vPointnum;

	//	fsw<<"{"<<"Vectorname"<<VsceneVSs.at(iter).m_Vsname<<"strings"<<"[";
	//	for (int it = 0; it< VsceneVSs.at(iter).Vstrings.end() - VsceneVSs.at(iter).Vstrings.begin(); it++)
	//	{
	//		fsw <<"{"<< "stringname" <<VsceneVSs.at(iter).Vstrings.at(it)<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";

	//fsw << "Vpointslist"<<"[:";
	//for (iter = 0; iter< VsceneVPs.end()-VsceneVPs.begin(); iter++ )
	//{
	//	stringstream str;
	//	string vPointnum;
	//	str<<iter;
	//	str>>vPointnum;

	//	fsw<<"{"<<"Vectorname"<<VsceneVPs.at(iter).m_Vpname<<"points"<<"[";
	//	//fsw<<"{"<<VsceneVPs.at(iter).m_Vpname<<"{";
	//	//fsw<<"{";
	//	for (it = 0; it< VsceneVPs.at(iter).Vpoints.end() - VsceneVPs.at(iter).Vpoints.begin(); it++)
	//	{
	//		//int f = Vpoints.end()- Vpoints.begin();
	//		stringstream str;
	//		string Pointnum;
	//		str<<it;
	//		str>>Pointnum;
	//		//fsw << Pointnum <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//		fsw <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//		//fsw <<"{"<< "pointnum"<< it+1<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";


	//fsw <<"alonexlist"<< "[:";
	//for (iter= 0; iter < VsceneXs.end()-VsceneXs.begin();iter++)//将数据全部写入文档
	//{

	//	fsw <<"{"<< "name" <<VsceneXs.at(iter).m_name<< "x" << VsceneXs.at(iter).m_x<< "}";

	//}
	//fsw << "]";

	//fsw <<"rectlist"<< "[:";
	//for (iter= 0; iter < VsceneRs.end()-VsceneRs.begin();iter++)//将矩形框数据全部写入文档
	//{

	//	fsw <<"{"<< "name" <<VsceneRs.at(iter).rect_name  << "x" << VsceneRs.at(iter).m_r.x<< "y" << VsceneRs.at(iter).m_r.y
	//		<<"height"<<VsceneRs.at(iter).m_r.height<<"width"<<VsceneRs.at(iter).m_r.width<< "}";

	//}
	//fsw << "]";

	//fsw <<"pointlist"<< "[:";
	//for (iter= 0; iter < VscenePs.end()-VscenePs.begin();iter++)//将点数据全部写入文档
	//{
	//	//contiwrite(routename,VscenePs.at(iter).point_name,VscenePs.at(iter));
	//	//fsw<<VscenePs.at(iter).point_name;
	//	fsw <<"{"<<"name" <<VscenePs.at(iter).point_name 
	//		<< "x" << VscenePs.at(iter).m_p.x<< "y" << VscenePs.at(iter).m_p.y << "}";

	//}
	//fsw << "]";

	//fsw.release();

}

//检查点向量
void Document::Vpoints(string pointname,Point2d p)
{
	vector<m_point>::iterator it = VscenePs.begin();
	for ( ; it!= VscenePs.end(); it++)
	{
		string name =  VscenePs.at(it-VscenePs.begin()).point_name;
		if (pointname == name)//如果有重复的节点，则修改其数据，然后全部数据重新写入文档中
		{
			VscenePs.at(it-VscenePs.begin()).m_p = p;
			//VscenePs.erase(it);
			break;
		}
	}

	if (it == VscenePs.end())//如果不是重复的点则存入容器VscenePs,然后全部数据重新写入文档中
	{
		m_point newpoint(pointname,p);
		VscenePs.push_back(newpoint);
	}
	
}

//写矩形框数据
void Document::writedata(string routename,string rectname,Rect m_rect)
{
	m_allnodedata alldata = readalldata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;
	VLaserParameters = alldata.VLPs;
	V_M_Datas = alldata.V_M_datas;
	V_S_Datas = alldata.V_S_datas;
	L_M_Datas = alldata.L_M_datas;
	
	Vrects(rectname,m_rect);

	writealldata(routename);
	
	//FileStorage fsw(routename, FileStorage::WRITE);

	//int iter;
	//int it;

	//fsw << "VDisandPointslist"<<"[:";
	//for (iter = 0; iter< VorientationVPs.end()-VorientationVPs.begin(); iter++ )
	//{
	//	fsw<<"{"<<"Vectorname"<<VorientationVPs.at(iter).point_name<<"points"<<"[";
	//	for (it = 0; it< VorientationVPs.at(iter).VLinePoints.end() - VorientationVPs.at(iter).VLinePoints.begin(); it++)
	//	{
	//		fsw <<"{"<<"distance"<<VorientationVPs.at(iter).VLinePoints.at(it).first
	//			<<"x" <<VorientationVPs.at(iter).VLinePoints.at(it).second.x
	//			<< "y" << VorientationVPs.at(iter).VLinePoints.at(it).second.y<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";

	//fsw << "Vstringslist"<<"[:";
	//for (iter = 0; iter< VsceneVSs.end()-VsceneVSs.begin(); iter++ )
	//{
	//	stringstream str;
	//	string vPointnum;
	//	str<<iter;
	//	str>>vPointnum;

	//	fsw<<"{"<<"Vectorname"<<VsceneVSs.at(iter).m_Vsname<<"strings"<<"[";
	//	for (it = 0; it< VsceneVSs.at(iter).Vstrings.end() - VsceneVSs.at(iter).Vstrings.begin(); it++)
	//	{
	//		fsw <<"{"<< "stringname" <<VsceneVSs.at(iter).Vstrings.at(it)<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";

	//fsw << "Vpointslist"<<"[:";
	//for (iter = 0; iter< VsceneVPs.end()-VsceneVPs.begin(); iter++ )
	//{
	//	stringstream str;
	//	string vPointnum;
	//	str<<iter;
	//	str>>vPointnum;

	//	fsw<<"{"<<"Vectorname"<<VsceneVPs.at(iter).m_Vpname<<"points"<<"[";
	//	//fsw<<"{"<<VsceneVPs.at(iter).m_Vpname<<"{";
	//	//fsw<<"{";
	//	for (it = 0; it< VsceneVPs.at(iter).Vpoints.end() - VsceneVPs.at(iter).Vpoints.begin(); it++)
	//	{
	//		//int f = Vpoints.end()- Vpoints.begin();
	//		stringstream str;
	//		string Pointnum;
	//		str<<it;
	//		str>>Pointnum;
	//		//fsw << Pointnum <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//		fsw <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//		//fsw <<"{"<< "pointnum"<< it+1<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";


	//fsw <<"alonexlist"<< "[:";
	//for (iter= 0; iter < VsceneXs.end()-VsceneXs.begin();iter++)//将数据全部写入文档
	//{

	//	fsw <<"{"<< "name" <<VsceneXs.at(iter).m_name<< "x" << VsceneXs.at(iter).m_x<< "}";

	//}
	//fsw << "]";

	//fsw <<"rectlist"<< "[:";
	//for (iter= 0; iter < VsceneRs.end()-VsceneRs.begin();iter++)//将矩形框数据全部写入文档
	//{

	//	fsw <<"{"<< "name" <<VsceneRs.at(iter).rect_name  << "x" << VsceneRs.at(iter).m_r.x<< "y" << VsceneRs.at(iter).m_r.y
	//		<<"height"<<VsceneRs.at(iter).m_r.height<<"width"<<VsceneRs.at(iter).m_r.width<< "}";

	//}
	//fsw << "]";

	//fsw <<"pointlist"<< "[:";
	//for (iter= 0; iter < VscenePs.end()-VscenePs.begin();iter++)//将点数据全部写入文档
	//{
	//	//contiwrite(routename,VscenePs.at(iter).point_name,VscenePs.at(iter));
	//	//fsw<<VscenePs.at(iter).point_name;
	//	fsw <<"{"<<"name" <<VscenePs.at(iter).point_name 
	//		<< "x" << VscenePs.at(iter).m_p.x<< "y" << VscenePs.at(iter).m_p.y << "}";

	//}
	//fsw << "]";

	//fsw.release();

}

//检查矩形向量
void Document::Vrects(string rectname,Rect r)
{
	vector<m_rect>::iterator it = VsceneRs.begin();
	for ( ; it!= VsceneRs.end(); it++)
	{
		string name =  VsceneRs.at(it-VsceneRs.begin()).rect_name;
		if (rectname == name)//如果有重复的节点，则修改其数据，然后全部数据重新写入文档中
		{
			VsceneRs.at(it-VsceneRs.begin()).m_r= r;
			//VscenePs.erase(it);
			break;
		}
	}

	if (it == VsceneRs.end())//如果不是重复的点则存入容器VscenePs,然后全部数据重新写入文档中
	{
		m_rect rect(rectname,r);
		VsceneRs.push_back(rect);
	}
}

//读某个点数据
Point2d Document::readpointdata(string routename,string pointname)
{
	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	Point p;
	FileStorage fs;
	try
	{	
		fs.open(routename,0);
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		FileStorage fcopy;
		fcopy.open(newroutename,0);
		FileNode pt= fcopy["pointlist"];
		int piter;
		for (piter= 0; piter < pt.end()-pt.begin();piter++)
		{
			string name = (string)pt[piter]["name"];
			if (name == pointname)
			{
				int x = (int)pt[piter]["x"];
				int y = (int)pt[piter]["y"];
				p.x = x;
				p.y = y;
				break;
			}		
		}
		if (piter == pt.end()-pt.begin())
		{
			//p = Point(0,0);
			cout<<"no this pointdata!"<<endl;
		}
		fcopy.release();
		return p;
	}

	//FileStorage fs(routename, FileStorage::READ);
	FileNode pt= fs["pointlist"];
	int piter;
	for (piter= 0; piter < pt.end()-pt.begin();piter++)
	{
		string name = (string)pt[piter]["name"];
		if (name == pointname)
		{
			int x = (int)pt[piter]["x"];
			int y = (int)pt[piter]["y"];
			p.x = x;
			p.y = y;
			break;
		}		
	}
	if (piter == pt.end()-pt.begin())
	{
		//p = Point(0,0);
		cout<<"no this pointdata!"<<endl;
	}
	fs.release();
	return p;
}

//读某个矩形框数据
Rect Document::readrectdata(string routename,string rectname)
{
	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	Rect r;
	FileStorage fs;
	try
	{	
		fs.open(routename,0);//read
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		FileStorage fcopys;
		fcopys.open(newroutename,0);
		FileNode rt = fcopys["rectlist"];	
		int riter;
		//FileNodeIterator it = t.begin(),it_end = t.end();
		for (riter= 0; riter < rt.end()-rt.begin();riter++)
		{
			string name = (string)rt[riter]["name"];
			if (name == rectname)
			{
				int x = (int)rt[riter]["x"];
				int y = (int)rt[riter]["y"];
				int height = (int)rt[riter]["height"];
				int width = (int)rt[riter]["width"];

				r = Rect(x,y,width,height);
				break;
			}		
		}

		if (riter == rt.end()-rt.begin())
		{
			cout<<"no this rectdata!"<<endl;
		}	
		fcopys.release();
		return r;
	}
	FileNode rt = fs["rectlist"];	
	int riter;
	//FileNodeIterator it = t.begin(),it_end = t.end();
	for (riter= 0; riter < rt.end()-rt.begin();riter++)
	{
		string name = (string)rt[riter]["name"];
		if (name == rectname)
		{
			int x = (int)rt[riter]["x"];
			int y = (int)rt[riter]["y"];
			int height = (int)rt[riter]["height"];
			int width = (int)rt[riter]["width"];

			r = Rect(x,y,width,height);
			break;
		}		
	}

	if (riter == rt.end()-rt.begin())
	{
		cout<<"no this rectdata!"<<endl;
	}	
	fs.release();
	return r;
}

//写点向量数据
void Document::writedata(string routename,string Vpointsname,vector<Point2d> Vpoints)
{
	m_allnodedata alldata = readalldata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;
	VLaserParameters = alldata.VLPs;
	V_M_Datas = alldata.V_M_datas;
	V_S_Datas = alldata.V_S_datas;
	L_M_Datas = alldata.L_M_datas;
	
	VVPs(Vpointsname, Vpoints);

	writealldata(routename);
	
	//int iter;
	//int it;
	//FileStorage fsw(routename, FileStorage::WRITE);

	//fsw << "VDisandPointslist"<<"[:";
	//for (iter = 0; iter< VorientationVPs.end()-VorientationVPs.begin(); iter++ )
	//{
	//	fsw<<"{"<<"Vectorname"<<VorientationVPs.at(iter).point_name<<"points"<<"[";
	//	for (it = 0; it< VorientationVPs.at(iter).VLinePoints.end() - VorientationVPs.at(iter).VLinePoints.begin(); it++)
	//	{
	//		fsw <<"{"<<"distance"<<VorientationVPs.at(iter).VLinePoints.at(it).first
	//			<<"x" <<VorientationVPs.at(iter).VLinePoints.at(it).second.x
	//			<< "y" << VorientationVPs.at(iter).VLinePoints.at(it).second.y<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";

	///*int i = Vpoints.end()- Vpoints.begin();
	//int j = VsceneVPs.end()-VsceneVPs.begin();*/
	////writepoints(fsw);

	//fsw << "Vstringslist"<<"[:";
	//for (iter = 0; iter< VsceneVSs.end()-VsceneVSs.begin(); iter++ )
	//{
	//	stringstream str;
	//	string vPointnum;
	//	str<<iter;
	//	str>>vPointnum;

	//	fsw<<"{"<<"Vectorname"<<VsceneVSs.at(iter).m_Vsname<<"strings"<<"[";
	//	for (it = 0; it< VsceneVSs.at(iter).Vstrings.end() - VsceneVSs.at(iter).Vstrings.begin(); it++)
	//	{
	//		fsw <<"{"<< "stringname" <<VsceneVSs.at(iter).Vstrings.at(it)<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";

	//fsw << "Vpointslist"<<"[:";
	//for (iter = 0; iter< VsceneVPs.end()-VsceneVPs.begin(); iter++ )
	//{
	//	stringstream str;
	//	string vPointnum;
	//	str<<iter;
	//	str>>vPointnum;

	//	fsw<<"{"<<"Vectorname"<<VsceneVPs.at(iter).m_Vpname<<"points"<<"[";
	//	//fsw<<"{"<<VsceneVPs.at(iter).m_Vpname<<"{";
	//	//fsw<<"{";
	//	for (it = 0; it< VsceneVPs.at(iter).Vpoints.end() - VsceneVPs.at(iter).Vpoints.begin(); it++)
	//	{
	//		//int f = Vpoints.end()- Vpoints.begin();
	//		stringstream str;
	//		string Pointnum;
	//		str<<it;
	//		str>>Pointnum;
	//		//fsw << Pointnum <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//		fsw <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//		//fsw <<"{"<< "pointnum"<< it+1<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";


	//fsw <<"alonexlist"<< "[:";
	//for (iter= 0; iter < VsceneXs.end()-VsceneXs.begin();iter++)//将数据全部写入文档
	//{

	//	fsw <<"{"<< "name" <<VsceneXs.at(iter).m_name<< "x" << VsceneXs.at(iter).m_x<< "}";

	//}
	//fsw << "]";

	//fsw <<"rectlist"<< "[:";
	//for (iter= 0; iter < VsceneRs.end()-VsceneRs.begin();iter++)//将矩形框数据全部写入文档
	//{

	//	fsw <<"{"<< "name" <<VsceneRs.at(iter).rect_name  << "x" << VsceneRs.at(iter).m_r.x<< "y" << VsceneRs.at(iter).m_r.y
	//		<<"height"<<VsceneRs.at(iter).m_r.height<<"width"<<VsceneRs.at(iter).m_r.width<< "}";

	//}
	//fsw << "]";

	//fsw <<"pointlist"<< "[:";
	//for (iter= 0; iter < VscenePs.end()-VscenePs.begin();iter++)//将点数据全部写入文档
	//{
	//	//contiwrite(routename,VscenePs.at(iter).point_name,VscenePs.at(iter));
	//	//fsw<<VscenePs.at(iter).point_name;
	//	fsw <<"{"<<"name" <<VscenePs.at(iter).point_name 
	//		<< "x" << VscenePs.at(iter).m_p.x<< "y" << VscenePs.at(iter).m_p.y << "}";

	//}
	//fsw << "]";
	//fsw.release();
}

void Document::VVPs(string vpname,vector<Point2d> Vvpoints)
{
	/*m_Vpoints newVpoints;
	newVpoints.m_Vpname = "ABC";
	Vvpoints.push_back(Point(2,3));
	newVpoints.Vpoints = Vvpoints;
	VsceneVPs.push_back(newVpoints);*/

	vector<m_Vpoints>::iterator it = VsceneVPs.begin();
	for (; it != VsceneVPs.end(); it++)
	{
		string name =  VsceneVPs.at(it-VsceneVPs.begin()).m_Vpname;
		if (vpname == name)//如果有重复的节点，则修改其数据，然后全部数据重新写入文档中
		{
			VsceneVPs.at(it-VsceneVPs.begin()).Vpoints = Vvpoints;
			//VscenePs.erase(it);
			break;
		}
	}

	if (it == VsceneVPs.end())//如果不是重复的点则存入容器VscenePs,然后全部数据重新写入文档中
	{
		m_Vpoints newVpoints;
		newVpoints.m_Vpname = vpname;
		newVpoints.Vpoints = Vvpoints;

		VsceneVPs.push_back(newVpoints);
	}

}

//读点向量数据
vector<Point2d> Document::readVPdata(string routename,string vpname)
{
	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	vector<Point2d> vp;
	FileStorage fs;
	try
	{	
		fs.open(routename,0);//read
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		FileStorage fcopy;
		fcopy.open(newroutename,0);
		FileNode pt= fcopy["Vpointslist"];
		int piter = pt.end()-pt.begin();
		for (piter= 0; piter < pt.end()-pt.begin();piter++)
		{
			string vname = (string)pt[piter]["Vectorname"];
			if (vname == vpname)
			{
				for (int it = 0; it <pt[piter]["points"].end()-pt[piter]["points"].begin(); it++)
				{
					double x = pt[piter]["points"][it]["x"];
					double y = pt[piter]["points"][it]["y"];
					Point2d Pxy = Point2d(x,y);
					vp.push_back(Pxy);
				}

				break;
			}		
		}
		if (piter == pt.end()-pt.begin())
		{
			cout<<"no this Vpoint data!"<<endl;
		}

		fcopy.release();
		return vp;
	}

	FileNode pt= fs["Vpointslist"];
	int piter = pt.end()-pt.begin();
	for (piter= 0; piter < pt.end()-pt.begin();piter++)
	{
		string vname = (string)pt[piter]["Vectorname"];
		if (vname == vpname)
		{
			for (int it = 0; it <pt[piter]["points"].end()-pt[piter]["points"].begin(); it++)
			{
				double x = pt[piter]["points"][it]["x"];
				double y = pt[piter]["points"][it]["y"];
				Point2d Pxy = Point2d(x,y);
				vp.push_back(Pxy);
			}

			break;
		}		
	}
	if (piter == pt.end()-pt.begin())
	{
		cout<<"no this Vpoint data!"<<endl;
	}

	fs.release();

	return vp;
}

//void writevpoints(FileStorage fsw)
//{
//	int iter;
//	fsw << "Vpointslist"<<"[:";
//		for (iter = 0; iter< VsceneVPs.end()-VsceneVPs.begin(); iter++ )
//		{
//			fsw<<"{"<<"Vectorname"<<VsceneVPs.at(iter).m_Vpname<<"points"<<"[";
//			for (it = 0; it< VsceneVPs.at(iter).Vpoints.end() - VsceneVPs.at(iter).Vpoints.begin(); it++)
//			{
//				fsw <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
//			}
//			fsw<<"]"<<"}";
//		}	
//	fsw << "]";
//
//}

//写string向量数据
void Document::writedata(string routename,string Vstringsname,vector<string>Vstrings)
{
	m_allnodedata alldata = readalldata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;
	VLaserParameters = alldata.VLPs;
	V_M_Datas = alldata.V_M_datas;
	V_S_Datas = alldata.V_S_datas;
	L_M_Datas = alldata.L_M_datas;
	
	VVstrings(Vstringsname, Vstrings);

	writealldata(routename);

	//int iter;
	//int it;
	//FileStorage fsw(routename, FileStorage::WRITE);

	//fsw << "VDisandPointslist"<<"[:";
	//for (iter = 0; iter< VorientationVPs.end()-VorientationVPs.begin(); iter++ )
	//{
	//	fsw<<"{"<<"Vectorname"<<VorientationVPs.at(iter).point_name<<"points"<<"[";
	//	for (it = 0; it< VorientationVPs.at(iter).VLinePoints.end() - VorientationVPs.at(iter).VLinePoints.begin(); it++)
	//	{
	//		fsw <<"{"<<"distance"<<VorientationVPs.at(iter).VLinePoints.at(it).first
	//			<<"x" <<VorientationVPs.at(iter).VLinePoints.at(it).second.x
	//			<< "y" << VorientationVPs.at(iter).VLinePoints.at(it).second.y<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";

	//fsw << "Vstringslist"<<"[:";
	//for (iter = 0; iter< VsceneVSs.end()-VsceneVSs.begin(); iter++ )
	//{
	//	stringstream str;
	//	string vPointnum;
	//	str<<iter;
	//	str>>vPointnum;

	//	fsw<<"{"<<"Vectorname"<<VsceneVSs.at(iter).m_Vsname<<"strings"<<"[";
	//	for (it = 0; it< VsceneVSs.at(iter).Vstrings.end() - VsceneVSs.at(iter).Vstrings.begin(); it++)
	//	{
	//		fsw <<"{"<< "stringname" <<VsceneVSs.at(iter).Vstrings.at(it)<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";

	//fsw << "Vpointslist"<<"[:";
	//for (iter = 0; iter< VsceneVPs.end()-VsceneVPs.begin(); iter++ )
	//{
	//	stringstream str;
	//	string vPointnum;
	//	str<<iter;
	//	str>>vPointnum;

	//	fsw<<"{"<<"Vectorname"<<VsceneVPs.at(iter).m_Vpname<<"points"<<"[";
	//	//fsw<<"{"<<VsceneVPs.at(iter).m_Vpname<<"{";
	//	//fsw<<"{";
	//	for (it = 0; it< VsceneVPs.at(iter).Vpoints.end() - VsceneVPs.at(iter).Vpoints.begin(); it++)
	//	{
	//		//int f = Vpoints.end()- Vpoints.begin();
	//		stringstream str;
	//		string Pointnum;
	//		str<<it;
	//		str>>Pointnum;
	//		//fsw << Pointnum <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//		fsw <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//		//fsw <<"{"<< "pointnum"<< it+1<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
	//	}
	//	fsw<<"]"<<"}";
	//}	
	//fsw << "]";


	//fsw <<"alonexlist"<< "[:";
	//for (iter= 0; iter < VsceneXs.end()-VsceneXs.begin();iter++)//将数据全部写入文档
	//{

	//	fsw <<"{"<< "name" <<VsceneXs.at(iter).m_name<< "x" << VsceneXs.at(iter).m_x<< "}";

	//}
	//fsw << "]";

	//fsw <<"rectlist"<< "[:";
	//for (iter= 0; iter < VsceneRs.end()-VsceneRs.begin();iter++)//将矩形框数据全部写入文档
	//{

	//	fsw <<"{"<< "name" <<VsceneRs.at(iter).rect_name  << "x" << VsceneRs.at(iter).m_r.x<< "y" << VsceneRs.at(iter).m_r.y
	//		<<"height"<<VsceneRs.at(iter).m_r.height<<"width"<<VsceneRs.at(iter).m_r.width<< "}";

	//}
	//fsw << "]";

	//fsw <<"pointlist"<< "[:";
	//for (iter= 0; iter < VscenePs.end()-VscenePs.begin();iter++)//将点数据全部写入文档
	//{
	//	//contiwrite(routename,VscenePs.at(iter).point_name,VscenePs.at(iter));
	//	//fsw<<VscenePs.at(iter).point_name;
	//	fsw <<"{"<<"name" <<VscenePs.at(iter).point_name 
	//		<< "x" << VscenePs.at(iter).m_p.x<< "y" << VscenePs.at(iter).m_p.y << "}";

	//}
	//fsw << "]";
	//fsw.release();
}

//检查向量
void Document::VVstrings(string vsname,vector<string>Vvstrings)
{
	vector<m_Vstrings>::iterator it = VsceneVSs.begin();
	for (; it != VsceneVSs.end(); it++)
	{
		string name =  VsceneVSs.at(it-VsceneVSs.begin()).m_Vsname;
		if (vsname == name)//如果有重复的节点，则修改其数据，然后全部数据重新写入文档中
		{
			VsceneVSs.at(it-VsceneVSs.begin()).Vstrings = Vvstrings;
			//VscenePs.erase(it);
			break;
		}
	}

	if (it == VsceneVSs.end())//如果不是重复的点则存入容器VsceneSs,然后全部数据重新写入文档中
	{
		m_Vstrings newVstrings;
		newVstrings.m_Vsname= vsname;
		newVstrings.Vstrings = Vvstrings;

		VsceneVSs.push_back(newVstrings);
	}

}

//读string向量数据
vector<string> Document::readVSdata(string routename,string vsname)
{
	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	vector<string> vs;
	FileStorage fs;
	try
	{	
		fs.open(routename,0);//read
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		FileStorage fcopy;
		fcopy.open(newroutename,0);

		FileNode st= fcopy["Vstringslist"];
		int siter = st.end()-st.begin();
		for (siter= 0; siter < st.end()-st.begin();siter++)
		{
			string vname = (string)st[siter]["Vectorname"];
			if (vname == vsname)
			{
				for (int it = 0; it <st[siter]["strings"].end()-st[siter]["strings"].begin(); it++)
				{
					string sname = st[siter]["strings"][it]["stringname"];
					vs.push_back(sname);
				}

				break;
			}		
		}
		if (siter == st.end()-st.begin())
		{
			//p = Point(0,0);
			cout<<"no this Vstring data!"<<endl;
		}

		fcopy.release();
		return vs;
	}

	FileNode st= fs["Vstringslist"];
	int siter = st.end()-st.begin();
	for (siter= 0; siter < st.end()-st.begin();siter++)
	{
		string vname = (string)st[siter]["Vectorname"];
		if (vname == vsname)
		{
			for (int it = 0; it <st[siter]["strings"].end()-st[siter]["strings"].begin(); it++)
			{
				string sname = st[siter]["strings"][it]["stringname"];
				vs.push_back(sname);
			}

			break;
		}		
	}
	if (siter == st.end()-st.begin())
	{
		//p = Point(0,0);
		cout<<"no this Vstring data!"<<endl;
	}
	
	fs.release();

	return vs;
}

//读取参数文档里所有数据
m_allnodedata Document::readalldata(string routename)
{
	m_allnodedata alldata;

	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	FileStorage fs;
	try
	{	
		fs.open(routename,0);//read
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"\nXML Exception!" ;
		fs.release();

		//同时，读备份文档
		FileStorage fcopy;
		fcopy.open(newroutename,0);//read模式打开备份文档

		///////////////////read datas//////////////////

		//读点
		FileNode pt = fcopy["pointlist"];	
		for (int piter= 0; piter < pt.end()-pt.begin();piter++)
		{
			string name = (string)pt[piter]["name"];
			int x = (int)pt[piter]["x"];
			int y = (int)pt[piter]["y"];

			m_point p;
			p.point_name = name;
			p.m_p = Point(x,y);

			alldata.points.push_back(p);
		}

		//读矩形
		FileNode rt = fcopy["rectlist"];	
		for (int riter= 0; riter < rt.end()-rt.begin();riter++)
		{
			string name = (string)rt[riter]["name"];
			int x = (int)rt[riter]["x"];
			int y = (int)rt[riter]["y"];
			int height = (int)rt[riter]["height"];
			int width = (int)rt[riter]["width"];

			m_rect r;
			r.rect_name = name;
			r.m_r = Rect(x,y,width,height);

			alldata.rects.push_back(r);
		}

		//读单独值
		FileNode xt = fcopy["alonexlist"];	
		for (int xiter= 0; xiter < xt.end()-xt.begin();xiter++)
		{
			string name = (string)xt[xiter]["name"];
			float x = (float)xt[xiter]["x"];

			m_alonex alonex;
			alonex.m_name = name;
			alonex.m_x = x;

			alldata.xs.push_back(alonex);
		}

		//读字符串向量
		FileNode st = fcopy["Vstringslist"];	
		for (int siter= 0; siter < st.end()-st.begin();siter++)
		{
			string vname = (string)st[siter]["Vectorname"];
			//vector<string>strs;//当前节点字符串向量
			m_Vstrings Vstrs;
			Vstrs.m_Vsname = vname;
			for (int it = 0; it <st[siter]["strings"].end()-st[siter]["strings"].begin(); it++)
			{
				string sname = st[siter]["strings"][it]["stringname"];
				Vstrs.Vstrings.push_back(sname);
			}
			alldata.VSs.push_back(Vstrs);	
		}

		//读点向量
		FileNode vpt = fcopy["Vpointslist"];	
		for (int vpiter= 0; vpiter < vpt.end()-vpt.begin();vpiter++)
		{
			string vname = (string)vpt[vpiter]["Vectorname"];
			//vector<string>strs;//当前节点字符串向量
			m_Vpoints Vps;
			Vps.m_Vpname = vname;
			for (int it = 0; it <vpt[vpiter]["points"].end()-vpt[vpiter]["points"].begin(); it++)
			{
				int x = vpt[vpiter]["points"][it]["x"];
				int y = vpt[vpiter]["points"][it]["y"];

				Vps.Vpoints.push_back(Point(x,y));
			}
			alldata.VPs.push_back(Vps);
		}

		//读实际到图像映射的点向量
		FileNode vopt = fcopy["VDisandPointslist"];	
		for (int vopiter= 0; vopiter < vopt.end()-vopt.begin();vopiter++)
		{
			string vname = (string)vopt[vopiter]["Vectorname"];
			m_OrientationPs Vops;
			Vops.point_name = vname;
			for (int it = 0; it <vopt[vopiter]["points"].end()-vopt[vopiter]["points"].begin(); it++)
			{
				double dis = vopt[vopiter]["points"][it]["distance"];
				int x = vopt[vopiter]["points"][it]["x"];
				int y = vopt[vopiter]["points"][it]["y"];

				Vops.VLinePoints.push_back(make_pair(dis,Point(x,y)));
			}
			alldata.VOPs.push_back(Vops);
		}

		//读激光参数
		FileNode vlpt = fcopy["laser_datas_list"];		
		for (int vlpiter= 0; vlpiter < vlpt.end()-vlpt.begin();vlpiter++)
		{
			laser_parameters laserdata;
			laserdata.guideline_stopline_name = (string)vlpt[vlpiter]["guideline_stopline_name"];
			laserdata.object_dis = (float)vlpt[vlpiter]["object_dis"];
			laserdata.guide_line_k = (float)vlpt[vlpiter]["guide_line_k"];
			laserdata.guide_line_b = (float)vlpt[vlpiter]["guide_line_b"];
			laserdata.guide_line_x = (float)vlpt[vlpiter]["guide_line_x"];
			laserdata.guide_line_y = (float)vlpt[vlpiter]["guide_line_y"];
			laserdata.offset_angle = (float)vlpt[vlpiter]["offset_angle"];
			alldata.VLPs.push_back(laserdata);
		}

		//视觉参数
		FileNode t= fcopy["vision_machine_datas_list"];

		alldata.V_M_datas.CheckVisionEngineNum = (int)t["CheckVisionEngineNum"];
		alldata.V_M_datas.CheckVisionEngineGap = (int)t["CheckVisionEngineGap"];
		alldata.V_M_datas.MaxReportDistance = (float)t["MaxReportDistance"];
		alldata.V_M_datas.GuideLineWidth = (float)t["GuideLineWidth"];
		alldata.V_M_datas.BlackestJudgeStart = (float)t["BlackestJudgeStart"];
		alldata.V_M_datas.BlackestJudgeStep = (float)t["BlackestJudgeStep"];
		alldata.V_M_datas.CircleJudgeStart = (float)t["CircleJudgeStart"];
		alldata.V_M_datas.CircleJudgeStep = (float)t["CircleJudgeStep"];
		alldata.V_M_datas.maxIterTimes = (float)t["maxIterTimes"];
		alldata.V_M_datas.MinEngineRadius = (float)t["MinEngineRadius"];
		alldata.V_M_datas.MinEnginePerimeter = (float)t["MinEnginePerimeter"];
		alldata.V_M_datas.MinTemplateScale = (float)t["MinTemplateScale"];
		alldata.V_M_datas.MaxTemplateScale = (float)t["MaxTemplateScale"];
		alldata.V_M_datas.MinMatchRate = (float)t["MinMatchRate"];
		alldata.V_M_datas.WheelDetectParamX = (float)t["WheelDetectParamX"];
		alldata.V_M_datas.WheelDetectParamY = (float)t["WheelDetectParamY"];
		alldata.V_M_datas.WheelRectHeight = (float)t["WheelRectHeight"];
		alldata.V_M_datas.CutDownThresh = (float)t["CutDownThresh"];
		alldata.V_M_datas.OriginalScale = (float)t["OriginalScale"];
		alldata.V_M_datas.MaxOffset = (float)t["MaxOffset"];

		//激光参数
		t= fcopy["laser_machine_datas_list"];

		alldata.L_M_datas.install_high = (float)t["install_high"];
		alldata.L_M_datas.HrzConfirmSteps = (int)t["HrzConfirmSteps"];
		alldata.L_M_datas.VertConfirmSteps = (int)t["VertConfirmSteps"];
		alldata.L_M_datas.Q12Steps = (int)t["Q12Steps"];
		alldata.L_M_datas.HrzOffsetAngle = (float)t["HrzOffsetAngle"];
		alldata.L_M_datas.HrzConfirmProbability = (float)t["HrzConfirmProbability"];
		alldata.L_M_datas.VertConfirmProbability = (float)t["VertConfirmProbability"];
		alldata.L_M_datas.WalkTestDistance = (int)t["WalkTestDistance"];
		alldata.L_M_datas.LaserForwardOffset = (int)t["LaserForwardOffset"];
		alldata.L_M_datas.LaserBackwardOffset = (int)t["LaserBackwardOffset"];
		alldata.L_M_datas.VerifyEngine = (int)t["VerifyEngine"];
		alldata.L_M_datas.VerifyPlaneType = (int)t["VerifyPlaneType"];
		alldata.L_M_datas.RightLeftEngine = (int)t["RightLeftEngine"];
		alldata.L_M_datas.NeedYardScan = (int)t["NeedYardScan"];
		alldata.L_M_datas.NeedBridgeScan = (int)t["NeedBridgeScan"];
		alldata.L_M_datas.RegularVertical = (int)t["RegularVertical"];
		alldata.L_M_datas.RegularVerticalTime = (int)t["RegularVerticalTime"];
		alldata.L_M_datas.AccurateGuideDis = (int)t["AccurateGuideDis"];
		alldata.L_M_datas.yard_scan_range = (int)t["yard_scan_range"];
		alldata.L_M_datas.yard_scan_obstacle_high = (int)t["yard_scan_obstacle_high"];
		alldata.L_M_datas.yard_scan_obstacle_width = (int)t["yard_scan_obstacle_width"];
		alldata.L_M_datas.VerifyHeadHighDis = (int)t["VerifyHeadHighDis"];
		alldata.L_M_datas.VerifyHeadHighMin = (int)t["VerifyHeadHighMin"];
		alldata.L_M_datas.VerifyHeadHighMax = (int)t["VerifyHeadHighMax"];
		alldata.L_M_datas.VerifyEngineDis = (int)t["VerifyEngineDis"];
		alldata.L_M_datas.EngineHeadDisMin = (int)t["EngineHeadDisMin"];
		alldata.L_M_datas.ScanEngineTimes = (int)t["ScanEngineTimes"];
		alldata.L_M_datas.TowContinu = (int)t["TowContinu"];
		alldata.L_M_datas.VerifyEngineWidthMin = (int)t["VerifyEngineWidthMin"];
		alldata.L_M_datas.VerifyEngineWidthMax = (int)t["VerifyEngineWidthMax"];
		alldata.L_M_datas.VerifyEngineCabinDisMin = (int)t["VerifyEngineCabinDisMin"];
		alldata.L_M_datas.VerifyEngineCabinDisMax = (int)t["VerifyEngineCabinDisMax"];
		alldata.L_M_datas.VerifyEngineHeadDisMin = (int)t["VerifyEngineHeadDisMin"];
		alldata.L_M_datas.VerifyEngineHeadDisMax = (int)t["VerifyEngineHeadDisMax"];
		alldata.L_M_datas.NeedShieldObscur = (int)t["NeedShieldObscur"];
		alldata.L_M_datas.Y_Obscur = (int)t["Y_Obscur"];
		alldata.L_M_datas.X_Obscur_Left = (int)t["X_Obscur_Left"];
		alldata.L_M_datas.X_Obscur_Right = (int)t["X_Obscur_Right"];
		alldata.L_M_datas.PlaneLeaveHighMin = (int)t["PlaneLeaveHighMin"];
		alldata.L_M_datas.PlaneLeaveHighMax = (int)t["PlaneLeaveHighMax"];
		alldata.L_M_datas.PlaneLeaveYMin = (int)t["PlaneLeaveYMin"];
		alldata.L_M_datas.PlaneLeaveYMax = (int)t["PlaneLeaveYMax"];
		alldata.L_M_datas.PlaneLeaveZMin = (int)t["PlaneLeaveZMin"];
		alldata.L_M_datas.PlaneLeaveConfirmPoint = (int)t["PlaneLeaveConfirmPoint"];
		alldata.L_M_datas.PlaneLeaveConfirmLine = (int)t["PlaneLeaveConfirmLine"];
		alldata.L_M_datas.SuspectMode = (int)t["SuspectMode"];
		alldata.L_M_datas.SuspectHotMax = (int)t["SuspectHotMax"];
		alldata.L_M_datas.SuspectRowMin = (int)t["SuspectRowMin"];
		alldata.L_M_datas.SuspectSMNumberMin = (int)t["SuspectSMNumberMin"];
		alldata.L_M_datas.MaxDistance = (int)t["MaxDistance"];
		alldata.L_M_datas.CatchDisMin = (int)t["CatchDisMin"];
		alldata.L_M_datas.CatchDisMax = (int)t["CatchDisMax"];
		alldata.L_M_datas.ObjectHighMin = (int)t["ObjectHighMin"];
		alldata.L_M_datas.ObjectHighMax = (int)t["ObjectHighMax"];
		alldata.L_M_datas.CarHighMax = (int)t["CarHighMax"];
		alldata.L_M_datas.ObjectDisMin = (int)t["ObjectDisMin"];
		alldata.L_M_datas.ObjectDisMax = (int)t["ObjectDisMax"];
		alldata.L_M_datas.WidthMaxRatio = (float)t["WidthMaxRatio"];
		alldata.L_M_datas.WidthMeasureRatio = (float)t["WidthMeasureRatio"];
		alldata.L_M_datas.NeighborDifferMax = (int)t["NeighborDifferMax"];
		alldata.L_M_datas.PlanePointNumberMin = (int)t["PlanePointNumberMin"];
		alldata.L_M_datas.BirdgePointNumberMin = (int)t["BirdgePointNumberMin"];
		alldata.L_M_datas.MaxGap = (int)t["MaxGap"];
		alldata.L_M_datas.AlwayMoveMax = (int)t["AlwayMoveMax"];
		alldata.L_M_datas.OfflineSimulaMode = (int)t["OfflineSimulaMode"];
		alldata.L_M_datas.SaveDataWhenOffline = (int)t["SaveDataWhenOffline"];
		alldata.L_M_datas.OfflineSleepTime = (int)t["OfflineSleepTime"];

		//系统参数
		t= fcopy["system_parameter"];

		alldata.V_S_datas.RunningMode = (string)t["RunningMode"];
		alldata.V_S_datas.StationNumber = (string)t["StationNumber"];
		alldata.V_S_datas.ServerAddress = (string)t["ServerAddress"];
		alldata.V_S_datas.ServerPort = (string)t["ServerPort"];
		alldata.V_S_datas.LogSaveRoot = (string)t["LogSaveRoot"];
		alldata.V_S_datas.LogPreserveDays = (double)t["LogPreserveDays"];
		alldata.V_S_datas.LEDCOM = (string)t["LEDCOM"];
		alldata.V_S_datas.LEDPAUD = (int)t["LEDPAUD"];
		alldata.V_S_datas.MODBUSCOM = (string)t["MODBUSCOM"];
		alldata.V_S_datas.MODBUSPAUD = (int)t["MODBUSPAUD"];
		alldata.V_S_datas.LASERCOM = (string)t["LASERCOM"];
		alldata.V_S_datas.LASERPAUD = (int)t["LASERPAUD"];
		alldata.V_S_datas.DIPCOM = (string)t["DIPCOM"];
		alldata.V_S_datas.DIPPAUD = (int)t["DIPPAUD"];

		alldata.V_S_datas.NeedPlaneLeaveScan = (double)t["NeedPlaneLeaveScan"];
		alldata.V_S_datas.AccuracyGuideDistance = (double)t["AccuracyGuideDistance"];
		alldata.V_S_datas.DriftDistance = (double)t["DriftDistance"];
		alldata.V_S_datas.DriftRemove = (double)t["DriftRemove"];
		alldata.V_S_datas.OverSpeed24 = (double)t["OverSpeed24"];
		alldata.V_S_datas.OverSpeed24Remove = (double)t["OverSpeed24Remove"];
		alldata.V_S_datas.OverSpeed = (double)t["OverSpeed"];
		alldata.V_S_datas.OverSpeedRemove = (double)t["OverSpeedRemove"];
		alldata.V_S_datas.ValidateDistance = (double)t["ValidateDistance"];
		alldata.V_S_datas.DriftWeightStart = (double)t["DriftWeightStart"];
		alldata.V_S_datas.DriftWeightEnd = (double)t["DriftWeightEnd"];
		alldata.V_S_datas.DistanceWeightStart = (double)t["DistanceWeightStart"];
		alldata.V_S_datas.DistanceWeightEnd = (double)t["DistanceWeightEnd"];
		alldata.V_S_datas.SpeedWeightStart = (double)t["SpeedWeightStart"];
		alldata.V_S_datas.SpeedWeightEnd = (double)t["SpeedWeightEnd"];
		alldata.V_S_datas.SlowDistance = (double)t["SlowDistance"];
		alldata.V_S_datas.DriftGain = (double)t["DriftGain"];
		alldata.V_S_datas.DistanceGain = (double)t["DistanceGain"];
		alldata.V_S_datas.MinDriftDistance = (double)t["MinDriftDistance"];
		alldata.V_S_datas.MaxStopDistance = (double)t["MaxStopDistance"];
		alldata.V_S_datas.TooFarDistance = (double)t["TooFarDistance"];
		alldata.V_S_datas.SubsystemErrorTime = (double)t["SubsystemErrorTime"];
		alldata.V_S_datas.LEDDelayTime = (double)t["LEDDelayTime"];
		alldata.V_S_datas.ModbusDelayTime = (double)t["ModbusDelayTime"];
		alldata.V_S_datas.BoardInterlockPort = (int)t["BoardInterlockPort"];
		alldata.V_S_datas.EmergencyStopPort = (int)t["EmergencyStopPort"];
		alldata.V_S_datas.AutoSleepTime = (double)t["AutoSleepTime"];
		alldata.V_S_datas.StopDisplayTime = (double)t["StopDisplayTime"];
		alldata.V_S_datas.ChockOnDisplayTime = (double)t["ChockOnDisplayTime"];
		alldata.V_S_datas.MaxSpeed = (double)t["MaxSpeed"];
		alldata.V_S_datas.MaxSpeedRemove = (double)t["MaxSpeedRemove"];
		alldata.V_S_datas.StopToOKTime = (double)t["StopToOKTime"];
		alldata.V_S_datas.SpeedSmoothWindow = (double)t["SpeedSmoothWindow"];
		alldata.V_S_datas.PBBAllow = (int)t["PBBAllow"];
		alldata.V_S_datas.PCAAllow = (int)t["PCAAllow"];
		alldata.V_S_datas.FGPAllow = (int)t["FGPAllow"];
		alldata.V_S_datas.PCAPort = (int)t["PCAPort"];
		alldata.V_S_datas.FGPPort = (int)t["FGPPort"];
		alldata.V_S_datas.Q12Port = (int)t["Q12Port"];
		alldata.V_S_datas.UPSPort = (int)t["UPSPort"];
		alldata.V_S_datas.LEDCOMERRPort = (int)t["LEDCOMERRPort"];
		alldata.V_S_datas.LEDERRPort = (int)t["LEDERRPort"];
		alldata.V_S_datas.DiskSpaceAlarm = (double)t["DiskSpaceAlarm"];
		alldata.V_S_datas.TaskMode = (int)t["TaskMode"];
		alldata.V_S_datas.SaveKeyImage = (int)t["SaveKeyImage"];
		alldata.V_S_datas.CheckLeaveMode = (int)t["CheckLeaveMode"];
		alldata.V_S_datas.RestartInterval = (int)t["RestartInterval"];
		alldata.V_S_datas.RestartTime = (int)t["RestartTime"];
		alldata.V_S_datas.CheckLeaveTime = (double)t["CheckLeaveTime"];
		alldata.V_S_datas.MaxAngleShakeX = (double)t["MaxAngleShakeX"];
		alldata.V_S_datas.MaxAngleShakeY = (double)t["MaxAngleShakeY"];
		alldata.V_S_datas.MaxAngleBiasX = (double)t["MaxAngleBiasX"];
		alldata.V_S_datas.MaxAngleBiasY = (double)t["MaxAngleBiasY"];

		fcopy.release();
		return alldata;
	}

	//打开成功了，则进行读，并将读到的数据写成备份文档
	//读点
	FileNode pt = fs["pointlist"];	
	for (int piter= 0; piter < pt.end()-pt.begin();piter++)
	{
		string name = (string)pt[piter]["name"];
		int x = (int)pt[piter]["x"];
		int y = (int)pt[piter]["y"];

		m_point p;
		p.point_name = name;
		p.m_p = Point(x,y);

		alldata.points.push_back(p);
	}

	//读矩形
	FileNode rt = fs["rectlist"];	
	for (int riter= 0; riter < rt.end()-rt.begin();riter++)
	{
		string name = (string)rt[riter]["name"];
		int x = (int)rt[riter]["x"];
		int y = (int)rt[riter]["y"];
		int height = (int)rt[riter]["height"];
		int width = (int)rt[riter]["width"];

		m_rect r;
		r.rect_name = name;
		r.m_r = Rect(x,y,width,height);

		alldata.rects.push_back(r);
	}

	//读单独值
	FileNode xt = fs["alonexlist"];	
	for (int xiter= 0; xiter < xt.end()-xt.begin();xiter++)
	{
		string name = (string)xt[xiter]["name"];
		float x = (float)xt[xiter]["x"];
		
		m_alonex alonex;
		alonex.m_name = name;
		alonex.m_x = x;

		alldata.xs.push_back(alonex);
	}

	//读字符串向量
	FileNode st = fs["Vstringslist"];	
	for (int siter= 0; siter < st.end()-st.begin();siter++)
	{
		string vname = (string)st[siter]["Vectorname"];
		//vector<string>strs;//当前节点字符串向量
		m_Vstrings Vstrs;
		Vstrs.m_Vsname = vname;
		for (int it = 0; it <st[siter]["strings"].end()-st[siter]["strings"].begin(); it++)
		{
			string sname = st[siter]["strings"][it]["stringname"];
			Vstrs.Vstrings.push_back(sname);
		}
		alldata.VSs.push_back(Vstrs);	
	}

	//读点向量
	FileNode vpt = fs["Vpointslist"];	
	for (int vpiter= 0; vpiter < vpt.end()-vpt.begin();vpiter++)
	{
		string vname = (string)vpt[vpiter]["Vectorname"];
		//vector<string>strs;//当前节点字符串向量
		m_Vpoints Vps;
		Vps.m_Vpname = vname;
		for (int it = 0; it <vpt[vpiter]["points"].end()-vpt[vpiter]["points"].begin(); it++)
		{
			int x = vpt[vpiter]["points"][it]["x"];
			int y = vpt[vpiter]["points"][it]["y"];

			Vps.Vpoints.push_back(Point(x,y));
		}
		alldata.VPs.push_back(Vps);
	}

	//读实际到图像映射的点向量
	FileNode vopt = fs["VDisandPointslist"];	
	for (int vopiter= 0; vopiter < vopt.end()-vopt.begin();vopiter++)
	{
		string vname = (string)vopt[vopiter]["Vectorname"];
		m_OrientationPs Vops;
		Vops.point_name = vname;
		for (int it = 0; it <vopt[vopiter]["points"].end()-vopt[vopiter]["points"].begin(); it++)
		{
			double dis = vopt[vopiter]["points"][it]["distance"];
			int x = vopt[vopiter]["points"][it]["x"];
			int y = vopt[vopiter]["points"][it]["y"];

			Vops.VLinePoints.push_back(make_pair(dis,Point(x,y)));
		}
		alldata.VOPs.push_back(Vops);
	}

	//读激光参数
	FileNode vlpt = fs["laser_datas_list"];		
	for (int vlpiter= 0; vlpiter < vlpt.end()-vlpt.begin();vlpiter++)
	{
		laser_parameters laserdata;
		laserdata.guideline_stopline_name = (string)vlpt[vlpiter]["guideline_stopline_name"];
		laserdata.object_dis = (float)vlpt[vlpiter]["object_dis"];
		laserdata.guide_line_k = (float)vlpt[vlpiter]["guide_line_k"];
		laserdata.guide_line_b = (float)vlpt[vlpiter]["guide_line_b"];
		laserdata.guide_line_x = (float)vlpt[vlpiter]["guide_line_x"];
		laserdata.guide_line_y = (float)vlpt[vlpiter]["guide_line_y"];
		laserdata.offset_angle = (float)vlpt[vlpiter]["offset_angle"];
		alldata.VLPs.push_back(laserdata);
	}

	
	FileNode t= fs["vision_machine_datas_list"];

	alldata.V_M_datas.CheckVisionEngineNum = (int)t["CheckVisionEngineNum"];
	alldata.V_M_datas.CheckVisionEngineGap = (int)t["CheckVisionEngineGap"];
	alldata.V_M_datas.MaxReportDistance = (float)t["MaxReportDistance"];
	alldata.V_M_datas.GuideLineWidth = (float)t["GuideLineWidth"];
	alldata.V_M_datas.BlackestJudgeStart = (float)t["BlackestJudgeStart"];
	alldata.V_M_datas.BlackestJudgeStep = (float)t["BlackestJudgeStep"];
	alldata.V_M_datas.CircleJudgeStart = (float)t["CircleJudgeStart"];
	alldata.V_M_datas.CircleJudgeStep = (float)t["CircleJudgeStep"];
	alldata.V_M_datas.maxIterTimes = (float)t["maxIterTimes"];
	alldata.V_M_datas.MinEngineRadius = (float)t["MinEngineRadius"];
	alldata.V_M_datas.MinEnginePerimeter = (float)t["MinEnginePerimeter"];
	alldata.V_M_datas.MinTemplateScale = (float)t["MinTemplateScale"];
	alldata.V_M_datas.MaxTemplateScale = (float)t["MaxTemplateScale"];
	alldata.V_M_datas.MinMatchRate = (float)t["MinMatchRate"];
	alldata.V_M_datas.WheelDetectParamX = (float)t["WheelDetectParamX"];
	alldata.V_M_datas.WheelDetectParamY = (float)t["WheelDetectParamY"];
	alldata.V_M_datas.WheelRectHeight = (float)t["WheelRectHeight"];
	alldata.V_M_datas.CutDownThresh = (float)t["CutDownThresh"];
	alldata.V_M_datas.OriginalScale = (float)t["OriginalScale"];
	alldata.V_M_datas.MaxOffset = (float)t["MaxOffset"];

	//激光参数
	t= fs["laser_machine_datas_list"];

	alldata.L_M_datas.install_high = (float)t["install_high"];
	alldata.L_M_datas.HrzConfirmSteps = (int)t["HrzConfirmSteps"];
	alldata.L_M_datas.VertConfirmSteps = (int)t["VertConfirmSteps"];
	alldata.L_M_datas.Q12Steps = (int)t["Q12Steps"];
	alldata.L_M_datas.HrzOffsetAngle = (float)t["HrzOffsetAngle"];
	alldata.L_M_datas.HrzConfirmProbability = (float)t["HrzConfirmProbability"];
	alldata.L_M_datas.VertConfirmProbability = (float)t["VertConfirmProbability"];
	alldata.L_M_datas.WalkTestDistance = (int)t["WalkTestDistance"];
	alldata.L_M_datas.LaserForwardOffset = (int)t["LaserForwardOffset"];
	alldata.L_M_datas.LaserBackwardOffset = (int)t["LaserBackwardOffset"];
	alldata.L_M_datas.VerifyEngine = (int)t["VerifyEngine"];
	alldata.L_M_datas.VerifyPlaneType = (int)t["VerifyPlaneType"];
	alldata.L_M_datas.RightLeftEngine = (int)t["RightLeftEngine"];
	alldata.L_M_datas.NeedYardScan = (int)t["NeedYardScan"];
	alldata.L_M_datas.NeedBridgeScan = (int)t["NeedBridgeScan"];
	alldata.L_M_datas.RegularVertical = (int)t["RegularVertical"];
	alldata.L_M_datas.RegularVerticalTime = (int)t["RegularVerticalTime"];
	alldata.L_M_datas.AccurateGuideDis = (int)t["AccurateGuideDis"];
	alldata.L_M_datas.yard_scan_range = (int)t["yard_scan_range"];
	alldata.L_M_datas.yard_scan_obstacle_high = (int)t["yard_scan_obstacle_high"];
	alldata.L_M_datas.yard_scan_obstacle_width = (int)t["yard_scan_obstacle_width"];
	alldata.L_M_datas.VerifyHeadHighDis = (int)t["VerifyHeadHighDis"];
	alldata.L_M_datas.VerifyHeadHighMin = (int)t["VerifyHeadHighMin"];
	alldata.L_M_datas.VerifyHeadHighMax = (int)t["VerifyHeadHighMax"];
	alldata.L_M_datas.VerifyEngineDis = (int)t["VerifyEngineDis"];
	alldata.L_M_datas.EngineHeadDisMin = (int)t["EngineHeadDisMin"];
	alldata.L_M_datas.ScanEngineTimes = (int)t["ScanEngineTimes"];
	alldata.L_M_datas.TowContinu = (int)t["TowContinu"];
	alldata.L_M_datas.VerifyEngineWidthMin = (int)t["VerifyEngineWidthMin"];
	alldata.L_M_datas.VerifyEngineWidthMax = (int)t["VerifyEngineWidthMax"];
	alldata.L_M_datas.VerifyEngineCabinDisMin = (int)t["VerifyEngineCabinDisMin"];
	alldata.L_M_datas.VerifyEngineCabinDisMax = (int)t["VerifyEngineCabinDisMax"];
	alldata.L_M_datas.VerifyEngineHeadDisMin = (int)t["VerifyEngineHeadDisMin"];
	alldata.L_M_datas.VerifyEngineHeadDisMax = (int)t["VerifyEngineHeadDisMax"];
	alldata.L_M_datas.NeedShieldObscur = (int)t["NeedShieldObscur"];
	alldata.L_M_datas.Y_Obscur = (int)t["Y_Obscur"];
	alldata.L_M_datas.X_Obscur_Left = (int)t["X_Obscur_Left"];
	alldata.L_M_datas.X_Obscur_Right = (int)t["X_Obscur_Right"];
	alldata.L_M_datas.PlaneLeaveHighMin = (int)t["PlaneLeaveHighMin"];
	alldata.L_M_datas.PlaneLeaveHighMax = (int)t["PlaneLeaveHighMax"];
	alldata.L_M_datas.PlaneLeaveYMin = (int)t["PlaneLeaveYMin"];
	alldata.L_M_datas.PlaneLeaveYMax = (int)t["PlaneLeaveYMax"];
	alldata.L_M_datas.PlaneLeaveZMin = (int)t["PlaneLeaveZMin"];
	alldata.L_M_datas.PlaneLeaveConfirmPoint = (int)t["PlaneLeaveConfirmPoint"];
	alldata.L_M_datas.PlaneLeaveConfirmLine = (int)t["PlaneLeaveConfirmLine"];
	alldata.L_M_datas.SuspectMode = (int)t["SuspectMode"];
	alldata.L_M_datas.SuspectHotMax = (int)t["SuspectHotMax"];
	alldata.L_M_datas.SuspectRowMin = (int)t["SuspectRowMin"];
	alldata.L_M_datas.SuspectSMNumberMin = (int)t["SuspectSMNumberMin"];
	alldata.L_M_datas.MaxDistance = (int)t["MaxDistance"];
	alldata.L_M_datas.CatchDisMin = (int)t["CatchDisMin"];
	alldata.L_M_datas.CatchDisMax = (int)t["CatchDisMax"];
	alldata.L_M_datas.ObjectHighMin = (int)t["ObjectHighMin"];
	alldata.L_M_datas.ObjectHighMax = (int)t["ObjectHighMax"];
	alldata.L_M_datas.CarHighMax = (int)t["CarHighMax"];
	alldata.L_M_datas.ObjectDisMin = (int)t["ObjectDisMin"];
	alldata.L_M_datas.ObjectDisMax = (int)t["ObjectDisMax"];
	alldata.L_M_datas.WidthMaxRatio = (float)t["WidthMaxRatio"];
	alldata.L_M_datas.WidthMeasureRatio = (float)t["WidthMeasureRatio"];
	alldata.L_M_datas.NeighborDifferMax = (int)t["NeighborDifferMax"];
	alldata.L_M_datas.PlanePointNumberMin = (int)t["PlanePointNumberMin"];
	alldata.L_M_datas.BirdgePointNumberMin = (int)t["BirdgePointNumberMin"];
	alldata.L_M_datas.MaxGap = (int)t["MaxGap"];
	alldata.L_M_datas.AlwayMoveMax = (int)t["AlwayMoveMax"];
	alldata.L_M_datas.OfflineSimulaMode = (int)t["OfflineSimulaMode"];
	alldata.L_M_datas.SaveDataWhenOffline = (int)t["SaveDataWhenOffline"];
	alldata.L_M_datas.OfflineSleepTime = (int)t["OfflineSleepTime"];

	t= fs["system_parameter"];

	alldata.V_S_datas.RunningMode = (string)t["RunningMode"];
	alldata.V_S_datas.StationNumber = (string)t["StationNumber"];
	alldata.V_S_datas.ServerAddress = (string)t["ServerAddress"];
	alldata.V_S_datas.ServerPort = (string)t["ServerPort"];
	alldata.V_S_datas.LogSaveRoot = (string)t["LogSaveRoot"];
	alldata.V_S_datas.LogPreserveDays = (double)t["LogPreserveDays"];
	alldata.V_S_datas.LEDCOM = (string)t["LEDCOM"];
	alldata.V_S_datas.LEDPAUD = (int)t["LEDPAUD"];
	alldata.V_S_datas.MODBUSCOM = (string)t["MODBUSCOM"];
	alldata.V_S_datas.MODBUSPAUD = (int)t["MODBUSPAUD"];
	alldata.V_S_datas.LASERCOM = (string)t["LASERCOM"];
	alldata.V_S_datas.LASERPAUD = (int)t["LASERPAUD"];
	alldata.V_S_datas.DIPCOM = (string)t["DIPCOM"];
	alldata.V_S_datas.DIPPAUD = (int)t["DIPPAUD"];

	alldata.V_S_datas.NeedPlaneLeaveScan = (double)t["NeedPlaneLeaveScan"];
	alldata.V_S_datas.AccuracyGuideDistance = (double)t["AccuracyGuideDistance"];
	alldata.V_S_datas.DriftDistance = (double)t["DriftDistance"];
	alldata.V_S_datas.DriftRemove = (double)t["DriftRemove"];
	alldata.V_S_datas.OverSpeed24 = (double)t["OverSpeed24"];
	alldata.V_S_datas.OverSpeed24Remove = (double)t["OverSpeed24Remove"];
	alldata.V_S_datas.OverSpeed = (double)t["OverSpeed"];
	alldata.V_S_datas.OverSpeedRemove = (double)t["OverSpeedRemove"];
	alldata.V_S_datas.ValidateDistance = (double)t["ValidateDistance"];
	alldata.V_S_datas.DriftWeightStart = (double)t["DriftWeightStart"];
	alldata.V_S_datas.DriftWeightEnd = (double)t["DriftWeightEnd"];
	alldata.V_S_datas.DistanceWeightStart = (double)t["DistanceWeightStart"];
	alldata.V_S_datas.DistanceWeightEnd = (double)t["DistanceWeightEnd"];
	alldata.V_S_datas.SpeedWeightStart = (double)t["SpeedWeightStart"];
	alldata.V_S_datas.SpeedWeightEnd = (double)t["SpeedWeightEnd"];
	alldata.V_S_datas.SlowDistance = (double)t["SlowDistance"];
	alldata.V_S_datas.DriftGain = (double)t["DriftGain"];
	alldata.V_S_datas.DistanceGain = (double)t["DistanceGain"];
	alldata.V_S_datas.MinDriftDistance = (double)t["MinDriftDistance"];
	alldata.V_S_datas.MaxStopDistance = (double)t["MaxStopDistance"];
	alldata.V_S_datas.TooFarDistance = (double)t["TooFarDistance"];
	alldata.V_S_datas.SubsystemErrorTime = (double)t["SubsystemErrorTime"];
	alldata.V_S_datas.LEDDelayTime = (double)t["LEDDelayTime"];
	alldata.V_S_datas.ModbusDelayTime = (double)t["ModbusDelayTime"];
	alldata.V_S_datas.BoardInterlockPort = (int)t["BoardInterlockPort"];
	alldata.V_S_datas.EmergencyStopPort = (int)t["EmergencyStopPort"];
	alldata.V_S_datas.AutoSleepTime = (double)t["AutoSleepTime"];
	alldata.V_S_datas.StopDisplayTime = (double)t["StopDisplayTime"];
	alldata.V_S_datas.ChockOnDisplayTime = (double)t["ChockOnDisplayTime"];
	alldata.V_S_datas.MaxSpeed = (double)t["MaxSpeed"];
	alldata.V_S_datas.MaxSpeedRemove = (double)t["MaxSpeedRemove"];
	alldata.V_S_datas.StopToOKTime = (double)t["StopToOKTime"];
	alldata.V_S_datas.SpeedSmoothWindow = (double)t["SpeedSmoothWindow"];
	alldata.V_S_datas.PBBAllow = (int)t["PBBAllow"];
	alldata.V_S_datas.PCAAllow = (int)t["PCAAllow"];
	alldata.V_S_datas.FGPAllow = (int)t["FGPAllow"];
	alldata.V_S_datas.PCAPort = (int)t["PCAPort"];
	alldata.V_S_datas.FGPPort = (int)t["FGPPort"];
	alldata.V_S_datas.Q12Port = (int)t["Q12Port"];
	alldata.V_S_datas.UPSPort = (int)t["UPSPort"];
	alldata.V_S_datas.LEDCOMERRPort = (int)t["LEDCOMERRPort"];
	alldata.V_S_datas.LEDERRPort = (int)t["LEDERRPort"];
	alldata.V_S_datas.DiskSpaceAlarm = (double)t["DiskSpaceAlarm"];
	alldata.V_S_datas.TaskMode = (int)t["TaskMode"];
	alldata.V_S_datas.SaveKeyImage = (int)t["SaveKeyImage"];
	alldata.V_S_datas.CheckLeaveMode = (int)t["CheckLeaveMode"];
	alldata.V_S_datas.RestartInterval = (int)t["RestartInterval"];
	alldata.V_S_datas.RestartTime = (int)t["RestartTime"];
	alldata.V_S_datas.CheckLeaveTime = (double)t["CheckLeaveTime"];
	alldata.V_S_datas.MaxAngleShakeX = (double)t["MaxAngleShakeX"];
	alldata.V_S_datas.MaxAngleShakeY = (double)t["MaxAngleShakeY"];
	alldata.V_S_datas.MaxAngleBiasX = (double)t["MaxAngleBiasX"];
	alldata.V_S_datas.MaxAngleBiasY = (double)t["MaxAngleBiasY"];

	fs.release();

	//将所有读到的数据写入备份文档
	//////////实际点与图像坐标////////////
	VorientationVPs = alldata.VOPs;
	VsceneVSs = alldata.VSs;
	VsceneVPs = alldata.VPs;
	VsceneXs = alldata.xs;
	VsceneRs = alldata.rects;
	VscenePs = alldata.points;
	V_M_Datas = alldata.V_M_datas;
	VLaserParameters = alldata.VLPs;
	V_S_Datas = alldata.V_S_datas;
	L_M_Datas = alldata.L_M_datas;
	writealldata(newroutename);
	return alldata;
}

//写结构体向量数据,距离与图像点相对应
void Document::writedata(string routename,string Vpsname,vector<pair<double,Point2d>>VLinePoints, int flag)
{
	m_allnodedata alldata = readalldata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;
	VLaserParameters = alldata.VLPs;
	V_M_Datas = alldata.V_M_datas;
	V_S_Datas = alldata.V_S_datas;
	L_M_Datas = alldata.L_M_datas;
	
	checkVorientationPs(Vpsname, VLinePoints, flag);

	writealldata(routename);
}

//检查距离与图像点相对应向量
void Document::checkVorientationPs(string vpname,vector<pair<double,Point2d>>vPoints, int flag)
{
	if (flag == 0)//该编号引导线标定点数据全部重新写入
	{
		vector<m_OrientationPs>::iterator it = VorientationVPs.begin();
		for (; it != VorientationVPs.end(); it++)
		{
			string name =  VorientationVPs.at(it-VorientationVPs.begin()).point_name;
			if (vpname == name)//如果有重复的引导线节点，则修改其数据，然后全部数据重新写入文档中
			{
				VorientationVPs.at(it-VorientationVPs.begin()).VLinePoints = vPoints;
				break;
			}
		}

		if (it == VorientationVPs.end())//如果不是重复的引导线节点则存入容器VsceneSs,然后全部数据重新写入文档中
		{
			m_OrientationPs newVPs;
			newVPs.point_name= vpname;
			newVPs.VLinePoints = vPoints;

			VorientationVPs.push_back(newVPs);
		}
	}
	else//在上次写好的数据基础上继续添加数据
	{
		//分两种情况，一种是点重复了，一种是点没重复的
		//分析向量里面distance的数据，来决定是否修改或添加point的坐标


		//先找出是哪条引导线需要添加点
		//vector<m_OrientationPs>::iterator it = VorientationVPs.begin();
		int it;
		vector<pair<double,Point2d>> originalVLPs;
		for ( it= 0; it != VorientationVPs.end()-VorientationVPs.begin(); it++)
		{
			string name =  VorientationVPs.at(it).point_name;
			if (vpname == name)//然后取出该条引导线的点向量
			{
				originalVLPs = VorientationVPs.at(it).VLinePoints;
				break;
			}
				
		}

		//处理新增向量中的所有点
		int iter;
		for (int vpointsit = 0; vpointsit != vPoints.end()-vPoints.begin(); vpointsit++)//新增向量的每个点
		{
			for (iter = 0; iter != originalVLPs.end() - originalVLPs.begin();  iter ++)
			{
				double distance =  originalVLPs.at(iter).first;
				if (vPoints.at(vpointsit).first == distance)//相同距离的坐标点，进行修改
				{
					originalVLPs.at(iter).second = vPoints.at(vpointsit).second;		
					break;
				}
			}

			if (iter == originalVLPs.end() - originalVLPs.begin())//若没有距离重复的点，则插入
			{
				originalVLPs.push_back(vPoints.at(vpointsit));
			}			
		}
		
		//存下改好的向量
		VorientationVPs.at(it).VLinePoints = originalVLPs;
	}
}

//读距离与图像点相对应结构体向量数据
vector<pair<double,Point2d>> Document::readVOrientationdata(string routename,string vname)
{
	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	vector<pair<double,Point2d>> vps;

	FileStorage fs;
	try
	{	
		fs.open(routename,0);
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		FileStorage fcopy;
		fcopy.open(newroutename,0);
		FileNode st= fcopy["VDisandPointslist"];
		int siter = st.end()-st.begin();
		for (siter= 0; siter < st.end()-st.begin();siter++)
		{
			string name = (string)st[siter]["Vectorname"];
			if (vname == name)
			{
				for (int it = 0; it <st[siter]["points"].end()-st[siter]["points"].begin(); it++)
				{
					float dis = st[siter]["points"][it]["distance"];
					Point2d P = Point2d(st[siter]["points"][it]["x"],st[siter]["points"][it]["y"]);
					vps.push_back(make_pair<double,Point2d>(static_cast<double&&>((double)dis),static_cast<Point2d&&>(P)));
				}

				break;
			}		
		}
		if (siter == st.end()-st.begin())
		{
			cout<<"no this Vstring data!"<<endl;
		}

		fcopy.release();
		return vps;
	}

	FileNode st= fs["VDisandPointslist"];
	int siter = st.end()-st.begin();
	for (siter= 0; siter < st.end()-st.begin();siter++)
	{
		string name = (string)st[siter]["Vectorname"];
		if (vname == name)
		{
			for (int it = 0; it <st[siter]["points"].end()-st[siter]["points"].begin(); it++)
			{
				float dis = st[siter]["points"][it]["distance"];
				Point2d P = Point2d(st[siter]["points"][it]["x"],st[siter]["points"][it]["y"]);
				vps.push_back(make_pair<double,Point2d>(static_cast<double&&>((double)dis),static_cast<Point2d&&>(P)));
			}

			break;
		}		
	}
	if (siter == st.end()-st.begin())
	{
		cout<<"no this Vstring data!"<<endl;
	}

	fs.release();

	return vps;
}

//由机型读出引导线停止线编号
void Document::read_gl_sl(string routename,string planetype)
{
	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	//string guaidlinename ;
	//string stoplinename ;
	int plane_flag = 0;

	FileStorage fs;
	try
	{	
		fs.open(routename,0);
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		//读备份文档
		FileStorage fcopys;
		fcopys.open(newroutename,0);
		FileNode st= fcopys["Vstringslist"];
		int siter = st.end()-st.begin();
		for (siter= 0; siter < st.end()-st.begin();siter++)
		{
			string guiad = st[siter]["strings"][0]["stringname"];
			string stop = st[siter]["strings"][1]["stringname"];
			for (int it = 2; it <st[siter]["strings"].end()-st[siter]["strings"].begin(); it++)
			{
				string planename = st[siter]["strings"][it]["stringname"];
				if (planename == planetype)
				{
					guaidlinename = guiad;
					stoplinename = stop;
					plane_flag = 1;
					break;
				}
			}
			if (1 == plane_flag)
			{
				break;
			}			
		}

		if (siter == st.end()-st.begin())
		{
			guaidlinename = "";
			stoplinename = "";
			cout<<"no this planetype!"<<endl;
		}

		fcopys.release();
	}

	FileNode st= fs["Vstringslist"];
	int siter = st.end()-st.begin();
	for (siter= 0; siter < st.end()-st.begin();siter++)
	{
		string guiad = st[siter]["strings"][0]["stringname"];
		string stop = st[siter]["strings"][1]["stringname"];
		for (int it = 2; it <st[siter]["strings"].end()-st[siter]["strings"].begin(); it++)
		{
			string planename = st[siter]["strings"][it]["stringname"];
			if (planename == planetype)
			{
				guaidlinename = guiad;
				stoplinename = stop;
				plane_flag = 1;
				break;
			}
		}
		if (1 == plane_flag)
		{
			break;
		}			
	}

	if (siter == st.end()-st.begin())
	{
		guaidlinename = "";
		stoplinename = "";
		cout<<"no this planetype!"<<endl;
	}

	fs.release();
}

vector<string> Document::read_Vplanetypes(string routename,string gl_slname)
{
	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	vector<string>planetypes;

	FileStorage fs;
	try
	{	
		fs.open(routename,0);
	}
	catch(std::exception e)
	{
		cout<<routename<<"XML Exception!" ;
		fs.release();
		//打开备份文档读
		FileStorage fcopys;
		fcopys.open(newroutename,0);
		FileNode st= fcopys["Vstringslist"];
		int siter = st.end()-st.begin();
		for (siter= 0; siter < st.end()-st.begin();siter++)
		{
			string gl_sl = st[siter]["Vectorname"];
			if (gl_slname == gl_sl)//找到输入的停止线和引导线编号，则读出所有的机型编号
			{
				for (int it = 2; it <st[siter]["strings"].end()-st[siter]["strings"].begin(); it++)
				{
					string planename = st[siter]["strings"][it]["stringname"];
					planetypes.push_back(planename);
				}
				fcopys.release();
				return planetypes;
			}	
		}

		if (siter == st.end()-st.begin())
		{
			guaidlinename = "";
			stoplinename = "";
			cout<<"no this gl_sl!"<<endl;
		}
		
		fcopys.release();
		return planetypes;
	}

	FileNode st= fs["Vstringslist"];
	int siter = st.end()-st.begin();
	for (siter= 0; siter < st.end()-st.begin();siter++)
	{
		string gl_sl = st[siter]["Vectorname"];
		if (gl_slname == gl_sl)//找到输入的停止线和引导线编号，则读出所有的机型编号
		{
			for (int it = 2; it <st[siter]["strings"].end()-st[siter]["strings"].begin(); it++)
			{
				string planename = st[siter]["strings"][it]["stringname"];
				planetypes.push_back(planename);
			}
			fs.release();
			return planetypes;
		}	
	}

	if (siter == st.end()-st.begin())
	{
		guaidlinename = "";
		stoplinename = "";
		cout<<"no this gl_sl!"<<endl;
	}
	
	fs.release();
	return planetypes;
}

//由机型读出机型数据
void Document::readplanedata(string routename,string planetype, PlaneInfo& planedata)//读机型数据
{
	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	//PlaneInfo planedata;
	planedata.head_Height = 0;
	memset(&planedata, 0, sizeof(planedata));
	//FileStorage fs;
	FileStorage fs;
	try
	{	
		fs.open(routename,0);
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		FileStorage fcopys;
		fcopys.open(newroutename,0);
		FileNode t= fs["planelist"];
		int iter = t.end()-t.begin();
		for (iter= 0; iter < t.end()-t.begin();iter++)
		{
			string name = (string)t[iter]["planetype"];
			if (name == planetype)
			{
				planedata.plane_Type = name;
				planedata.cabin_Width = (float)t[iter]["cabin_Width"];
				planedata.wing_Width = (float)t[iter]["wing_Width"];
				planedata.head_Height = (float)t[iter]["head_Height"];
				planedata.head_wheel_HoriDis = (float)t[iter]["head_wheel_HoriDis"];
				planedata.engine_Centerdis = (float)t[iter]["engine_Centerdis"];
				planedata.engine_Radius = (float)t[iter]["engine_Radius"];
				planedata.center_floor_Dis = (float)t[iter]["center_floor_Dis"];
				planedata.wheel_width = (float)t[iter]["wheel_width"];
				planedata.wheel_height = (float)t[iter]["wheel_height"];
				planedata.engine_head_HoriDis = (float)t[iter]["engine_head_HoriDis"];

				vector<float>VhCF; 
				for (int it = 0; it <  t[iter]["horizontalCurveFactor"].end() - t[iter]["horizontalCurveFactor"].begin(); it++)
				{
					float x = (float)t[iter]["horizontalCurveFactor"][it]["data"];
					VhCF.push_back(x);
				}
				planedata.horizontalCurveFactor = VhCF;

				planedata.numHorizontalCurveFactor = (int) t[iter]["numHorizontalCurveFactor"];
				planedata.horizontalXMaxFromHead = (int) t[iter]["horizontalXMaxFromHead"];
				planedata.minAverage = (int) t[iter]["minAverage"];
				planedata.maxAverage = (int) t[iter]["maxAverage"];
				break;
			}		
		}
		if (iter == t.end()-t.begin())
		{
			cout<<"no this planetype data!"<<endl;
		}

		fcopys.release();
		return;// planedata;
	}
	try{
	FileNode t= fs["planelist"];
	int iter = t.end()-t.begin();
	for (iter= 0; iter < t.end()-t.begin();iter++)
	{
		string name = (string)t[iter]["planetype"];
		if (name == planetype)
		{
			planedata.plane_Type = name;
			planedata.cabin_Width = (float)t[iter]["cabin_Width"];
			planedata.wing_Width = (float)t[iter]["wing_Width"];
			planedata.head_Height = (float)t[iter]["head_Height"];
			planedata.head_wheel_HoriDis = (float)t[iter]["head_wheel_HoriDis"];
			planedata.engine_Centerdis = (float)t[iter]["engine_Centerdis"];
			planedata.engine_Radius = (float)t[iter]["engine_Radius"];
			planedata.center_floor_Dis = (float)t[iter]["center_floor_Dis"];
			planedata.wheel_width = (float)t[iter]["wheel_width"];
			planedata.wheel_height = (float)t[iter]["wheel_height"];
			planedata.engine_head_HoriDis = (float)t[iter]["engine_head_HoriDis"];
			

			vector<float>VhCF; 
			for (int it = 0; it <  t[iter]["horizontalCurveFactor"].end() - t[iter]["horizontalCurveFactor"].begin(); it++)
			{
				float x = (float)t[iter]["horizontalCurveFactor"][it]["data"];
				VhCF.push_back(x);
			}
			planedata.horizontalCurveFactor = VhCF;

			planedata.numHorizontalCurveFactor = (int) t[iter]["numHorizontalCurveFactor"];
			planedata.horizontalXMaxFromHead = (int) t[iter]["horizontalXMaxFromHead"];
			planedata.minAverage = (int) t[iter]["minAverage"];
			planedata.maxAverage = (int) t[iter]["maxAverage"];
			break;
		}		
	}
	if (iter == t.end()-t.begin())
	{
		cout<<"no this planetype data!"<<endl;
	}
	}
	catch(std::exception &e)
	{

	}
	fs.release();
	return;// planedata;
}


//写机型数据
void Document::writedata(string routename,string planename,PlaneInfo planetypeinfo)//写机型数据
{
	/*m_allnodedata alldata = readallplaneinfodata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;*/

	//VVstrings(Vstringsname, Vstrings);

	VPlaneInfos = readallplaneinfodata(routename);
	Vplaneinfos(planename,planetypeinfo);

	int iter;
	int it;
	FileStorage fsw(routename, FileStorage::WRITE);

	fsw << "planelist"<<"[:";
	for (iter = 0; iter< VPlaneInfos.end() - VPlaneInfos.begin(); iter++ )
	{
		fsw<<"{"<<"planetype"<<VPlaneInfos.at(iter).plane_Type
			<<"cabin_Width"<<VPlaneInfos.at(iter).cabin_Width
			<<"wing_Width"<<VPlaneInfos.at(iter).wing_Width
			<<"head_Height"<<VPlaneInfos.at(iter).head_Height
			<<"head_wheel_HoriDis"<<VPlaneInfos.at(iter).head_wheel_HoriDis
			<<"engine_Centerdis"<<VPlaneInfos.at(iter).engine_Centerdis
			<<"engine_Radius"<<VPlaneInfos.at(iter).engine_Radius
			<<"center_floor_Dis"<<VPlaneInfos.at(iter).center_floor_Dis
			<<"wheel_width"<<VPlaneInfos.at(iter).wheel_width
			<<"wheel_height"<<VPlaneInfos.at(iter).wheel_height
			<<"engine_head_HoriDis"<<VPlaneInfos.at(iter).engine_head_HoriDis
			
			<<"numHorizontalCurveFactor"<<VPlaneInfos.at(iter).numHorizontalCurveFactor
			<<"horizontalXMaxFromHead"<<VPlaneInfos.at(iter).horizontalXMaxFromHead
			<<"minAverage"<<VPlaneInfos.at(iter).minAverage
			<<"maxAverage"<<VPlaneInfos.at(iter).maxAverage

			<<"horizontalCurveFactor"<<"[";

		for (it = 0; it< VPlaneInfos.at(iter).horizontalCurveFactor.end() - VPlaneInfos.at(iter).horizontalCurveFactor.begin(); it++)
		{
			fsw<<"{"<<"data"<<VPlaneInfos.at(iter).horizontalCurveFactor.at(it) <<"}" ;
		}
		fsw<<"]"<<"}";
	}	
	fsw << "]";

	fsw.release();
}

//检查向量
void Document::Vplaneinfos(string planetype,PlaneInfo planetypeinfo)//检查向量
{
	vector<PlaneInfo>::iterator it = VPlaneInfos.begin();
	for (; it != VPlaneInfos.end(); it++)
	{
		string planename =   VPlaneInfos.at(it- VPlaneInfos.begin()).plane_Type;
		if (planename == planetype)//如果有重复的节点，则修改其数据，然后全部数据重新写入文档中
		{
			VPlaneInfos.at(it-VPlaneInfos.begin()).cabin_Width=  planetypeinfo.cabin_Width;
			VPlaneInfos.at(it-VPlaneInfos.begin()).wing_Width =  planetypeinfo.wing_Width;
			VPlaneInfos.at(it-VPlaneInfos.begin()).head_Height=  planetypeinfo.head_Height;
			VPlaneInfos.at(it-VPlaneInfos.begin()).head_wheel_HoriDis=  planetypeinfo.head_wheel_HoriDis;
			VPlaneInfos.at(it-VPlaneInfos.begin()).engine_Centerdis=  planetypeinfo.engine_Centerdis;
			VPlaneInfos.at(it-VPlaneInfos.begin()).engine_Radius=  planetypeinfo.engine_Radius;
			VPlaneInfos.at(it-VPlaneInfos.begin()).center_floor_Dis=  planetypeinfo.center_floor_Dis;
			VPlaneInfos.at(it-VPlaneInfos.begin()).wheel_width=  planetypeinfo.wheel_width;
			VPlaneInfos.at(it-VPlaneInfos.begin()).wheel_height=  planetypeinfo.wheel_height;
			VPlaneInfos.at(it-VPlaneInfos.begin()).engine_head_HoriDis=  planetypeinfo.engine_head_HoriDis;

			VPlaneInfos.at(it-VPlaneInfos.begin()).numHorizontalCurveFactor=  planetypeinfo.numHorizontalCurveFactor;
			VPlaneInfos.at(it-VPlaneInfos.begin()).horizontalXMaxFromHead=  planetypeinfo.horizontalXMaxFromHead;
			VPlaneInfos.at(it-VPlaneInfos.begin()).minAverage=  planetypeinfo.minAverage;
			VPlaneInfos.at(it-VPlaneInfos.begin()).maxAverage=  planetypeinfo.maxAverage;

			VPlaneInfos.at(it-VPlaneInfos.begin()).horizontalCurveFactor =  planetypeinfo.horizontalCurveFactor;
		
			break;
		}
	}

	if (it == VPlaneInfos.end())//如果不是重复的点则存入容器VPlaneInfos,然后全部数据重新写入文档中
	{
		/*m_Vstrings newVstrings;
		newVstrings.m_Vsname= vsname;
		newVstrings.Vstrings =  VPlaneInfos;*/

		VPlaneInfos.push_back(planetypeinfo);
	}

}

//读取所有已经写好的机型数据
vector<PlaneInfo> Document::readallplaneinfodata(string routename)
{
	vector<PlaneInfo> allplanetypedata;

	string newroutename = routename ;
	newroutename.insert(newroutename.length()-4,"copy");

	FileStorage fs;
	
	try
	{	
		fs.open(routename,0);//read模式
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"\nXML Exception!" ;
		fs.release();

		//同时，读备份文档
		FileStorage fcopy;
		fcopy.open(newroutename,0);//read模式打开备份文档
		FileNode rt = fcopy["planelist"];	
		for (int iter= 0; iter < rt.end()-rt.begin();iter++)
		{
			PlaneInfo planedata;
			vector<float>VhCF;
			planedata.plane_Type = (string)rt[iter]["planetype"];
			planedata.cabin_Width = (float)rt[iter]["cabin_Width"];
			planedata.wing_Width = (float)rt[iter]["wing_Width"];
			planedata.head_Height = (float)rt[iter]["head_Height"];
			planedata.head_wheel_HoriDis = (float)rt[iter]["head_wheel_HoriDis"];
			planedata.engine_Centerdis = (float)rt[iter]["engine_Centerdis"];
			planedata.engine_Radius = (float)rt[iter]["engine_Radius"];
			planedata.center_floor_Dis = (float)rt[iter]["center_floor_Dis"];
			planedata.wheel_width = (float)rt[iter]["wheel_width"];
			planedata.wheel_height = (float)rt[iter]["wheel_height"];
			planedata.engine_head_HoriDis = (float)rt[iter]["engine_head_HoriDis"];

			for (int it = 0; it <  rt[iter]["horizontalCurveFactor"].end() - rt[iter]["horizontalCurveFactor"].begin(); it++)
			{
				float x = (float)rt[iter]["horizontalCurveFactor"][it]["data"];
				VhCF.push_back(x);
			}
			planedata.horizontalCurveFactor = VhCF;

			planedata.numHorizontalCurveFactor = (int) rt[iter]["numHorizontalCurveFactor"];
			planedata.horizontalXMaxFromHead = (int) rt[iter]["horizontalXMaxFromHead"];
			planedata.minAverage = (int) rt[iter]["minAverage"];
			planedata.maxAverage = (int) rt[iter]["maxAverage"];

			allplanetypedata.push_back(planedata);
		}

		fcopy.release();
		return allplanetypedata;
	}

	//打开读成功了，首先进行读；然后存下这次读到的结果，存成备份文档
	//读
	FileNode rt = fs["planelist"];	
	for (int iter= 0; iter < rt.end()-rt.begin();iter++)
	{
		PlaneInfo planedata;
		vector<float>VhCF;
		planedata.plane_Type = (string)rt[iter]["planetype"];
		planedata.cabin_Width = (float)rt[iter]["cabin_Width"];
		planedata.wing_Width = (float)rt[iter]["wing_Width"];
		planedata.head_Height = (float)rt[iter]["head_Height"];
		planedata.head_wheel_HoriDis = (float)rt[iter]["head_wheel_HoriDis"];
		planedata.engine_Centerdis = (float)rt[iter]["engine_Centerdis"];
		planedata.engine_Radius = (float)rt[iter]["engine_Radius"];
		planedata.center_floor_Dis = (float)rt[iter]["center_floor_Dis"];
		planedata.wheel_width = (float)rt[iter]["wheel_width"];
		planedata.wheel_height = (float)rt[iter]["wheel_height"];
		planedata.engine_head_HoriDis = (float)rt[iter]["engine_head_HoriDis"];

		for (int it = 0; it <  rt[iter]["horizontalCurveFactor"].end() - rt[iter]["horizontalCurveFactor"].begin(); it++)
		{
			float x = (float)rt[iter]["horizontalCurveFactor"][it]["data"];
			VhCF.push_back(x);
		}
		planedata.horizontalCurveFactor = VhCF;

		planedata.numHorizontalCurveFactor = (int) rt[iter]["numHorizontalCurveFactor"];
		planedata.horizontalXMaxFromHead = (int) rt[iter]["horizontalXMaxFromHead"];
		planedata.minAverage = (int) rt[iter]["minAverage"];
		planedata.maxAverage = (int) rt[iter]["maxAverage"];

		allplanetypedata.push_back(planedata);
	}

	fs.release();

	////////////////将上述读的数据存入备份文档//////////////////////////
	int iter;
	int it;
	FileStorage fsw(newroutename, FileStorage::WRITE);

	fsw << "planelist"<<"[:";
	for (iter = 0; iter< allplanetypedata.end() - allplanetypedata.begin(); iter++ )
	{
		fsw<<"{"<<"planetype"<<allplanetypedata.at(iter).plane_Type
			<<"cabin_Width"<<allplanetypedata.at(iter).cabin_Width
			<<"wing_Width"<<allplanetypedata.at(iter).wing_Width
			<<"head_Height"<<allplanetypedata.at(iter).head_Height
			<<"head_wheel_HoriDis"<<allplanetypedata.at(iter).head_wheel_HoriDis
			<<"engine_Centerdis"<<allplanetypedata.at(iter).engine_Centerdis
			<<"engine_Radius"<<allplanetypedata.at(iter).engine_Radius
			<<"center_floor_Dis"<<allplanetypedata.at(iter).center_floor_Dis
			<<"wheel_width"<<allplanetypedata.at(iter).wheel_width
			<<"wheel_height"<<allplanetypedata.at(iter).wheel_height
			<<"engine_head_HoriDis"<<allplanetypedata.at(iter).engine_head_HoriDis
			
			<<"numHorizontalCurveFactor"<<allplanetypedata.at(iter).numHorizontalCurveFactor
			<<"horizontalXMaxFromHead"<<allplanetypedata.at(iter).horizontalXMaxFromHead
			<<"minAverage"<<allplanetypedata.at(iter).minAverage
			<<"maxAverage"<<allplanetypedata.at(iter).maxAverage

			<<"horizontalCurveFactor"<<"[";

		for (it = 0; it< allplanetypedata.at(iter).horizontalCurveFactor.end() - allplanetypedata.at(iter).horizontalCurveFactor.begin(); it++)
		{
			fsw<<"{"<<"data"<<allplanetypedata.at(iter).horizontalCurveFactor.at(it) <<"}" ;
		}
		fsw<<"]"<<"}";
	}	
	fsw << "]";

	fsw.release();

	return allplanetypedata;
}

//由全部机型数据向量读某个机型数据，输入要读出的机型名
void Document::readplanedata(string planetype, PlaneInfo& planedata)//读机型数据
{
	//string newroutename = routename ;
	//newroutename.insert(newroutename.length()-4,"copy");

	//PlaneInfo planedata;
	int iter;

	for (iter = 0; iter < VPlaneInfos.end()-VPlaneInfos.begin(); iter++)
	{
		string name = VPlaneInfos.at(iter).plane_Type;
		if (name == planetype)
		{
			planedata.plane_Type = name;
			planedata.cabin_Width = VPlaneInfos.at(iter).cabin_Width;
			planedata.wing_Width = VPlaneInfos.at(iter).wing_Width;
			planedata.head_Height = VPlaneInfos.at(iter).head_Height;
			planedata.head_wheel_HoriDis = VPlaneInfos.at(iter).head_wheel_HoriDis;
			planedata.engine_Centerdis = VPlaneInfos.at(iter).engine_Centerdis ;
			planedata.engine_Radius = VPlaneInfos.at(iter).engine_Radius;
			planedata.center_floor_Dis = VPlaneInfos.at(iter).center_floor_Dis;
			planedata.wheel_width = VPlaneInfos.at(iter).wheel_width ;
			planedata.wheel_height = VPlaneInfos.at(iter).wheel_height;
			planedata.engine_head_HoriDis = VPlaneInfos.at(iter).engine_head_HoriDis;

			planedata.horizontalCurveFactor = VPlaneInfos.at(iter).horizontalCurveFactor;

			planedata.numHorizontalCurveFactor = VPlaneInfos.at(iter).numHorizontalCurveFactor;
			planedata.horizontalXMaxFromHead = VPlaneInfos.at(iter).horizontalXMaxFromHead;
			planedata.minAverage = VPlaneInfos.at(iter).minAverage;
			planedata.maxAverage = VPlaneInfos.at(iter).maxAverage;
			break;
		}		
	}
	if (iter ==  VPlaneInfos.end()-VPlaneInfos.begin())
	{
		cout<<"no this planetype data!"<<endl;
	}

	return;// planedata;
}

//写所有参数数据
void Document::writealldata(string routename)
{
	int iter;
	int it;
	FileStorage fsw(routename, FileStorage::WRITE);

	//////////实际点与图像坐标////////////
	fsw << "VDisandPointslist"<<"[:";
	for (iter = 0; iter< VorientationVPs.end()-VorientationVPs.begin(); iter++ )
	{
		fsw<<"{"<<"Vectorname"<<VorientationVPs.at(iter).point_name<<"points"<<"[";
		for (it = 0; it< VorientationVPs.at(iter).VLinePoints.end() - VorientationVPs.at(iter).VLinePoints.begin(); it++)
		{
			fsw <<"{"<<"distance"<<VorientationVPs.at(iter).VLinePoints.at(it).first
				<<"x" <<VorientationVPs.at(iter).VLinePoints.at(it).second.x
				<< "y" << VorientationVPs.at(iter).VLinePoints.at(it).second.y<<"}";
		}
		fsw<<"]"<<"}";
	}	
	fsw << "]";

	///////////停止线，引导线上的机型信息//////
	fsw << "Vstringslist"<<"[:";
	for (iter = 0; iter< VsceneVSs.end()-VsceneVSs.begin(); iter++ )
	{
		stringstream str;
		string vPointnum;
		str<<iter;
		str>>vPointnum;

		fsw<<"{"<<"Vectorname"<<VsceneVSs.at(iter).m_Vsname<<"strings"<<"[";
		for (int it = 0; it< VsceneVSs.at(iter).Vstrings.end() - VsceneVSs.at(iter).Vstrings.begin(); it++)
		{
			fsw <<"{"<< "stringname" <<VsceneVSs.at(iter).Vstrings.at(it)<<"}";
		}
		fsw<<"]"<<"}";
	}	
	fsw << "]";

	///////////////////////////////////////////////////
	fsw << "Vpointslist"<<"[:";
	for (iter = 0; iter< VsceneVPs.end()-VsceneVPs.begin(); iter++ )
	{
		stringstream str;
		string vPointnum;
		str<<iter;
		str>>vPointnum;

		fsw<<"{"<<"Vectorname"<<VsceneVPs.at(iter).m_Vpname<<"points"<<"[";
		//fsw<<"{"<<VsceneVPs.at(iter).m_Vpname<<"{";
		//fsw<<"{";
		for (it = 0; it< VsceneVPs.at(iter).Vpoints.end() - VsceneVPs.at(iter).Vpoints.begin(); it++)
		{
			//int f = Vpoints.end()- Vpoints.begin();
			stringstream str;
			string Pointnum;
			str<<it;
			str>>Pointnum;
			//fsw << Pointnum <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
			fsw <<"{"<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
			//fsw <<"{"<< "pointnum"<< it+1<< "x" <<VsceneVPs.at(iter).Vpoints.at(it).x<< "y" << VsceneVPs.at(iter).Vpoints.at(it).y<<"}";
		}
		fsw<<"]"<<"}";
	}	
	fsw << "]";

	/////////////////单独点的参数设置信息/////////////////////////////////
	fsw <<"alonexlist"<< "[:";
	for (iter= 0; iter < VsceneXs.end()-VsceneXs.begin();iter++)//将数据全部写入文档
	{

		fsw <<"{"<< "name" <<VsceneXs.at(iter).m_name<< "x" << VsceneXs.at(iter).m_x<< "}";

	}
	fsw << "]";

	////////////////区域框设置信息///////////////////////////////////////////////////////
	fsw <<"rectlist"<< "[:";
	for (iter= 0; iter < VsceneRs.end()-VsceneRs.begin();iter++)//将矩形框数据全部写入文档
	{

		fsw <<"{"<< "name" <<VsceneRs.at(iter).rect_name  << "x" << VsceneRs.at(iter).m_r.x<< "y" << VsceneRs.at(iter).m_r.y
			<<"height"<<VsceneRs.at(iter).m_r.height<<"width"<<VsceneRs.at(iter).m_r.width<< "}";

	}
	fsw << "]";

	/////////////////停止线，引导线的起止点信息//////////////////////////////
	fsw <<"pointlist"<< "[:";
	for (iter= 0; iter < VscenePs.end()-VscenePs.begin();iter++)//将点数据全部写入文档
	{
		//contiwrite(routename,VscenePs.at(iter).point_name,VscenePs.at(iter));
		//fsw<<VscenePs.at(iter).point_name;
		fsw <<"{"<<"name" <<VscenePs.at(iter).point_name 
			<< "x" << VscenePs.at(iter).m_p.x<< "y" << VscenePs.at(iter).m_p.y << "}";

	}
	fsw << "]";

	//激光参数
	fsw << "laser_datas_list"<<"[:";
	for (iter = 0; iter< VLaserParameters.end()-VLaserParameters.begin(); iter++ )
	{
		fsw<<"{"<<"guideline_stopline_name"<<VLaserParameters.at(iter).guideline_stopline_name
			<<"object_dis"<<VLaserParameters.at(iter).object_dis
			<<"guide_line_k"<<VLaserParameters.at(iter).guide_line_k
			<<"guide_line_b"<<VLaserParameters.at(iter).guide_line_b
			<<"guide_line_x"<<VLaserParameters.at(iter).guide_line_x
			<<"guide_line_y"<<VLaserParameters.at(iter).guide_line_y
			<<"offset_angle"<<VLaserParameters.at(iter).offset_angle;
		fsw<<"}";
	}	
	fsw << "]";

	//存视觉检测与跟踪参数及上位机等信息GuideLineWidth
	fsw << "vision_machine_datas_list";//<<"[:";
	fsw<<"{"<<"CheckVisionEngineNum"<<V_M_Datas.CheckVisionEngineNum
		<<"CheckVisionEngineGap"<<V_M_Datas.CheckVisionEngineGap
	    <<"MaxReportDistance"<<V_M_Datas.MaxReportDistance
		<<"GuideLineWidth"<<V_M_Datas.GuideLineWidth
		<<"BlackestJudgeStart"<<V_M_Datas.BlackestJudgeStart
		<<"BlackestJudgeStep"<<V_M_Datas.BlackestJudgeStep
		<<"CircleJudgeStart"<<V_M_Datas.CircleJudgeStart
		<<"CircleJudgeStep"<<V_M_Datas.CircleJudgeStep
		<<"maxIterTimes"<<V_M_Datas.maxIterTimes
		<<"MinEngineRadius"<<V_M_Datas.MinEngineRadius
		<<"MinEnginePerimeter"<<V_M_Datas.MinEnginePerimeter
		<<"MinTemplateScale"<<V_M_Datas.MinTemplateScale
		<<"MaxTemplateScale"<<V_M_Datas.MaxTemplateScale
		<<"MinMatchRate"<<V_M_Datas.MinMatchRate
		<<"WheelDetectParamX"<<V_M_Datas.WheelDetectParamX
		<<"WheelDetectParamY"<<V_M_Datas.WheelDetectParamY
		<<"WheelRectHeight"<<V_M_Datas.WheelRectHeight
		<<"CutDownThresh"<<V_M_Datas.CutDownThresh
		<<"OriginalScale"<<V_M_Datas.OriginalScale
		<<"MaxOffset"<<V_M_Datas.MaxOffset;
	fsw<<"}";	

	//存激光信息
	fsw << "laser_machine_datas_list";//<<"[:";
	fsw<<"{"<<"install_high"<<L_M_Datas.install_high
		<<"HrzConfirmSteps"<<L_M_Datas.HrzConfirmSteps
		<<"VertConfirmSteps"<<L_M_Datas.VertConfirmSteps
		<<"Q12Steps"<<L_M_Datas.Q12Steps
		<<"HrzOffsetAngle"<<L_M_Datas.HrzOffsetAngle
		<<"HrzConfirmProbability"<<L_M_Datas.HrzConfirmProbability
		<<"VertConfirmProbability"<<L_M_Datas.VertConfirmProbability
		<<"WalkTestDistance"<<L_M_Datas.WalkTestDistance
		<<"LaserForwardOffset"<<L_M_Datas.LaserForwardOffset
		<<"LaserBackwardOffset"<<L_M_Datas.LaserBackwardOffset
		<<"VerifyEngine"<<L_M_Datas.VerifyEngine
		<<"VerifyPlaneType"<<L_M_Datas.VerifyPlaneType
		<<"RightLeftEngine"<<L_M_Datas.RightLeftEngine
		<<"NeedYardScan"<<L_M_Datas.NeedYardScan
		<<"NeedBridgeScan"<<L_M_Datas.NeedBridgeScan
		<<"RegularVertical"<<L_M_Datas.RegularVertical
		<<"RegularVerticalTime"<<L_M_Datas.RegularVerticalTime
		<<"AccurateGuideDis"<<L_M_Datas.AccurateGuideDis
		<<"yard_scan_range"<<L_M_Datas.yard_scan_range
		<<"yard_scan_obstacle_high"<<L_M_Datas.yard_scan_obstacle_high
		<<"yard_scan_obstacle_width"<<L_M_Datas.yard_scan_obstacle_width
		<<"VerifyHeadHighDis"<<L_M_Datas.VerifyHeadHighDis
		<<"VerifyHeadHighMin"<<L_M_Datas.VerifyHeadHighMin
		<<"VerifyHeadHighMax"<<L_M_Datas.VerifyHeadHighMax
		<<"VerifyEngineDis"<<L_M_Datas.VerifyEngineDis
		<<"EngineHeadDisMin"<<L_M_Datas.EngineHeadDisMin
		<<"ScanEngineTimes"<<L_M_Datas.ScanEngineTimes
		<<"TowContinu"<<L_M_Datas.TowContinu
		<<"VerifyEngineWidthMin"<<L_M_Datas.VerifyEngineWidthMin
		<<"VerifyEngineWidthMax"<<L_M_Datas.VerifyEngineWidthMax
		<<"VerifyEngineCabinDisMin"<<L_M_Datas.VerifyEngineCabinDisMin
		<<"VerifyEngineCabinDisMax"<<L_M_Datas.VerifyEngineCabinDisMax
		<<"VerifyEngineHeadDisMin"<<L_M_Datas.VerifyEngineHeadDisMin
		<<"VerifyEngineHeadDisMax"<<L_M_Datas.VerifyEngineHeadDisMax
		<<"NeedShieldObscur"<<L_M_Datas.NeedShieldObscur
		<<"Y_Obscur"<<L_M_Datas.Y_Obscur
		<<"X_Obscur_Left"<<L_M_Datas.X_Obscur_Left
		<<"X_Obscur_Right"<<L_M_Datas.X_Obscur_Right
		<<"PlaneLeaveHighMin"<<L_M_Datas.PlaneLeaveHighMin
		<<"PlaneLeaveHighMax"<<L_M_Datas.PlaneLeaveHighMax
		<<"PlaneLeaveYMin"<<L_M_Datas.PlaneLeaveYMin
		<<"PlaneLeaveYMax"<<L_M_Datas.PlaneLeaveYMax
		<<"PlaneLeaveZMin"<<L_M_Datas.PlaneLeaveZMin
		<<"PlaneLeaveConfirmPoint"<<L_M_Datas.PlaneLeaveConfirmPoint
		<<"PlaneLeaveConfirmLine"<<L_M_Datas.PlaneLeaveConfirmLine
		<<"SuspectMode"<<L_M_Datas.SuspectMode
		<<"SuspectHotMax"<<L_M_Datas.SuspectHotMax
		<<"SuspectRowMin"<<L_M_Datas.SuspectRowMin
		<<"SuspectSMNumberMin"<<L_M_Datas.SuspectSMNumberMin
		<<"MaxDistance"<<L_M_Datas.MaxDistance
		<<"CatchDisMin"<<L_M_Datas.CatchDisMin
		<<"CatchDisMax"<<L_M_Datas.CatchDisMax
		<<"ObjectHighMin"<<L_M_Datas.ObjectHighMin
		<<"ObjectHighMax"<<L_M_Datas.ObjectHighMax
		<<"CarHighMax"<<L_M_Datas.CarHighMax
		<<"ObjectDisMin"<<L_M_Datas.ObjectDisMin
		<<"ObjectDisMax"<<L_M_Datas.ObjectDisMax
		<<"WidthMaxRatio"<<L_M_Datas.WidthMaxRatio
		<<"WidthMeasureRatio"<<L_M_Datas.WidthMeasureRatio
		<<"NeighborDifferMax"<<L_M_Datas.NeighborDifferMax
		<<"PlanePointNumberMin"<<L_M_Datas.PlanePointNumberMin
		<<"BirdgePointNumberMin"<<L_M_Datas.BirdgePointNumberMin
		<<"MaxGap"<<L_M_Datas.MaxGap
		<<"AlwayMoveMax"<<L_M_Datas.AlwayMoveMax
		<<"OfflineSimulaMode"<<L_M_Datas.OfflineSimulaMode
		<<"SaveDataWhenOffline"<<L_M_Datas.SaveDataWhenOffline
		<<"OfflineSleepTime"<<L_M_Datas.OfflineSleepTime;
	fsw<<"}";
	
	//存系统数据
	fsw << "system_parameter";//<<"[:";
	fsw<<"{"<<"RunningMode"<<V_S_Datas.RunningMode
		<<"StationNumber"<<V_S_Datas.StationNumber 
		<<"ServerAddress"<<V_S_Datas.ServerAddress
		<<"ServerPort"<<V_S_Datas.ServerPort
		<<"LogSaveRoot"<<V_S_Datas.LogSaveRoot
		<<"LogPreserveDays"<<V_S_Datas.LogPreserveDays
		<<"LEDCOM"<<V_S_Datas.LEDCOM
		<<"LEDPAUD"<<V_S_Datas.LEDPAUD
		<<"MODBUSCOM"<<V_S_Datas.MODBUSCOM
		<<"MODBUSPAUD"<<V_S_Datas.MODBUSPAUD
		<<"LASERCOM"<<V_S_Datas.LASERCOM
		<<"LASERPAUD"<<V_S_Datas.LASERPAUD
		<<"DIPCOM"<<V_S_Datas.DIPCOM
		<<"DIPPAUD"<<V_S_Datas.DIPPAUD
		
		//<<"DistanceWeight"<<V_M_Datas.DistanceWeight
		//<<"DriftWeight"<<V_M_Datas.DriftWeight
		//<<"DriftBound"<<V_M_Datas.DriftBound
		//<<"SpeedBound"<<V_M_Datas.SpeedBound
		<<"NeedPlaneLeaveScan"<<V_S_Datas.NeedPlaneLeaveScan
		<<"AccuracyGuideDistance"<<V_S_Datas.AccuracyGuideDistance
		<<"DriftDistance"<<V_S_Datas.DriftDistance
		<<"DriftRemove"<<V_S_Datas.DriftRemove
		<<"OverSpeed24"<<V_S_Datas.OverSpeed24
		<<"OverSpeed24Remove"<<V_S_Datas.OverSpeed24Remove
		<<"OverSpeed"<<V_S_Datas.OverSpeed
		<<"OverSpeedRemove"<<V_S_Datas.OverSpeedRemove
		<<"ValidateDistance"<<V_S_Datas.ValidateDistance
		<<"DriftWeightStart"<<V_S_Datas.DriftWeightStart
		<<"DriftWeightEnd"<<V_S_Datas.DriftWeightEnd
		<<"DistanceWeightStart"<<V_S_Datas.DistanceWeightStart
		<<"DistanceWeightEnd"<<V_S_Datas.DistanceWeightEnd
		<<"SpeedWeightStart"<<V_S_Datas.SpeedWeightStart
		<<"SpeedWeightEnd"<<V_S_Datas.SpeedWeightEnd
		<<"SlowDistance"<<V_S_Datas.SlowDistance
		<<"DriftGain"<<V_S_Datas.DriftGain
		<<"DistanceGain"<<V_S_Datas.DistanceGain
		<<"MinDriftDistance"<<V_S_Datas.MinDriftDistance  
		<<"MaxStopDistance"<<V_S_Datas.MaxStopDistance
		<<"TooFarDistance"<<V_S_Datas.TooFarDistance
		<<"SubsystemErrorTime"<<V_S_Datas.SubsystemErrorTime
		<<"LEDDelayTime"<<V_S_Datas.LEDDelayTime
		<<"ModbusDelayTime"<<V_S_Datas.ModbusDelayTime
		<<"BoardInterlockPort"<<V_S_Datas.BoardInterlockPort
		<<"EmergencyStopPort"<<V_S_Datas.EmergencyStopPort
		<<"AutoSleepTime"<<V_S_Datas.AutoSleepTime
		<<"StopDisplayTime"<<V_S_Datas.StopDisplayTime
		<<"ChockOnDisplayTime"<<V_S_Datas.ChockOnDisplayTime
		<<"MaxSpeed"<<V_S_Datas.MaxSpeed
		<<"MaxSpeedRemove"<<V_S_Datas.MaxSpeedRemove
		<<"StopToOKTime"<<V_S_Datas.StopToOKTime
		<<"SpeedSmoothWindow"<<V_S_Datas.SpeedSmoothWindow
		<<"PBBAllow"<<V_S_Datas.PBBAllow
		<<"PCAAllow"<<V_S_Datas.PCAAllow
		<<"FGPAllow"<<V_S_Datas.FGPAllow
		<<"PCAPort"<<V_S_Datas.PCAPort
		<<"FGPPort"<<V_S_Datas.FGPPort
		<<"Q12Port"<<V_S_Datas.Q12Port
		<<"UPSPort"<<V_S_Datas.UPSPort
		<<"LEDCOMERRPort"<<V_S_Datas.LEDCOMERRPort
		<<"LEDERRPort"<<V_S_Datas.LEDERRPort
		<<"DiskSpaceAlarm" <<V_S_Datas.DiskSpaceAlarm
		<<"TaskMode" <<V_S_Datas.TaskMode
		<<"CheckLeaveMode" <<V_S_Datas.CheckLeaveMode
		<<"RestartInterval" <<V_S_Datas.RestartInterval
		<<"RestartTime" <<V_S_Datas.RestartTime
		<<"CheckLeaveTime" <<V_S_Datas.CheckLeaveTime
		<<"SaveKeyImage" <<V_S_Datas.SaveKeyImage
		<<"MaxAngleBiasX" <<V_S_Datas.MaxAngleBiasX
		<<"MaxAngleBiasY" <<V_S_Datas.MaxAngleBiasY
	    <<"MaxAngleShakeX" <<V_S_Datas.MaxAngleShakeX
		<<"MaxAngleShakeY" <<V_S_Datas.MaxAngleShakeY;

	fsw<<"}";

	fsw.release();
}


////按引导线停止线编号写激光场景定义参数数据
void Document::writedata(string routename,string gl_sl_name,laser_parameters newlaser_datas)
{
	m_allnodedata alldata = readalldata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;
	VLaserParameters = alldata.VLPs;
	V_M_Datas = alldata.V_M_datas;
	V_S_Datas = alldata.V_S_datas;
	L_M_Datas = alldata.L_M_datas;

	VLaserdatas(gl_sl_name,newlaser_datas);

	writealldata(routename);

	/*int iter;
	int it;
	FileStorage fsw(routename, FileStorage::WRITE);

	fsw << "laser_datas_list"<<"[:";
	for (iter = 0; iter< VLaserParameters.end()-VLaserParameters.begin(); iter++ )
	{
		fsw<<"{"<<"guideline_stopline_name"<<VLaserParameters.at(iter).guideline_stopline_name
			<<"install_high"<<VLaserParameters.at(iter).install_high
			<<"object_dis"<<VLaserParameters.at(iter).object_dis
			<<"guide_line_k"<<VLaserParameters.at(iter).guide_line_k
			<<"guide_line_b"<<VLaserParameters.at(iter).guide_line_b
			<<"guide_line_x"<<VLaserParameters.at(iter).guide_line_x
			<<"guide_line_y"<<VLaserParameters.at(iter).guide_line_y
			<<"HrzConfirmSteps"<<VLaserParameters.at(iter).HrzConfirmSteps
			<<"VertConfirmSteps"<<VLaserParameters.at(iter).VertConfirmSteps;
		fsw<<"}";
	}	
	fsw << "]";

	fsw.release();*/
}

//检查激光场景定义参数向量
void  Document::VLaserdatas(string gl_sl_name,laser_parameters newlaserdatas)
{
	//vector<laser_parameters>::iterator it = VLaserParameters.begin();
	int it;
	for (it = 0; it != VLaserParameters.end()-VLaserParameters.begin(); it++)
	{
		string glslname =  VLaserParameters.at(it).guideline_stopline_name;
		if (gl_sl_name == glslname)//如果有重复的节点，则修改其数据，然后全部数据重新写入文档中
		{
			VLaserParameters.at(it).guideline_stopline_name = newlaserdatas.guideline_stopline_name;
			VLaserParameters.at(it).object_dis = newlaserdatas.object_dis;
			VLaserParameters.at(it).guide_line_k = newlaserdatas.guide_line_k;
			VLaserParameters.at(it).guide_line_b = newlaserdatas.guide_line_b;
			VLaserParameters.at(it).guide_line_x = newlaserdatas.guide_line_x;
			VLaserParameters.at(it).guide_line_y = newlaserdatas.guide_line_y;
			VLaserParameters.at(it).offset_angle = newlaserdatas.offset_angle;
			break;
		}
	}

	if (it == VLaserParameters.end()-VLaserParameters.begin())//如果不是重复的点则存
	{
		VLaserParameters.push_back(newlaserdatas);
	}
}

//通过停止线引导线编号读激光参数
laser_parameters Document::readlaserdatas(string routename, string glname, string slname)
{
	string newroutename = routename;
	newroutename.insert(newroutename.length()-4,"copy");

	int gl_len = glname.length();
	int sl_len = slname.length();
	string gl_sl_name;
	if ((sl_len == 5)&&(gl_len == 6))
	{
		gl_sl_name = "Laser" + glname.substr(5,1) + slname.substr(4,1) ;
	}
	else 
	{
		gl_sl_name = "";
		cout<<"not correct guideline and stopline information!"<<endl;
	}
	laser_parameters gl_sl_laserdatas;

	FileStorage fs;
	try
	{	
		if (!fs.open(routename,0))
			;
		else
			;
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		FileStorage fcopy;
		fcopy.open(newroutename,0);//read模式打开备份文档
		FileNode t= fcopy["laser_datas_list"];
		int iter = t.end()-t.begin();
		for (iter= 0; iter < t.end()-t.begin();iter++)
		{
			string name = (string)t[iter]["guideline_stopline_name"];
			if (gl_sl_name == name)
			{
				gl_sl_laserdatas.guideline_stopline_name = name;
				gl_sl_laserdatas.object_dis = (float)t[iter]["object_dis"];
				gl_sl_laserdatas.guide_line_k = (float)t[iter]["guide_line_k"];
				gl_sl_laserdatas.guide_line_b= (float)t[iter]["guide_line_b"];
				gl_sl_laserdatas.guide_line_x = (float)t[iter]["guide_line_x"];
				gl_sl_laserdatas.guide_line_y = (float)t[iter]["guide_line_y"];
				gl_sl_laserdatas.offset_angle = (float)t[iter]["offset_angle"];
				break;
			}		
		}
		if (iter == t.end()-t.begin())
		{
			cout<<"no this gl_sl_laserdata !"<<endl;
		}

		fcopy.release();
		return gl_sl_laserdatas;
	}

	FileNode t= fs["laser_datas_list"];
	int iter = t.end()-t.begin();
	for (iter= 0; iter < t.end()-t.begin();iter++)
	{
		string name = (string)t[iter]["guideline_stopline_name"];
		if (gl_sl_name == name)
		{
			gl_sl_laserdatas.guideline_stopline_name = name;
			gl_sl_laserdatas.object_dis = (float)t[iter]["object_dis"];
			gl_sl_laserdatas.guide_line_k = (float)t[iter]["guide_line_k"];
			gl_sl_laserdatas.guide_line_b= (float)t[iter]["guide_line_b"];
			gl_sl_laserdatas.guide_line_x = (float)t[iter]["guide_line_x"];
			gl_sl_laserdatas.guide_line_y = (float)t[iter]["guide_line_y"];
			gl_sl_laserdatas.offset_angle = (float)t[iter]["offset_angle"];
			break;
		}		
	}
	if (iter == t.end()-t.begin())
	{
		cout<<"no this gl_sl_laserdata !"<<endl;
	}

	fs.release();
	return gl_sl_laserdatas;
}

/////////////////视觉参数读写/////////////////////////
void Document::writedata(string routename,vision_machine_parameters vison_datas)//写参数
{
	m_allnodedata alldata = readalldata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;
	VLaserParameters = alldata.VLPs;
	//V_M_Datas = alldata.V_M_datas;
	V_S_Datas = alldata.V_S_datas;
	L_M_Datas = alldata.L_M_datas;

	V_M_Datas = vison_datas;

	writealldata(routename);

	//FileStorage fsw(routename, FileStorage::WRITE);
	//fsw << "vision_machine_datas_list";//<<"[:";
	//fsw<<"{"<<"ServerAddress"<<vison_datas.ServerAddress
	//		<<"ServerPort"<<vison_datas.ServerPort
	//		<<"LogSaveRoot"<<vison_datas.LogSaveRoot

	//		<<"DistanceWeight"<<vison_datas.DistanceWeight
	//      <<"GuideLineWidth"<<V_M_Datas.GuideLineWidth
	//		<<"BlackestJudgeStart"<<vison_datas.BlackestJudgeStart
	//		<<"BlackestJudgeStep"<<vison_datas.BlackestJudgeStep
	//		<<"CircleJudgeStart"<<vison_datas.CircleJudgeStart
	//		<<"CircleJudgeStep"<<vison_datas.CircleJudgeStep
	//		<<"maxIterTimes"<<vison_datas.maxIterTimes
	//		<<"MinEngineRadius"<<vison_datas.MinEngineRadius
	//		<<"MinEnginePerimeter"<<vison_datas.MinEnginePerimeter
	//		<<"MinTemplateScale"<<vison_datas.MinTemplateScale
	//		<<"MaxTemplateScale"<<vison_datas.MaxTemplateScale
	//		<<"MinMatchRate"<<vison_datas.MinMatchRate
	//		<<"WheelDetectParamX"<<vison_datas.WheelDetectParamX
	//		<<"WheelDetectParamY"<<vison_datas.WheelDetectParamY
	//		<<"WheelRectHeight"<<vison_datas.WheelRectHeight
	//		<<"CutDownThresh"<<vison_datas.CutDownThresh
	//		<<"OriginalScale"<<vison_datas.OriginalScale
	//		<<"MaxOffset"<<vison_datas.MaxOffset;
	//	fsw<<"}";	
	////fsw << "]";
	//fsw.release();
}
vision_machine_parameters Document::readvisiondatas(string routename)//读参数
{
	string newroutename = routename;
	newroutename.insert(newroutename.length()-4,"copy");

	vision_machine_parameters v_m_datas;

	FileStorage fs;
	try
	{	
		fs.open(routename,0);
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		FileStorage fcopy;
		fcopy.open(newroutename,0);//read模式打开备份文档
		FileNode t= fcopy["vision_machine_datas_list"];

		v_m_datas.CheckVisionEngineNum = (int)t["CheckVisionEngineNum"];
		v_m_datas.CheckVisionEngineGap = (int)t["CheckVisionEngineGap"];
		v_m_datas.MaxReportDistance = (float)t["MaxReportDistance"];
		v_m_datas.GuideLineWidth = (float)t["GuideLineWidth"];
		v_m_datas.BlackestJudgeStart = (float)t["BlackestJudgeStart"];
		v_m_datas.BlackestJudgeStep = (float)t["BlackestJudgeStep"];
		v_m_datas.CircleJudgeStart = (float)t["CircleJudgeStart"];
		v_m_datas.CircleJudgeStep = (float)t["CircleJudgeStep"];
		v_m_datas.maxIterTimes = (float)t["maxIterTimes"];
		v_m_datas.MinEngineRadius = (float)t["MinEngineRadius"];
		v_m_datas.MinEnginePerimeter = (float)t["MinEnginePerimeter"];
		v_m_datas.MinTemplateScale = (float)t["MinTemplateScale"];
		v_m_datas.MaxTemplateScale = (float)t["MaxTemplateScale"];
		v_m_datas.MinMatchRate = (float)t["MinMatchRate"];
		v_m_datas.WheelDetectParamX = (float)t["WheelDetectParamX"];
		v_m_datas.WheelDetectParamY = (float)t["WheelDetectParamY"];
		v_m_datas.WheelRectHeight = (float)t["WheelRectHeight"];
		v_m_datas.CutDownThresh = (float)t["CutDownThresh"];
		v_m_datas.OriginalScale = (float)t["OriginalScale"];
		v_m_datas.MaxOffset = (float)t["MaxOffset"];

		fcopy.release();
		return v_m_datas;
	}

	FileNode t= fs["vision_machine_datas_list"];

	v_m_datas.CheckVisionEngineNum = (int)t["CheckVisionEngineNum"];
	v_m_datas.CheckVisionEngineGap = (int)t["CheckVisionEngineGap"];
	v_m_datas.MaxReportDistance = (float)t["MaxReportDistance"];
	v_m_datas.GuideLineWidth = (float)t["GuideLineWidth"];
	v_m_datas.BlackestJudgeStart = (float)t["BlackestJudgeStart"];
	v_m_datas.BlackestJudgeStep = (float)t["BlackestJudgeStep"];
	v_m_datas.CircleJudgeStart = (float)t["CircleJudgeStart"];
	v_m_datas.CircleJudgeStep = (float)t["CircleJudgeStep"];
	v_m_datas.maxIterTimes = (float)t["maxIterTimes"];
	v_m_datas.MinEngineRadius = (float)t["MinEngineRadius"];
	v_m_datas.MinEnginePerimeter = (float)t["MinEnginePerimeter"];
	v_m_datas.MinTemplateScale = (float)t["MinTemplateScale"];
	v_m_datas.MaxTemplateScale = (float)t["MaxTemplateScale"];
	v_m_datas.MinMatchRate = (float)t["MinMatchRate"];
	v_m_datas.WheelDetectParamX = (float)t["WheelDetectParamX"];
	v_m_datas.WheelDetectParamY = (float)t["WheelDetectParamY"];
	v_m_datas.WheelRectHeight = (float)t["WheelRectHeight"];
	v_m_datas.CutDownThresh = (float)t["CutDownThresh"];
	v_m_datas.OriginalScale = (float)t["OriginalScale"];
	v_m_datas.MaxOffset = (float)t["MaxOffset"];

	fs.release();
	return v_m_datas;
}

/////////////////激光参数读写/////////////////////////
void Document::writedata(string routename,laser_machine_parameters laser_datas)//写参数
{
	m_allnodedata alldata = readalldata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;
	VLaserParameters = alldata.VLPs;
	//V_M_Datas = alldata.V_M_datas;
	V_S_Datas = alldata.V_S_datas;

	V_M_Datas = alldata.V_M_datas;
	L_M_Datas = laser_datas;

	writealldata(routename);
}
laser_machine_parameters Document::readlaserparamters(string routename)//读参数
{
	string newroutename = routename;
	newroutename.insert(newroutename.length()-4,"copy");

	laser_machine_parameters l_m_datas;

	FileStorage fs;
	try
	{	
		fs.open(routename,0);
	}
	catch(std::exception e)
	{
		//WRITE_LOG(e.what());
		cout<<routename<<"XML Exception!" ;
		fs.release();
		FileStorage fcopy;
		fcopy.open(newroutename,0);//read模式打开备份文档
		FileNode t= fcopy["laser_machine_datas_list"];

		l_m_datas.install_high = (float)t["install_high"];
		l_m_datas.HrzConfirmSteps = (int)t["HrzConfirmSteps"];
		l_m_datas.VertConfirmSteps = (int)t["VertConfirmSteps"];
		l_m_datas.Q12Steps = (int)t["Q12Steps"];
		l_m_datas.HrzOffsetAngle = (float)t["HrzOffsetAngle"];
		l_m_datas.HrzConfirmProbability = (float)t["HrzConfirmProbability"];
		l_m_datas.VertConfirmProbability = (float)t["VertConfirmProbability"];
		l_m_datas.WalkTestDistance = (int)t["WalkTestDistance"];
		l_m_datas.LaserForwardOffset = (int)t["LaserForwardOffset"];
		l_m_datas.LaserBackwardOffset = (int)t["LaserBackwardOffset"];
		l_m_datas.VerifyEngine = (int)t["VerifyEngine"];
		l_m_datas.VerifyPlaneType = (int)t["VerifyPlaneType"];
		l_m_datas.RightLeftEngine = (int)t["RightLeftEngine"];
		l_m_datas.NeedYardScan = (int)t["NeedYardScan"];
		l_m_datas.NeedBridgeScan = (int)t["NeedBridgeScan"];
		l_m_datas.RegularVertical = (int)t["RegularVertical"];
		l_m_datas.RegularVerticalTime = (int)t["RegularVerticalTime"];
		l_m_datas.AccurateGuideDis = (int)t["AccurateGuideDis"];
		l_m_datas.yard_scan_range = (int)t["yard_scan_range"];
		l_m_datas.yard_scan_obstacle_high = (int)t["yard_scan_obstacle_high"];
		l_m_datas.yard_scan_obstacle_width = (int)t["yard_scan_obstacle_width"];
		l_m_datas.VerifyHeadHighDis = (int)t["VerifyHeadHighDis"];
		l_m_datas.VerifyHeadHighMin = (int)t["VerifyHeadHighMin"];
		l_m_datas.VerifyHeadHighMax = (int)t["VerifyHeadHighMax"];
		l_m_datas.VerifyEngineDis = (int)t["VerifyEngineDis"];
		l_m_datas.EngineHeadDisMin = (int)t["EngineHeadDisMin"];
		l_m_datas.ScanEngineTimes = (int)t["ScanEngineTimes"];
		l_m_datas.TowContinu = (int)t["TowContinu"];
		l_m_datas.VerifyEngineWidthMin = (int)t["VerifyEngineWidthMin"];
		l_m_datas.VerifyEngineWidthMax = (int)t["VerifyEngineWidthMax"];
		l_m_datas.VerifyEngineCabinDisMin = (int)t["VerifyEngineCabinDisMin"];
		l_m_datas.VerifyEngineCabinDisMax = (int)t["VerifyEngineCabinDisMax"];
		l_m_datas.VerifyEngineHeadDisMin = (int)t["VerifyEngineHeadDisMin"];
		l_m_datas.VerifyEngineHeadDisMax = (int)t["VerifyEngineHeadDisMax"];
		l_m_datas.NeedShieldObscur = (int)t["NeedShieldObscur"];
		l_m_datas.Y_Obscur = (int)t["Y_Obscur"];
		l_m_datas.X_Obscur_Left = (int)t["X_Obscur_Left"];
		l_m_datas.X_Obscur_Right = (int)t["X_Obscur_Right"];
		l_m_datas.PlaneLeaveHighMin = (int)t["PlaneLeaveHighMin"];
		l_m_datas.PlaneLeaveHighMax = (int)t["PlaneLeaveHighMax"];
		l_m_datas.PlaneLeaveYMin = (int)t["PlaneLeaveYMin"];
		l_m_datas.PlaneLeaveYMax = (int)t["PlaneLeaveYMax"];
		l_m_datas.PlaneLeaveZMin = (int)t["PlaneLeaveZMin"];
		l_m_datas.PlaneLeaveConfirmPoint = (int)t["PlaneLeaveConfirmPoint"];
		l_m_datas.PlaneLeaveConfirmLine = (int)t["PlaneLeaveConfirmLine"];
		l_m_datas.SuspectMode = (int)t["SuspectMode"];
		l_m_datas.SuspectHotMax = (int)t["SuspectHotMax"];
		l_m_datas.SuspectRowMin = (int)t["SuspectRowMin"];
		l_m_datas.SuspectSMNumberMin = (int)t["SuspectSMNumberMin"];
		l_m_datas.MaxDistance = (int)t["MaxDistance"];
		l_m_datas.CatchDisMin = (int)t["CatchDisMin"];
		l_m_datas.CatchDisMax = (int)t["CatchDisMax"];
		l_m_datas.ObjectHighMin = (int)t["ObjectHighMin"];
		l_m_datas.ObjectHighMax = (int)t["ObjectHighMax"];
		l_m_datas.CarHighMax = (int)t["CarHighMax"];
		l_m_datas.ObjectDisMin = (int)t["ObjectDisMin"];
		l_m_datas.ObjectDisMax = (int)t["ObjectDisMax"];
		l_m_datas.WidthMaxRatio = (float)t["WidthMaxRatio"];
		l_m_datas.WidthMeasureRatio = (float)t["WidthMeasureRatio"];
		l_m_datas.NeighborDifferMax = (int)t["NeighborDifferMax"];
		l_m_datas.PlanePointNumberMin = (int)t["PlanePointNumberMin"];
		l_m_datas.BirdgePointNumberMin = (int)t["BirdgePointNumberMin"];
		l_m_datas.MaxGap = (int)t["MaxGap"];
		l_m_datas.AlwayMoveMax = (int)t["AlwayMoveMax"];
		l_m_datas.OfflineSimulaMode = (int)t["OfflineSimulaMode"];
		l_m_datas.SaveDataWhenOffline = (int)t["SaveDataWhenOffline"];
		l_m_datas.OfflineSleepTime = (int)t["OfflineSleepTime"];
		fcopy.release();
		return l_m_datas;
	}

	FileNode t= fs["laser_machine_datas_list"];

	l_m_datas.install_high = (float)t["install_high"];
	l_m_datas.HrzConfirmSteps = (int)t["HrzConfirmSteps"];
	l_m_datas.VertConfirmSteps = (int)t["VertConfirmSteps"];
	l_m_datas.Q12Steps = (int)t["Q12Steps"];
	l_m_datas.HrzOffsetAngle = (float)t["HrzOffsetAngle"];
	l_m_datas.HrzConfirmProbability = (float)t["HrzConfirmProbability"];
	l_m_datas.VertConfirmProbability = (float)t["VertConfirmProbability"];
	l_m_datas.WalkTestDistance = (int)t["WalkTestDistance"];
	l_m_datas.LaserForwardOffset = (int)t["LaserForwardOffset"];
	l_m_datas.LaserBackwardOffset = (int)t["LaserBackwardOffset"];
	l_m_datas.VerifyEngine = (int)t["VerifyEngine"];
	l_m_datas.VerifyPlaneType = (int)t["VerifyPlaneType"];
	l_m_datas.RightLeftEngine = (int)t["RightLeftEngine"];
	l_m_datas.NeedYardScan = (int)t["NeedYardScan"];
	l_m_datas.NeedBridgeScan = (int)t["NeedBridgeScan"];
	l_m_datas.RegularVertical = (int)t["RegularVertical"];
	l_m_datas.RegularVerticalTime = (int)t["RegularVerticalTime"];
	l_m_datas.AccurateGuideDis = (int)t["AccurateGuideDis"];
	l_m_datas.yard_scan_range = (int)t["yard_scan_range"];
	l_m_datas.yard_scan_obstacle_high = (int)t["yard_scan_obstacle_high"];
	l_m_datas.yard_scan_obstacle_width = (int)t["yard_scan_obstacle_width"];
	l_m_datas.VerifyHeadHighDis = (int)t["VerifyHeadHighDis"];
	l_m_datas.VerifyHeadHighMin = (int)t["VerifyHeadHighMin"];
	l_m_datas.VerifyHeadHighMax = (int)t["VerifyHeadHighMax"];
	l_m_datas.VerifyEngineDis = (int)t["VerifyEngineDis"];
	l_m_datas.EngineHeadDisMin = (int)t["EngineHeadDisMin"];
	l_m_datas.ScanEngineTimes = (int)t["ScanEngineTimes"];
	l_m_datas.TowContinu = (int)t["TowContinu"];
	l_m_datas.VerifyEngineWidthMin = (int)t["VerifyEngineWidthMin"];
	l_m_datas.VerifyEngineWidthMax = (int)t["VerifyEngineWidthMax"];
	l_m_datas.VerifyEngineCabinDisMin = (int)t["VerifyEngineCabinDisMin"];
	l_m_datas.VerifyEngineCabinDisMax = (int)t["VerifyEngineCabinDisMax"];
	l_m_datas.VerifyEngineHeadDisMin = (int)t["VerifyEngineHeadDisMin"];
	l_m_datas.VerifyEngineHeadDisMax = (int)t["VerifyEngineHeadDisMax"];
	l_m_datas.NeedShieldObscur = (int)t["NeedShieldObscur"];
	l_m_datas.Y_Obscur = (int)t["Y_Obscur"];
	l_m_datas.X_Obscur_Left = (int)t["X_Obscur_Left"];
	l_m_datas.X_Obscur_Right = (int)t["X_Obscur_Right"];
	l_m_datas.PlaneLeaveHighMin = (int)t["PlaneLeaveHighMin"];
	l_m_datas.PlaneLeaveHighMax = (int)t["PlaneLeaveHighMax"];
	l_m_datas.PlaneLeaveYMin = (int)t["PlaneLeaveYMin"];
	l_m_datas.PlaneLeaveYMax = (int)t["PlaneLeaveYMax"];
	l_m_datas.PlaneLeaveZMin = (int)t["PlaneLeaveZMin"];
	l_m_datas.PlaneLeaveConfirmPoint = (int)t["PlaneLeaveConfirmPoint"];
	l_m_datas.PlaneLeaveConfirmLine = (int)t["PlaneLeaveConfirmLine"];
	l_m_datas.SuspectMode = (int)t["SuspectMode"];
	l_m_datas.SuspectHotMax = (int)t["SuspectHotMax"];
	l_m_datas.SuspectRowMin = (int)t["SuspectRowMin"];
	l_m_datas.SuspectSMNumberMin = (int)t["SuspectSMNumberMin"];
	l_m_datas.MaxDistance = (int)t["MaxDistance"];
	l_m_datas.CatchDisMin = (int)t["CatchDisMin"];
	l_m_datas.CatchDisMax = (int)t["CatchDisMax"];
	l_m_datas.ObjectHighMin = (int)t["ObjectHighMin"];
	l_m_datas.ObjectHighMax = (int)t["ObjectHighMax"];
	l_m_datas.CarHighMax = (int)t["CarHighMax"];
	l_m_datas.ObjectDisMin = (int)t["ObjectDisMin"];
	l_m_datas.ObjectDisMax = (int)t["ObjectDisMax"];
	l_m_datas.WidthMaxRatio = (float)t["WidthMaxRatio"];
	l_m_datas.WidthMeasureRatio = (float)t["WidthMeasureRatio"];
	l_m_datas.NeighborDifferMax = (int)t["NeighborDifferMax"];
	l_m_datas.PlanePointNumberMin = (int)t["PlanePointNumberMin"];
	l_m_datas.BirdgePointNumberMin = (int)t["BirdgePointNumberMin"];
	l_m_datas.MaxGap = (int)t["MaxGap"];
	l_m_datas.AlwayMoveMax = (int)t["AlwayMoveMax"];
	l_m_datas.OfflineSimulaMode = (int)t["OfflineSimulaMode"];
	l_m_datas.SaveDataWhenOffline = (int)t["SaveDataWhenOffline"];
	l_m_datas.OfflineSleepTime = (int)t["OfflineSleepTime"];
	fs.release();
	return l_m_datas;
}

void Document::writedata(string routename,system_parameters system_datas)//写参数
{
	m_allnodedata alldata = readalldata(routename);
	VsceneXs = alldata.xs;
	VscenePs = alldata.points;
	VsceneRs = alldata.rects;
	VsceneVPs = alldata.VPs;
	VsceneVSs = alldata.VSs;
	VorientationVPs = alldata.VOPs;
	VLaserParameters = alldata.VLPs;
	V_M_Datas = alldata.V_M_datas;
	L_M_Datas = alldata.L_M_datas;

	V_S_Datas = system_datas;

	writealldata(routename);

	//FileStorage fsw(routename, FileStorage::WRITE);
	//fsw << "vision_machine_datas_list";//<<"[:";
	//fsw<<"{"<<"ServerAddress"<<vison_datas.ServerAddress
	//		<<"ServerPort"<<vison_datas.ServerPort
	//		<<"LogSaveRoot"<<vison_datas.LogSaveRoot

	//		<<"DistanceWeight"<<vison_datas.DistanceWeight

	//		<<"BlackestJudgeStart"<<vison_datas.BlackestJudgeStart
	//		<<"BlackestJudgeStep"<<vison_datas.BlackestJudgeStep
	//		<<"CircleJudgeStart"<<vison_datas.CircleJudgeStart
	//		<<"CircleJudgeStep"<<vison_datas.CircleJudgeStep
	//		<<"maxIterTimes"<<vison_datas.maxIterTimes
	//		<<"MinEngineRadius"<<vison_datas.MinEngineRadius
	//		<<"MinEnginePerimeter"<<vison_datas.MinEnginePerimeter
	//		<<"MinTemplateScale"<<vison_datas.MinTemplateScale
	//		<<"MaxTemplateScale"<<vison_datas.MaxTemplateScale
	//		<<"MinMatchRate"<<vison_datas.MinMatchRate
	//		<<"WheelDetectParamX"<<vison_datas.WheelDetectParamX
	//		<<"WheelDetectParamY"<<vison_datas.WheelDetectParamY
	//		<<"WheelRectHeight"<<vison_datas.WheelRectHeight
	//		<<"CutDownThresh"<<vison_datas.CutDownThresh
	//		<<"OriginalScale"<<vison_datas.OriginalScale
	//		<<"MaxOffset"<<vison_datas.MaxOffset;
	//	fsw<<"}";	
	////fsw << "]";
	//fsw.release();
}
system_parameters Document::readsystemdatas(string routename)//读参数
{
	string newroutename = routename;
	newroutename.insert(newroutename.length()-4,"copy");

	system_parameters v_s_datas;

	FileStorage fs;
	try
	{	
		fs.open(routename,0);
	}
	catch(std::exception e)
	{
		TRACE(e.what());
		//LOG(WARNING)<<routename<<"XML Exception!"<<e.what() ;
		fs.release();
		FileStorage fcopy;
		fcopy.open(newroutename,0);//read模式打开备份文档
		FileNode t= fcopy["system_parameter"];

		v_s_datas.RunningMode = (string)t["RunningMode"];
		v_s_datas.StationNumber = (string)t["StationNumber"];
		v_s_datas.ServerAddress = (string)t["ServerAddress"];
		v_s_datas.ServerPort = (string)t["ServerPort"];
		v_s_datas.LogSaveRoot = (string)t["LogSaveRoot"];
		v_s_datas.LogPreserveDays = (double)t["LogPreserveDays"];
		v_s_datas.LEDCOM = (string)t["LEDCOM"];
		v_s_datas.LEDPAUD = (int)t["LEDPAUD"];
		v_s_datas.MODBUSCOM = (string)t["MODBUSCOM"];
		v_s_datas.MODBUSPAUD = (int)t["MODBUSPAUD"];
		v_s_datas.LASERCOM = (string)t["LASERCOM"];
		v_s_datas.LASERPAUD = (int)t["LASERPAUD"];
		v_s_datas.DIPCOM = (string)t["DIPCOM"];
		v_s_datas.DIPPAUD = (int)t["DIPPAUD"];

		v_s_datas.NeedPlaneLeaveScan = (double)t["NeedPlaneLeaveScan"];
		v_s_datas.AccuracyGuideDistance = (double)t["AccuracyGuideDistance"];
		v_s_datas.DriftDistance = (double)t["DriftDistance"];
		v_s_datas.DriftRemove = (double)t["DriftRemove"];
		v_s_datas.OverSpeed24 = (double)t["OverSpeed24"];
		v_s_datas.OverSpeed24Remove = (double)t["OverSpeed24Remove"];
		v_s_datas.OverSpeed = (double)t["OverSpeed"];
		v_s_datas.OverSpeedRemove = (double)t["OverSpeedRemove"];
		v_s_datas.ValidateDistance = (double)t["ValidateDistance"];
		v_s_datas.DriftWeightStart = (double)t["DriftWeightStart"];
		v_s_datas.DriftWeightEnd = (double)t["DriftWeightEnd"];
		v_s_datas.DistanceWeightStart = (double)t["DistanceWeightStart"];
		v_s_datas.DistanceWeightEnd = (double)t["DistanceWeightEnd"];
		v_s_datas.SpeedWeightStart = (double)t["SpeedWeightStart"];
		v_s_datas.SpeedWeightEnd = (double)t["SpeedWeightEnd"];
		v_s_datas.SlowDistance = (double)t["SlowDistance"];
		v_s_datas.DriftGain = (double)t["DriftGain"];
		v_s_datas.DistanceGain = (double)t["DistanceGain"];
		v_s_datas.MinDriftDistance = (double)t["MinDriftDistance"];
		v_s_datas.MaxStopDistance = (double)t["MaxStopDistance"];
		v_s_datas.TooFarDistance = (double)t["TooFarDistance"];
		v_s_datas.SubsystemErrorTime = (double)t["SubsystemErrorTime"];
		v_s_datas.LEDDelayTime = (double)t["LEDDelayTime"];
		v_s_datas.ModbusDelayTime = (double)t["ModbusDelayTime"];
		v_s_datas.BoardInterlockPort = (int)t["BoardInterlockPort"];
		v_s_datas.EmergencyStopPort = (int)t["EmergencyStopPort"];
		v_s_datas.AutoSleepTime = (double)t["AutoSleepTime"];
		v_s_datas.StopDisplayTime = (double)t["StopDisplayTime"];
		v_s_datas.ChockOnDisplayTime = (double)t["ChockOnDisplayTime"];
		v_s_datas.MaxSpeed = (double)t["MaxSpeed"];
		v_s_datas.MaxSpeedRemove = (double)t["MaxSpeedRemove"];
		v_s_datas.StopToOKTime = (double)t["StopToOKTime"];
		v_s_datas.SpeedSmoothWindow = (double)t["SpeedSmoothWindow"];
		v_s_datas.PBBAllow = (int)t["PBBAllow"];
		v_s_datas.PCAAllow = (int)t["PCAAllow"];
		v_s_datas.FGPAllow = (int)t["FGPAllow"];
		v_s_datas.PCAPort = (int)t["PCAPort"];
		v_s_datas.FGPPort = (int)t["FGPPort"];
		v_s_datas.Q12Port = (int)t["Q12Port"];
		v_s_datas.UPSPort = (int)t["UPSPort"];
		v_s_datas.LEDCOMERRPort = (int)t["LEDCOMERRPort"];
		v_s_datas.LEDERRPort = (int)t["LEDERRPort"];
		v_s_datas.DiskSpaceAlarm = (double)t["DiskSpaceAlarm"];
		v_s_datas.TaskMode = (int)t["TaskMode"];
		v_s_datas.SaveKeyImage = (int)t["SaveKeyImage"];
		v_s_datas.CheckLeaveMode = (int)t["CheckLeaveMode"];
		v_s_datas.RestartInterval = (int)t["RestartInterval"];
		v_s_datas.RestartTime = (int)t["RestartTime"];
		v_s_datas.CheckLeaveTime = (double)t["CheckLeaveTime"];
		v_s_datas.MaxAngleShakeX = (double)t["MaxAngleShakeX"];
		v_s_datas.MaxAngleShakeY = (double)t["MaxAngleShakeY"];
		v_s_datas.MaxAngleBiasX = (double)t["MaxAngleBiasX"];
		v_s_datas.MaxAngleBiasY = (double)t["MaxAngleBiasY"];

		fcopy.release();
		return v_s_datas;
	}

	FileNode t= fs["system_parameter"];

	v_s_datas.RunningMode = (string)t["RunningMode"];
	v_s_datas.StationNumber = (string)t["StationNumber"];
	v_s_datas.ServerAddress = (string)t["ServerAddress"];
	v_s_datas.ServerPort = (string)t["ServerPort"];
	v_s_datas.LogSaveRoot = (string)t["LogSaveRoot"];
	v_s_datas.LogPreserveDays = (double)t["LogPreserveDays"];
	v_s_datas.LEDCOM = (string)t["LEDCOM"];
	v_s_datas.LEDPAUD = (int)t["LEDPAUD"];
	v_s_datas.MODBUSCOM = (string)t["MODBUSCOM"];
	v_s_datas.MODBUSPAUD = (int)t["MODBUSPAUD"];
	v_s_datas.LASERCOM = (string)t["LASERCOM"];
	v_s_datas.LASERPAUD = (int)t["LASERPAUD"];
	v_s_datas.DIPCOM = (string)t["DIPCOM"];
	v_s_datas.DIPPAUD = (int)t["DIPPAUD"];

	v_s_datas.NeedPlaneLeaveScan = (double)t["NeedPlaneLeaveScan"];
	v_s_datas.AccuracyGuideDistance = (double)t["AccuracyGuideDistance"];
	v_s_datas.DriftDistance = (double)t["DriftDistance"];
	v_s_datas.DriftRemove = (double)t["DriftRemove"];
	v_s_datas.OverSpeed24 = (double)t["OverSpeed24"];
	v_s_datas.OverSpeed24Remove = (double)t["OverSpeed24Remove"];
	v_s_datas.OverSpeed = (double)t["OverSpeed"];
	v_s_datas.OverSpeedRemove = (double)t["OverSpeedRemove"];
	v_s_datas.ValidateDistance = (double)t["ValidateDistance"];
	v_s_datas.DriftWeightStart = (double)t["DriftWeightStart"];
	v_s_datas.DriftWeightEnd = (double)t["DriftWeightEnd"];
	v_s_datas.DistanceWeightStart = (double)t["DistanceWeightStart"];
	v_s_datas.DistanceWeightEnd = (double)t["DistanceWeightEnd"];
	v_s_datas.SpeedWeightStart = (double)t["SpeedWeightStart"];
	v_s_datas.SpeedWeightEnd = (double)t["SpeedWeightEnd"];
	v_s_datas.SlowDistance = (double)t["SlowDistance"];
	v_s_datas.DriftGain = (double)t["DriftGain"];
	v_s_datas.DistanceGain = (double)t["DistanceGain"];
	v_s_datas.MinDriftDistance = (double)t["MinDriftDistance"];
	v_s_datas.MaxStopDistance = (double)t["MaxStopDistance"];
	v_s_datas.TooFarDistance = (double)t["TooFarDistance"];
	v_s_datas.SubsystemErrorTime = (double)t["SubsystemErrorTime"];
	v_s_datas.LEDDelayTime = (double)t["LEDDelayTime"];
	v_s_datas.ModbusDelayTime = (double)t["ModbusDelayTime"];
	v_s_datas.BoardInterlockPort = (int)t["BoardInterlockPort"];
	v_s_datas.EmergencyStopPort = (int)t["EmergencyStopPort"];
	v_s_datas.AutoSleepTime = (double)t["AutoSleepTime"];
	v_s_datas.StopDisplayTime = (double)t["StopDisplayTime"];
	v_s_datas.ChockOnDisplayTime = (double)t["ChockOnDisplayTime"];
	v_s_datas.MaxSpeed = (double)t["MaxSpeed"];
	v_s_datas.MaxSpeedRemove = (double)t["MaxSpeedRemove"];
	v_s_datas.StopToOKTime = (double)t["StopToOKTime"];
	v_s_datas.SpeedSmoothWindow = (double)t["SpeedSmoothWindow"];
	v_s_datas.PBBAllow = (int)t["PBBAllow"];
	v_s_datas.PCAAllow = (int)t["PCAAllow"];
	v_s_datas.FGPAllow = (int)t["FGPAllow"];
	v_s_datas.PCAPort = (int)t["PCAPort"];
	v_s_datas.FGPPort = (int)t["FGPPort"];
	v_s_datas.Q12Port = (int)t["Q12Port"];
	v_s_datas.UPSPort = (int)t["UPSPort"];
	v_s_datas.LEDCOMERRPort = (int)t["LEDCOMERRPort"];
	v_s_datas.LEDERRPort = (int)t["LEDERRPort"];
	v_s_datas.DiskSpaceAlarm = (double)t["DiskSpaceAlarm"];
	v_s_datas.TaskMode = (int)t["TaskMode"];
	v_s_datas.SaveKeyImage = (int)t["SaveKeyImage"];
	v_s_datas.CheckLeaveMode = (int)t["CheckLeaveMode"];
	v_s_datas.RestartInterval = (int)t["RestartInterval"];
	v_s_datas.RestartTime = (int)t["RestartTime"];
	v_s_datas.CheckLeaveTime = (double)t["CheckLeaveTime"];
	v_s_datas.MaxAngleShakeX = (double)t["MaxAngleShakeX"];
	v_s_datas.MaxAngleShakeY = (double)t["MaxAngleShakeY"];
	v_s_datas.MaxAngleBiasX = (double)t["MaxAngleBiasX"];
	v_s_datas.MaxAngleBiasY = (double)t["MaxAngleBiasY"];

	fs.release();
	return v_s_datas;
}