#pragma once

#ifndef LOADDXF_H
#define LOADDXF_H

#include "dl_creationadapter.h"

//����߶�������
typedef struct  
{
	double x;
	double y;
	double z;
}PointStruct;

//*************************************************************************************************
class CLoadDXF : public DL_CreationAdapter
{
public:
	CLoadDXF();
	~CLoadDXF();
	virtual void addLayer(const DL_LayerData& data);
	virtual void addLine(const DL_LineData& data);
	virtual void addArc(const DL_ArcData& data);
	virtual void addCircle(const DL_CircleData& data);
	virtual void addPolyline(const DL_PolylineData& data);
	virtual void addVertex(const DL_VertexData& data);

	//RGBת������  
	COLORREF LoadColorreF(int Index);
	//��ȡͼ����ɫ
	COLORREF GetLayerColor(CString Layer);
	//ȷ��ͼ���Ƿ����أ��õ�Delete
	BOOL GetLayerDelete(CString Layer);

private:
	int j;
	int Fill;     //����߶�ȡ��
	int Numble;
	PointStruct* m_PointList;
};

#endif
//******************************************************************************************************

//ͼ��Ԫ�ػ���
class CDraw : public CObject
{
protected:
	CString m_Layer;
	COLORREF m_Color;
	BOOL b_Delete;
	virtual void Serialize(CArchive& ar); 
public:
	CDraw(){}
	CDraw(CString Layer,COLORREF Color,BOOL Delete)
	{
		m_Layer = Layer;
		m_Color = Color;
		b_Delete = Delete;
	}
	//����ͼ���麯��
	virtual void Draw(CDC *pDC,int m_DrawMode,int m_DrawMode1,COLORREF BackColor) = 0;
	//ͼ��Ԫ�ر߽���μ����麯��
	virtual void GetRect(double *minX,double *minY,double *maxX,double *maxY) = 0;
	//����㵽ֱ�ߵľ���ĺ���
	double PointLine(double xx,double yy,double x1,double y1,double x2,double y2);
	//���������ľ���ĺ���
	double CalDisp(double x1,double y1,double x2,double y2);
	//�жϵ��Ƿ���һ������������еĺ���
	BOOL PointRgn(double x,double y,int Numble,PointStruct *PointList,double blc);
	//�ж��Ƿ񱻵�ѡ�еĺ���
	virtual BOOL IsPoint(double x,double y,double j1,double blc) = 0;	
	//ͼ�ε���ת
	void RotatePoint (double baseX , double baseY , double angle , double *xx, double *yy);
	//ͼ�ε���ת�еĺ���
	double GetAngle (double baseX , double baseY , double xx , double yy);
	//Ϊ��ʵ�ָ���ͼ��Ԫ�صļ���,������麯��
	virtual void Rotate (double baseX , double baseY , double angle) = 0;
	//ͼ�������е��Ӻ���
	void ScalePoint (double x_Base , double y_Base , double x_Scale , double y_Scale , double* xx , double* yy);
	//����ͼ��Ԫ�ط��������Ĵ��麯��
	virtual void Scale (double x_Base , double y_Base , double x_Scale , double y_Scale ) = 0;
	//����ͼ���ƶ����麯��
	virtual void Move (double x_Move , double y_Move) = 0;
	//����Ϊ�˼���ֱ�߶κ�ֱ�߶��ཻ��ϵ�ĺ���
	int LineLine(double m_X1 , double m_Y1 , double m_X2 , double m_Y2, double XXX1 , double YYY1 , double XXX2 , double YYY2);
	//����ֱ�߶�������ཻ
	BOOL LineRan(double xx1 , double yy1 , double xx2 , double yy2,PointStruct * pPoint,double blc);
	//��������������ཻ
	BOOL PLineRan(int PointNunb,PointStruct * pPLine, PointStruct * pPoint,double blc);
	//�����ͼ��Ԫ���ǲ����ھ���������----�������
	BOOL PointRgnRec(double x ,double y, PointStruct *PointList,double blc);
	//��ѡͼ��Ԫ���жϵ��麯��
	virtual BOOL IsRectangle(PointStruct *pPoint ,double blc) = 0;
	//��������Ͽ�ѡ��ͼ��
	virtual BOOL IsLayerCombox(CString Lyaer) =0 ;
	//Ԫ�ض������ɫ����
	virtual void BlueBox(CDC* pDC)=0;

	virtual ~CDraw(){}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//ͼ����
class CLayer : public CObject 
{
protected:
	DECLARE_SERIAL(CLayer);
	CString m_Layer;
	COLORREF m_Color;
	BOOL b_Delete;
public:
	CLayer(){}
	CLayer(CString Layer,COLORREF Color,BOOL Delete)
	{
		m_Layer = Layer;
		m_Color = Color;
		b_Delete = Delete;
	}
	virtual void Serialize(CArchive& ar);
};

//ֱ����////////////////////////////////////////////////////////////////////////////////////////////////////////
class CLine :public CDraw
{
protected:
	DECLARE_SERIAL(CLine);
	double m_X1,m_X2,m_Y1,m_Y2;
public:
	CLine(){}
	CLine(CString Layer,COLORREF Color,BOOL Delete,double X1,double Y1,double X2,double Y2)
		:CDraw(Layer,Color,Delete)
	{
		m_X1 = X1;
		m_Y1 = Y1;
		m_X2 = X2;
		m_Y2 = Y2;
	}
	virtual void Draw(CDC *pDC,int m_DrawMode,int m_DrawMode1,COLORREF BackColor);
	virtual void GetRect(double *minX,double *minY,double *maxX,double *maxY);
	virtual void Serialize(CArchive& ar);
	virtual BOOL IsPoint(double x,double y,double j1,double blc);
	virtual void Rotate (double baseX , double baseY , double angle);
	virtual void Scale (double x_Base , double y_Base , double x_Scale , double y_Scale );
	virtual void Move (double x_Move , double y_Move) ;
	virtual BOOL IsRectangle(PointStruct *pPoint ,double blc) ;
	virtual BOOL IsLayerCombox(CString Lyaer);
	virtual void BlueBox(CDC* pDC);
};

//�����///////////////////////////////////////////////////////////////////////////////////////////////////
class CPline :public CDraw
{
protected:
	int m_Numble; //������
	BOOL b_Fill;//���߶α�־��1=�رգ�0=��
	PointStruct* m_PointList;//�洢�������������ָ��
	DECLARE_SERIAL(CPline);
public:
	CPline()
	{m_Numble = 0;}
	CPline(CString Layer,COLORREF Color,BOOL Delete,int Numble,PointStruct *PointList,BOOL Fill)
		:CDraw(Layer,Color,Delete)
	{
		m_Numble = Numble;
		b_Fill = Fill;
		m_PointList = new PointStruct[Numble+1];//��̬����洢��������Ľṹ����
		if (Numble > 0)                          //���������Ŀ����0
		{
			for (int i = 0 ;i < Numble ; i++)
				m_PointList[i] = PointList[i];
		}
	}

	virtual void Draw(CDC *pDC,int m_DrawMode,int m_DrawMode1,COLORREF BackColor);
	virtual void GetRect(double *minX,double *minY,double *maxX,double *maxY);
	virtual void Serialize(CArchive& ar);
	virtual BOOL IsPoint(double x,double y,double j1,double blc);	
	virtual void Rotate (double baseX , double baseY , double angle);
	virtual void Scale (double x_Base , double y_Base , double x_Scale , double y_Scale );
	virtual void Move (double x_Move , double y_Move) ;
	virtual BOOL IsRectangle(PointStruct *pPoint ,double blc) ;
	virtual BOOL IsLayerCombox(CString Lyaer);
	virtual void BlueBox(CDC* pDC);

	~CPline()
	{
		if (m_Numble>0)           //���������Ŀ����0��ɾ����̬����Ľṹ����
			delete m_PointList;
	}
};

//Բ��//////////////////////////////////////////////////////////////////////////////////////////////////////////
class CCircle:public CDraw
{
protected:
	double m_CirceX , m_CirceY , m_CirceR;
	DECLARE_SERIAL(CCircle);
public:
	CCircle(){}
	CCircle(CString Layer,COLORREF Color,BOOL Delete,double CirceX,double CirceY,double CirceR)
		:CDraw(Layer,Color,Delete)
	{
		m_CirceX = CirceX;
		m_CirceY = CirceY;
		m_CirceR = CirceR;
	}
	virtual void Draw(CDC *pDC,int m_DrawMode,int m_DrawMode1,COLORREF BackColor);
	virtual void GetRect(double *minX,double *minY,double *maxX,double *maxY);
	virtual void Serialize(CArchive& ar);
	virtual BOOL IsPoint(double x,double y,double j1,double blc);	
	virtual void Rotate (double baseX , double baseY , double angle);
	virtual void Scale (double x_Base , double y_Base , double x_Scale , double y_Scale );
	virtual void Move (double x_Move , double y_Move) ;
	virtual BOOL IsRectangle(PointStruct *pPoint ,double blc) ;
	virtual BOOL IsLayerCombox(CString Lyaer);
	virtual void BlueBox(CDC* pDC);
};

//������/////////////////////////////////////////////////////////////////////////////////////////////////////////
class CArc:public CCircle
{
protected:
	//Բ���������յ�Ƕȣ��û��ȱ�ʾ��
	double m_Angle1,m_Angle2;
	BOOL IsInArc(double angle);
	DECLARE_SERIAL(CArc);
public:
	CArc(){}
	CArc(CString Layer,COLORREF Color,BOOL Delete,double CirceX,double CirceY,double CirceR,double Angle1,double Angle2)
		:CCircle(Layer,Color,Delete,CirceX,CirceY,CirceR)
	{
		m_Angle1 = Angle1;
		m_Angle2 = Angle2;
	}
	virtual void Draw(CDC *pDC,int m_DrawMode,int m_DrawMode1,COLORREF BackColor);
	virtual void GetRect(double *minX,double *minY,double *maxX,double *maxY);
	virtual void Serialize(CArchive& ar);
	virtual BOOL IsPoint(double x,double y,double j1,double blc);	
	virtual void Rotate (double baseX , double baseY , double angle);
	virtual void Scale (double x_Base , double y_Base , double x_Scale , double y_Scale );
	//virtual void Move (double x_Move , double y_Move);
	//virtual BOOL IsRectangle(PointStruct *pPoint ,double blc) ;
	virtual BOOL IsLayerCombox(CString Lyaer);
	virtual void BlueBox(CDC* pDC);
};