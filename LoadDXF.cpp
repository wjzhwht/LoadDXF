#include "StdAfx.h"
#include "MainFrm.h"
#include "FMDoc.h"
#include "LoadDXF.h"

//****************************************************************************************************
extern void DPtoVP(double x,double y,int*X,int*Y);
extern void VPtoDP(int x,int y,double*X,double*Y);
extern int DLtoVL(double l);
extern double VLtoDL(int l);


//Ϊ������ػ��ٶȣ����Լӵ�ȫ�ֺ���
double xMinScreen,yMinScreen,xMaxScreen,yMaxScreen;
//Ϊ������ػ��ٶȣ����Լӵ�ȫ�ֺ���������Ϊ�˼�����Ļ�ı߽���Ԫ�ض���ı߽��ǲ����ཻ
BOOL IsRectCross(double minx,double miny,double maxx,double maxy);

//�������Ƿ�����Ļ�����ཻ
BOOL IsRectCross(double minx,double miny,double maxx,double maxy)
{
	if(minx>xMaxScreen||maxx<xMinScreen||miny>yMaxScreen||maxy<yMinScreen) //�������������ཻ
		return FALSE; //�粻�ཻ��������0
	else 
		return TRUE; //����ཻ�ͷ���1
}



//Ĭ�Ϲ��캯��
CLoadDXF::CLoadDXF()
{
	j = 0;
	Fill = -1;
	Numble = 0;
}
//Ĭ����������
CLoadDXF::~CLoadDXF()
{

}


//��ȡͼ�������
void CLoadDXF::addLayer(const DL_LayerData& data)
{
	CMainFrame* pFrame=(CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CFMDoc *pDoc=(CFMDoc *)pFrame->GetActiveDocument();
	CString Layer ;
	COLORREF Color;
	BOOL Delete;
	int nTemp;
	//�õ�ͼ������
	Layer = data.name.c_str();
	//�õ���ɫ����
	nTemp = attributes.getColor();  
	if ( nTemp < 0)
	{
		Delete = TRUE;   //���С��0��˵�����ͼ�������ص�
	} 
	else
	{
		Delete = FALSE;  //������ڻ����0��˵�����ͼ����Ҫ��ʾ��
	}
	//����ɫ����ת��ΪRGB
	Color = LoadColorreF(nTemp);    
	//���DeleteΪFALSE�����õ���ͼ�����װ��Array
	if (!Delete)  
	{
	  pDoc->AddLayer(Layer,Color,Delete); 
	}
	//����������ǻ����ͻὫCAD�����е����ݶ�ȡ��������������������
	//pDoc->AddLayer(Layer,Color,Delete); 
}

//��ȡֱ�ߵ�����
void CLoadDXF::addLine(const DL_LineData& data)
{
	CMainFrame* pFrame=(CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CFMDoc *pDoc=(CFMDoc *)pFrame->GetActiveDocument();
	CString Layer ;
	COLORREF Color;
	BOOL Delete;

	//�õ�ֱ�ߵ���ĩ������
	double X1 = data.x1;
	double Y1 = data.y1;
	double X2 = data.x2;
	double Y2 = data.y2;

	//�õ�ͼ������
	Layer = attributes.getLayer().c_str();
	//�õ�ͼ�����ɫ
	Color = GetLayerColor(Layer);
	//�õ����ֱ�߶����Ƿ�Ҫ����
	Delete = GetLayerDelete( Layer);
	//���DeleteΪFALSE,���ֱ�߶����Ҫ��ʾ
	if (!Delete)  
	{
		pDoc->AddLine(Layer,Color,Delete,X1,Y1,X2,Y2);
	}
	//����������ǻ����ͻὫCAD�����е����ݶ�ȡ����������
	//pDoc->AddLine(Layer,Color,Delete,X1,Y1,X2,Y2);

}
//��ȡ���ߵ�����
void CLoadDXF::addArc(const DL_ArcData& data)
{
	CMainFrame* pFrame=(CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CFMDoc *pDoc=(CFMDoc *)pFrame->GetActiveDocument();
	CString Layer ;
	COLORREF Color;
	BOOL Delete;

	double CirceX = data.cx;
	double CirceY = data.cy;
	double CirceR = data.radius;
	//��Դ��������ǽǶȣ�����Ҫ���Ƕ�ת��Ϊ���ȣ��Ƕ�ת��Ϊ���ȹ�ʽ������=�Ƕȡ�(�� ��180 )
	double Angle1 = data.angle1 * (pi/180);
	double Angle2 = data.angle2 * (pi/180);

	Layer = attributes.getLayer().c_str();
	Color = GetLayerColor(Layer);
	Delete = GetLayerDelete( Layer);
	if (!Delete)
	{
		pDoc->AddArc(Layer,Color,Delete,CirceX,CirceY,CirceR,Angle1,Angle2);
	}

}
//��ȡԲ������
void CLoadDXF::addCircle(const DL_CircleData& data)
{
	CMainFrame* pFrame=(CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CFMDoc *pDoc=(CFMDoc *)pFrame->GetActiveDocument();
	CString Layer ;
	COLORREF Color;
	BOOL Delete;

	double CirceX = data.cx;
	double CirceY = data.cy;
	double CirceR = data.radius;

	Layer = attributes.getLayer().c_str();
	Color = GetLayerColor(Layer);
	Delete = GetLayerDelete( Layer);
	if (!Delete)
	{
		pDoc->AddPCircle(Layer,Color,Delete,CirceX,CirceY,CirceR);
	}

}
//��ȡ����ߵ�����//////�պϱ�־λ��1��ʾ�պϣ�0��ʾ�Ǳպ�
void CLoadDXF::addPolyline(const DL_PolylineData& data)
{
	//���߶α�־��1=�رգ�0=��
	Fill =(int)data.flags; 
	//������
	Numble = (int)data.number;   
	if (Numble>0)
	{
		m_PointList = new PointStruct[Numble+1];
	}
}
//�������������ʵ��// bulge = ͹�� Ĭ��ֵ��0  ,,(data.x, data.y, data.z,data.bulge);bulge = ͹�� Ĭ��ֵ��0
void CLoadDXF::addVertex(const DL_VertexData& data)
{
	if ( Numble>0 )
	{
		m_PointList[j].x = data.x;
		m_PointList[j].y = data.y;
		j++;
	}
	if ( j == Numble )
	{
		CMainFrame* pFrame=(CMainFrame*)(AfxGetApp()->m_pMainWnd);
		CFMDoc *pDoc=(CFMDoc *)pFrame->GetActiveDocument();
		CString Layer;
		COLORREF Color;
		BOOL Delete ;
		Layer = attributes.getLayer().c_str();
		Color = GetLayerColor(Layer);
		Delete = GetLayerDelete( Layer);
		if (!Delete)
		{
			pDoc->AddPLine(Layer,Color,Delete,Numble,m_PointList,Fill);
		}
		j = 0;
		BOOL Fill = -1;
		int Numble = 0;
		delete m_PointList;
	}
}

//*************************************************************************************************************

//ͼ�ε���ת
void CDraw::RotatePoint (double baseX , double baseY , double angle , double *xx, double *yy)
{
	double xx1,yy1;
	xx1 = *xx;
	yy1 = *yy;
	//�õ�������ڻ���Ļ���
	double angle1 = GetAngle(baseX ,baseY ,xx1,yy1);
	//�õ�����������
	double j1 = CalDisp(baseX ,baseY ,xx1,yy1);
	//�õ���ת���ĺ�����
	xx1 = baseX + j1*cos(angle + angle1);
	//�õ���ת����������
	yy1 = baseY + j1*sin (angle + angle1);
	//������ת��ĺ�����
	*xx = xx1;
	//������ת���������
	*yy = yy1;
}
//ͼ�ε���ת
double CDraw::GetAngle(double baseX , double baseY , double xx , double yy)
{
	double r ,ang;
	if ( fabs (baseX - xx) <=0.001 && fabs( baseY - yy) <=0.001) //�������ͼ���ĵ���ͬһ����
	{
		return 0;
	}
	if (fabs(baseX - xx) <=0.001) //�����������һ����ֱ����
	{
		if (yy>baseY)
			ang = (double)(pi/2);
		else
			ang = (double)(pi*3/2);
		return ang;
	}
	else
	{
		r = (double) sqrt ((xx - baseX) * (xx - baseX) + (yy - baseY) * (yy - baseY));
		double kk = (xx - baseX) /r;
		ang = (double) acos (kk);
		if (yy< baseY)
			ang = (double)(pi*2) - ang;
		return ang;
	}
}

//�������ŵĺ���
void CDraw::ScalePoint (double x_Base , double y_Base , double x_Scale , double y_Scale , double* xx , double* yy)
{
	*xx = x_Base + (*xx - x_Base) * x_Scale;
	*yy = y_Base + (*yy - y_Base) * y_Scale;
}

//ͼ��Ԫ�ػ��മ�л�
void CDraw::Serialize(CArchive& ar)
{
	if (ar.IsStoring())  //����Ǵ洢����
	{
		ar<<m_Layer<<m_Color<<b_Delete;
	}
	else                //����Ƕ�ȡ����
	{
		ar>>m_Layer>>m_Color>>b_Delete;
	}
}

//ͼ�㴮�л�
IMPLEMENT_SERIAL(CLayer,CObject,1)
	void CLayer::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar<<m_Layer<<m_Color<<b_Delete;
	} 
	else
	{
		ar>>m_Layer>>m_Color>>b_Delete;
	}

}

//ֱ�ߵĻ���
void CLine::Draw(CDC *pDC,int m_DrawMode,int m_DrawMode1,COLORREF BackColor)
{
	int x1,y1,x2,y2;
	if(b_Delete)	     //����Ѿ�����ɾ��״̬,�˳�����
		return;

	//���˶����Ƿ�����Ļ�����ཻ
	double minx,miny,maxx,maxy;
	GetRect(&minx,&miny,&maxx,&maxy);
	if(!IsRectCross(minx,miny,maxx,maxy))
		return;

	int LineType = 0;    //����
	int LineWide = 1;    //�߿�
	COLORREF ColorPen = m_Color;   //��ɫ

	if(m_DrawMode1==1)	//�����ѡ��ѡ���������ʾ�����ͼ��Ҫ������ʾ
	{
		
		int r = GetRValue(m_Color);
		int g = GetGValue(m_Color);
		int b = GetBValue(m_Color);
		ColorPen = RGB(255-r,255-g,255-b);
		
		if(LineType!=2) //���ͼ��Ԫ�ز����õĵ�2�����Σ����ߣ�������õ�2������
			LineType=2; 
		else
			LineType++;  //���ͼ��Ԫ���õĵ�2�����Σ����ߣ�������õ�3������
	}
	else if(m_DrawMode1==2)	 //���Ҫָ����ɫ���ƣ�����ɫ��Ϊ��Ļ��ɫBackColor
		ColorPen = BackColor;

	CPen pen((int)LineType,LineWide,ColorPen); //��������ʼ��һ��CPen�����趨���ʵ�����.���.��ɫ
	CPen* pOldPen = pDC->SelectObject(&pen);   //�ڻ�ͼ������ѡ��CPen����

	if(m_DrawMode==0)             //������õ�0�ֻ���ģʽ
		pDC->SetROP2(R2_COPYPEN); //�趨���ǵĻ���ģʽ
	else if(m_DrawMode==1)        //������õ�1�ֻ���ģʽ
		pDC->SetROP2(R2_NOTXORPEN);     //�����÷�д�Ļ���ģʽ

	DPtoVP(m_X1,m_Y1,&x1,&y1);    
	DPtoVP(m_X2,m_Y2,&x2,&y2);
	pDC->MoveTo(x1,y1);
	pDC->LineTo(x2,y2);
	pDC->SelectObject(pOldPen);//�ָ�����
	pDC->SetROP2(R2_COPYPEN);//����Ϊ�˿�ѡ֮�������ʧ
}

//�õ�ֱ�߶��������Ե����,����Line::GetRect���ã����㲢����һ��ֱ�ߵľ��α߽�
void CLine::GetRect(double *minX,double *minY,double *maxX,double *maxY)
{
	*minX = min(m_X1,m_X2);   //���㲢������Сxֵ
	*maxX = max(m_X1,m_X2);   //���㲢�������xֵ
	*minY = min(m_Y1,m_Y2);   //���㲢������Сyֵ
	*maxY = max(m_Y1,m_Y2);   //���㲢�������yֵ
}

//������CLine::IsPoint
//���ã��жϵ��Ƿ���ֱ���ϣ�������x,y)�ǵ��е�����꣬jlΪ���еľ��뷶Χ
//���أ�1-����ֱ�� 0-û�е���
BOOL CLine::IsPoint(double x,double y,double jl , double blc)
{
	double xx,x1,x2,y1,y2;

	//���ͼ��Ԫ������ɾ����־������û��ѡ�б�־
	if(b_Delete)
		return FALSE;

	//�õ�ÿ��ֱ�ߵı߽����
	GetRect(&x1,&y1,&x2,&y2);

	 //����㲻�ڱ߽������,��������FALSE
	if(!(x>=x1&&x<=x2&&y>=y1&&y<=y2))
		return FALSE;

	//�õ����е㵽ֱ�߾���
	xx=PointLine(x,y,m_X1,m_Y1,m_X2,m_Y2);
	if(xx<jl)//���ѡ��
		return TRUE;
	return FALSE;
}
//�������ߵ���ת
void CLine::Rotate(double baseX , double baseY , double angle)
{
	RotatePoint( baseX , baseY , angle , &this->m_X1 , &this->m_Y1 );
	RotatePoint( baseX , baseY , angle , &this->m_X2 , &this->m_Y2 );
}

//����ֱ�ߵ�����
void CLine::Scale (double x_Base , double y_Base , double x_Scale , double y_Scale )
{
	ScalePoint(x_Base ,y_Base ,x_Scale ,y_Scale ,&m_X1,&m_Y1);
	ScalePoint(x_Base ,y_Base ,x_Scale ,y_Scale ,&m_X2,&m_Y2);
}

//ֱ�ߵ��ƶ�
void CLine::Move(double x_Move , double y_Move)
{
	m_X1 += x_Move;
	m_X2 += x_Move;
	m_Y1 += y_Move;
	m_Y2 += y_Move;
}

//��ѡͼ��Ԫ���жϵ��麯�����ж�ֱ���ǲ���������ཻ�������ڲ�
BOOL CLine:: IsRectangle(PointStruct *pPoint ,double blc) 
{
	//����ֱ��������ཻ������TRUE
	if (LineRan(m_X1 , m_Y1 , m_X2 , m_Y2, pPoint, blc))
	{
		return TRUE;
	}
	//����ֱ��ֻҪ��һ���ھ����ڲ�������TRUE
	if (PointRgnRec(m_X1 ,m_Y1, pPoint,blc))
	{
		return TRUE;
	}
	//����ֱ��δ������ཻ��Ҳ���ھ����ڲ����ͷ���FALSE
	return FALSE;
}

//��������Ͽ�ѡ��ͼ��
BOOL CLine::IsLayerCombox(CString Lyaer)
{
	if (m_Layer == Lyaer)
	{
		return TRUE;
	}
	return FALSE;
}

//Ԫ�ض������ɫ����
void CLine::BlueBox(CDC* pDC)
{
	int x1,y1,x2,y2;
	DPtoVP(m_X1,m_Y1,&x1,&y1);    
	DPtoVP(m_X2,m_Y2,&x2,&y2);
	CPen pen , *pOldPen;
	pen.CreatePen(0,1,RGB(128,128,128));
	pOldPen=pDC->SelectObject(&pen);
	CBrush brush ,*pOldBrush;
	brush.CreateSolidBrush(RGB(0,255,0));
	pOldBrush = pDC->SelectObject(&brush);
	pDC->Rectangle(x1-4,y1-4,x1+4,y1+4);
	pDC->Rectangle(x2-4,y2-4,x2+4,y2+4);
	pDC->Rectangle((x1+x2)/2-4,(y1+y2)/2-4,(x1+x2)/2+4,(y1+y2)/2+4);
	pDC->SelectObject(pOldPen);     //�ָ�����
	pen.DeleteObject();
	pDC->SelectObject(pOldBrush); //�ָ�ˢ��
	brush.DeleteObject();
}

IMPLEMENT_SERIAL(CLine,CObject,1)
	void CLine::Serialize(CArchive& ar)
{
	CDraw::Serialize(ar);
	if (ar.IsStoring())
	{
		ar<<m_X1<<m_Y1<<m_X2<<m_Y2;
	} 
	else
	{
		ar>>m_X1>>m_Y1>>m_X2>>m_Y2;
	}
}

//****************************************************************************************************

//����߻���
void CPline::Draw(CDC *pDC,int m_DrawMode,int m_DrawMode1,COLORREF BackColor)
{
	int x1,y1;
	if(b_Delete)	//����Ѿ�����ɾ��״̬,�˳�����
		return;

	//���˶����Ƿ�����Ļ�����ཻ
	double minx,miny,maxx,maxy;
	GetRect(&minx,&miny,&maxx,&maxy);
	if(!IsRectCross(minx,miny,maxx,maxy))
		return;

	int LineType = 0;    //����
	int LineWide = 1;    //�߿�
	COLORREF ColorPen = m_Color;   //��ɫ

	if(m_DrawMode1==1)	//�����ѡ��ѡ���������ʾ�����ͼ��Ҫ������ʾ
	{
		int r = GetRValue(m_Color);
		int g = GetGValue(m_Color);
		int b = GetBValue(m_Color);
		ColorPen = RGB(255-r,255-g,255-b);

		if(LineType!=2) //���ͼ��Ԫ�ز����õĵ�2�����Σ����ߣ�������õ�2������
			LineType=2; 
		else
			LineType++;  //���ͼ��Ԫ���õĵ�2�����Σ����ߣ�������õ�3������
	}
	else if(m_DrawMode1==2)	 //���Ҫָ����ɫ���ƣ�����ɫ��Ϊ��Ļ��ɫBackColor
		ColorPen = BackColor;

	CPen pen((int)LineType,LineWide,ColorPen); //��������ʼ��һ��CPen�����趨���ʵ�����.���.��ɫ
	CPen* pOldPen = pDC->SelectObject(&pen);   //�ڻ�ͼ������ѡ��CPen����

	//CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	//CBrush *pOldBrush = pDC->SelectObject(pBrush);
	pDC->SelectStockObject(NULL_BRUSH);   //���ò����״̬��͸����ˢ

	if(m_DrawMode==0)             //������õ�0�ֻ���ģʽ
		pDC->SetROP2(R2_COPYPEN); //�趨���ǵĻ���ģʽ
	else if(m_DrawMode==1)        //������õ�1�ֻ���ģʽ
		pDC->SetROP2(R2_NOTXORPEN);     //�����÷�д�Ļ���ģʽ

	POINT* ppoint;
	ppoint = new POINT[m_Numble+1];
	for (int i = 0 ; i < m_Numble ; i++)  //��ʵ������ת������Ļ�������꣬����ppoint
	{
		DPtoVP(m_PointList[i].x , m_PointList[i].y , &x1 , &y1);
		ppoint[i].x = x1;
		ppoint[i].y = y1;
	}

	if(! b_Fill)   //��������ֱ��
	{
		ppoint[m_Numble] = ppoint[0];
		pDC->MoveTo(ppoint[0].x , ppoint[0].y);
		for(int i = 0; i< m_Numble ;i++)
			pDC->LineTo(ppoint[i].x , ppoint[i].y);
	}

	else
	{
		pDC->Polygon(ppoint,m_Numble);  //���ƶ���� 
	}

	delete ppoint;
	pDC->SelectObject(pOldPen);     //�ָ�����
	//pDC->SelectObject(pOldBrush); //�ָ�ˢ��
	pDC->SetROP2(R2_COPYPEN);//����Ϊ�˿�ѡ֮�������ʧ
}

//����߶��������Ե����//����:CPline::GetRect����:���㲢����һ������ֱ�ߵı߽����
void CPline::GetRect(double *minX,double *minY,double *maxX,double *maxY)
{
	double x1,y1,x2,y2;
	//�õ�һ�����������ʼ������
	x1 = m_PointList[0].x;
	y1 = m_PointList[0].y;
	x2 = m_PointList[0].x;
	y2 = m_PointList[0].y;
	//�ӵڶ������㿪ʼѭ��
	for (int i = 1; i<m_Numble; i++)
	{
		x1 = min(x1, m_PointList[i].x);    //�õ���Сx����
		x2 = max(x2, m_PointList[i].x);    //�õ����x����
		y1 = min(y1, m_PointList[i].y);    //�õ���Сy����
		y2 = max(y2, m_PointList[i].y);    //�õ����y����
	}
	*minX = x1; 
	*maxX = x2; 
	*minY = y1; 
	*maxY = y2;
}

//������PLine::IsPoint
//���ܣ��жϵ��Ƿ�������ֱ���ϣ�������x,y)�ǵ��е�����꣬jlΪ���еľ��뷶Χ��
//���أ�1-��������ֱ�߻������� 0-û�е���
BOOL CPline::IsPoint(double x,double y,double jl,double blc)
{
	int i;
	double xx,x1,y1,x2,y2;

	//���ͼ��Ԫ������ɾ����־����������FALSE
	if(b_Delete)
		return FALSE;

	//�õ�����ֱ�߻���������ı߽����
	GetRect(&x1,&y1,&x2,&y2); 

	//������е㲻�ڱ߽������,����ѡ��
	if(!(x>=x1-jl&&x<=x2+jl&&y>=y1-jl&&y<=y2+jl))
		return FALSE;	

	//���������ֱ��
	if(!b_Fill)			
	{
		for(i=0;i<m_Numble-1;i++)  //���ζ�����ֱ�ߵ�ÿ��ֱ�߽����ж�
		{
			xx=PointLine(x,y,m_PointList[i].x,m_PointList[i].y,m_PointList[i+1].x,m_PointList[i+1].y);
			if(xx<jl)                     //���ѡ��
				return TRUE;	
		}
	}
	//����Ƕ��������
	else				
	{
		 //������ڷ��������
		if(PointRgn(x,y,m_Numble,m_PointList,blc))   
			return TRUE;
	}
	return FALSE; //û��ѡ������ֱ�߻������򣬷���0
}

//�������ߵ���ת
void CPline::Rotate(double baseX , double baseY , double angle)
{
	for (int i = 0 ; i < this->m_Numble ; i++)
	{
		RotatePoint(baseX,baseY,angle,&m_PointList[i].x , &m_PointList[i].y);
	}
}
//�������ߵ�����
void CPline::Scale (double x_Base , double y_Base , double x_Scale , double y_Scale )
{
    for ( int i = 0 ; i< m_Numble ; i++)
    {
		ScalePoint(x_Base ,y_Base ,x_Scale ,y_Scale ,&m_PointList[i].x,&m_PointList[i].y);
    }

}

//����ߵ��ƶ�
void CPline::Move(double x_Move , double y_Move)
{
	for (int i = 0; i<m_Numble ;i++)
	{
		m_PointList[i].x += x_Move;
		m_PointList[i].y += y_Move;
	}
}

//��ѡͼ��Ԫ���жϵ��麯�����ж�ֱ���ǲ���������ཻ�������ڲ�
BOOL CPline:: IsRectangle(PointStruct *pPoint ,double blc) 
{
	//���������е�һ��ֱ��������ཻ������TRUE
	if (PLineRan(m_Numble,m_PointList, pPoint, blc))
	{
		return TRUE;
	}
	//���������еĵ�һ��������ཻ������TRUE
	if (PointRgnRec(m_PointList[0].x , m_PointList[0].y , pPoint , blc))
	{
		return TRUE;
	}
	return FALSE;
}


//��������Ͽ�ѡ��ͼ��
BOOL CPline::IsLayerCombox(CString Lyaer)
{
	if (m_Layer == Lyaer)
	{
		return TRUE;
	}
	return FALSE;
}

//Ԫ�ض������ɫ����
void CPline::BlueBox(CDC* pDC)
{
	CPen pen , *pOldPen;
	pen.CreatePen(0,1,RGB(0,128,0));
	pOldPen=pDC->SelectObject(&pen);
	CBrush brush ,*pOldBrush;
	brush.CreateSolidBrush(RGB(0,0,255));
	pOldBrush = pDC->SelectObject(&brush);
	int x1,y1;
	for (int i = 0 ; i < m_Numble ; i++) 
	{
		DPtoVP(m_PointList[i].x , m_PointList[i].y , &x1 , &y1);
		pDC->Rectangle(x1-4,y1-4,x1+4,y1+4);
	}
	pDC->SelectObject(pOldPen);     //�ָ�����
	pen.DeleteObject();
	pDC->SelectObject(pOldBrush); //�ָ�ˢ��
	brush.DeleteObject();
}

IMPLEMENT_SERIAL(CPline,CObject,1)
	void CPline::Serialize(CArchive& ar)
{
	int i;
	CDraw::Serialize(ar);
	if (ar.IsStoring())
	{
		ar<<b_Fill<<m_Numble;
		for (i = 0 ; i < m_Numble ; i++)
		{
			ar<<m_PointList[i].x<<m_PointList[i].y;
		}
	} 
	else
	{
		ar>>b_Fill>>m_Numble;
		m_PointList = new PointStruct[m_Numble];
		for (i = 0;i<m_Numble;i++)
		{
			ar>>m_PointList[i].x>>m_PointList[i].y;
		}
	}

}

//***********************************************************************************************************************

//����Բ
void CCircle::Draw(CDC *pDC,int m_DrawMode,int m_DrawMode1,COLORREF BackColor)
{
	if(b_Delete)	//����Ѿ�����ɾ��״̬,�˳�����
		return;
	int x ,y ,r;

	//���˶����Ƿ�����Ļ�����ཻ
	double minx,miny,maxx,maxy;
	GetRect(&minx,&miny,&maxx,&maxy);
	if(!IsRectCross(minx,miny,maxx,maxy))
		return;

	int LineType = 0;    //����
	int LineWide = 1;    //�߿�
	COLORREF ColorPen = m_Color;   //��ɫ

	if(m_DrawMode1==1)	//�����ѡ��ѡ���������ʾ�����ͼ��Ҫ������ʾ
	{
		int r = GetRValue(m_Color);
		int g = GetGValue(m_Color);
		int b = GetBValue(m_Color);
		ColorPen = RGB(255-r,255-g,255-b);

		if(LineType!=2) //���ͼ��Ԫ�ز����õĵ�2�����Σ����ߣ�������õ�2������
			LineType=2; 
		else
			LineType++;  //���ͼ��Ԫ���õĵ�2�����Σ����ߣ�������õ�3������
	}
	else if(m_DrawMode1==2)	 //���Ҫָ����ɫ���ƣ�����ɫ��Ϊ��Ļ��ɫBackColor
		ColorPen = BackColor;

	CPen pen((int)LineType,LineWide,ColorPen); //��������ʼ��һ��CPen�����趨���ʵ�����.���.��ɫ
	CPen* pOldPen = pDC->SelectObject(&pen);   //�ڻ�ͼ������ѡ��CPen����

	//CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	//CBrush *pOldBrush = pDC->SelectObject(pBrush);
	pDC->SelectStockObject(NULL_BRUSH);

	if(m_DrawMode==0)             //������õ�0�ֻ���ģʽ
		pDC->SetROP2(R2_COPYPEN); //�趨���ǵĻ���ģʽ
	else if(m_DrawMode==1)        //������õ�1�ֻ���ģʽ
		pDC->SetROP2(R2_NOTXORPEN);     //�����÷�д�Ļ���ģʽ

	DPtoVP(m_CirceX,m_CirceY,&x,&y);  //����õ�Բ�ĵ��߼�����
	r = DLtoVL(m_CirceR);             //����õ��뾶���߼����곤��
	pDC->Ellipse(x-r,y-r,x+r,y+r);    //����Բ
	pDC->SelectObject(pOldPen);       //�ָ�����
	pDC->SetROP2(R2_COPYPEN);//����Ϊ�˿�ѡ֮�������ʧ

}

//������CCircle::GetRect���ã����㲢����һ����Բ�ľ��α߽�
void CCircle::GetRect(double *minX,double *minY,double *maxX,double *maxY)
{
	*minX = m_CirceX - m_CirceR;  //�õ���������Сxֵ
	*minY = m_CirceY - m_CirceR;  //�õ����������xֵ
	*maxX = m_CirceX + m_CirceR;  //�õ���������Сyֵ
	*maxY = m_CirceY + m_CirceR;  //�õ����������yֵ
}

//������CCircle::IsPoint
//���ܣ��жϵ��Ƿ���Բ�ϣ�������x,y)�ǵ��е�����꣬jlΪ���еľ��뷶Χ��
//���أ�1-����Բ��Բ������0-û��ѡ��
BOOL CCircle::IsPoint(double x,double y,double jl, double blc)
{
	double xx;
	//���ͼ��Ԫ������ɾ����־����������FALSE
	if(b_Delete)
		return FALSE;

	//�㵽Բ�ĵľ���
	xx=CalDisp(x,y,m_CirceX,m_CirceY);	

	//������㵽Բ�ľ���С��jl
	if((xx >(m_CirceR-jl)) && (xx <(m_CirceR+jl)))
		return TRUE;

	//�������Բ��
	if(xx<=m_CirceR) 
		return TRUE;
// 	
	//���û�е���Բ�������Բ������0
	return FALSE;    
}

//����Բ����ת
void CCircle::Rotate(double baseX , double baseY , double angle)
{
	RotatePoint(baseX,baseY,angle, &this->m_CirceX, &this->m_CirceY);
}

//����Բ������
void CCircle::Scale (double x_Base , double y_Base , double x_Scale , double y_Scale )
{
	m_CirceX = x_Base + (m_CirceX - x_Base) * x_Scale;
	m_CirceY = y_Base + (m_CirceY - y_Base) * y_Scale;
	m_CirceR = m_CirceR * x_Scale;
}

//Բ���ƶ�
void CCircle::Move(double x_Move , double y_Move)
{
	m_CirceX += x_Move;
	m_CirceY += y_Move;
}

BOOL CCircle:: IsRectangle(PointStruct *pPoint ,double blc) 
{
	//����Բ���ھ����У�����TRUE
	if (PointRgnRec(m_CirceX, m_CirceY , pPoint , blc))
	{
		return TRUE;
	}
	return FALSE;

}

//��������Ͽ�ѡ��ͼ��
BOOL CCircle::IsLayerCombox(CString Lyaer)
{
	if (m_Layer == Lyaer)
	{
		return TRUE;
	}
	return FALSE;
}

//Ԫ�ض������ɫ����
void CCircle::BlueBox(CDC* pDC)
{
	CPen pen , *pOldPen;
	pen.CreatePen(0,1,RGB(128,0,0));
	pOldPen=pDC->SelectObject(&pen);
	CBrush brush ,*pOldBrush;
	brush.CreateSolidBrush(RGB(255,0,0));
	pOldBrush = pDC->SelectObject(&brush);

	int x,y,r;
	DPtoVP(m_CirceX,m_CirceY,&x,&y);  //����õ�Բ�ĵ��߼�����
	r = DLtoVL(m_CirceR);             //����õ��뾶���߼����곤��

	pDC->Rectangle(x-4,y-4,x+4,y+4);
	pDC->Rectangle(x-r-4,y-4,x-r+4,y+4);
	pDC->Rectangle(x+r-4,y-4,x+r+4,y+4);
	pDC->Rectangle(x-4,y-r-4,x+4,y-r+4);
	pDC->Rectangle(x-4,y+r-4,x+4,y+r+4);
	
	pDC->SelectObject(pOldPen);     //�ָ�����
	pen.DeleteObject();
	pDC->SelectObject(pOldBrush); //�ָ�ˢ��
	brush.DeleteObject();
}

IMPLEMENT_SERIAL(CCircle,CObject,1)
	void CCircle::Serialize(CArchive& ar)
{
	CDraw::Serialize(ar);
	if (ar.IsStoring())
	{
		ar<<m_CirceX<<m_CirceY<<m_CirceR;
	} 
	else
	{
		ar>>m_CirceX>>m_CirceY>>m_CirceR;
	}
}

//*****************************************************************************************************

//���ƻ���
void CArc::Draw(CDC *pDC,int m_DrawMode,int m_DrawMode1,COLORREF BackColor)
{
	if(b_Delete)	//����Ѿ�����ɾ��״̬,�˳�����
		return;

	//���˶����Ƿ�����Ļ�����ཻ
	double minx,miny,maxx,maxy;
	GetRect(&minx,&miny,&maxx,&maxy);
	if(!IsRectCross(minx,miny,maxx,maxy))
		return;

	int x ,y ,rr;
	int x1,x2,y1,y2;

	int LineType = 0;    //����
	int LineWide = 1;    //�߿�
	COLORREF ColorPen = m_Color;   //��ɫ

	if(m_DrawMode1==1)	//�����ѡ��ѡ���������ʾ�����ͼ��Ҫ������ʾ
	{
		int r = GetRValue(m_Color);
		int g = GetGValue(m_Color);
		int b = GetBValue(m_Color);
		ColorPen = RGB(255-r,255-g,255-b);

		if(LineType!=2) //���ͼ��Ԫ�ز����õĵ�2�����Σ����ߣ�������õ�2������
			LineType=2; 
		else
			LineType++;  //���ͼ��Ԫ���õĵ�2�����Σ����ߣ�������õ�3������
	}
	else if(m_DrawMode1==2)	 //���Ҫָ����ɫ���ƣ�����ɫ��Ϊ��Ļ��ɫBackColor
		ColorPen = BackColor;

	CPen pen((int)LineType,LineWide,ColorPen); //��������ʼ��һ��CPen�����趨���ʵ�����.���.��ɫ
	CPen* pOldPen = pDC->SelectObject(&pen);   //�ڻ�ͼ������ѡ��CPen����

	if(m_DrawMode==0)             //������õ�0�ֻ���ģʽ
		pDC->SetROP2(R2_COPYPEN); //�趨���ǵĻ���ģʽ
	else if(m_DrawMode==1)        //������õ�1�ֻ���ģʽ
		pDC->SetROP2(R2_NOTXORPEN);     //�����÷�д�Ļ���ģʽ

	DPtoVP(m_CirceX,m_CirceY,&x,&y);  //��Բ��ʵ������תΪ��Ļ����
	rr = DLtoVL(m_CirceR);            //����õ��뾶���߼����곤��          
	//�õ���������
	x1 = x + DLtoVL(m_CirceR * cos(m_Angle1));
	y1 = y - DLtoVL(m_CirceR * sin(m_Angle1));
	//�õ��յ������
	x2 = x + DLtoVL(m_CirceR * cos(m_Angle2));
	y2 = y - DLtoVL(m_CirceR * sin(m_Angle2));

	pDC->Arc(x-rr,y-rr,x+rr,y+rr,x1,y1,x2,y2);  //����Բ��
	pDC->SelectObject(pOldPen);                 //�ָ�����
	pDC->SetROP2(R2_COPYPEN);//����Ϊ�˿�ѡ֮�������ʧ
}

//������Aarc::GetRect���ã�����һ�����ı߽���Σ�����������arc1��
void CArc::GetRect(double *minX,double *minY,double *maxX,double *maxY)
{
	double x,y,x1,x2,y1,y2;
	int i ;
	//  �õ�Բ���������
	x = m_CirceX + (double)(cos(m_Angle1) * m_CirceR);
	y = m_CirceY + (double)(sin(m_Angle1) * m_CirceR);
	//���߽���γ�ֵ��
	x1 = x;
	y1 = y;
	x2 = x;
	y2 = y;
	// �õ�Բ���յ�����
	x = m_CirceX + (double)(cos(m_Angle2) * m_CirceR);
	y = m_CirceY + (double)(sin(m_Angle2) * m_CirceR);
	//�õ���С��������� 
	x1 = min(x,x1);
	x2 = max(x,x2);
	y1 = min(y,y1);
	y2 = max(y,y2);

	for (i = 0 ; i < 4; i++)
	{
		if (IsInArc(pi/2*i))
		{
			if (i == 0)
			{
				x = m_CirceX + m_CirceR;
				y = m_CirceY;
			}
			if (i == 1)
			{
				x = m_CirceX ;
				y = m_CirceY + m_CirceR ;
			}
			if (i == 2)
			{
				x = m_CirceX - m_CirceR;
				y = m_CirceY;
			}
			if (i == 3)
			{
				x = m_CirceX ;
				y = m_CirceY - m_CirceR ;
			}

			x1 = min(x,x1);
			x2 = max(x,x2);
			y1 = min(y,y1);
			y2 = max(y,y2);
		}
	}
	*minX = x1; 
	*maxX = x2; 
	*minY = y1; 
	*maxY = y2;
}
BOOL CArc::IsInArc(double angle)
{
	if ((m_Angle1 < m_Angle2 && angle < m_Angle2 && angle > m_Angle1) 
		|| m_Angle1 > m_Angle2 && (!(angle <= m_Angle1 && angle >= m_Angle2)))
		return TRUE;
	else
		return FALSE;
}

//������CArc::IsPoint
//���ܣ��жϵ��Ƿ���Բ���ϣ�������x,y)�ǵ��е�����꣬jlΪ���еľ��뷶Χ��
//���أ�1-����Բ����index���ص��е�ͼ��Ԫ�ص����кţ�index1����Ψһʶ���
BOOL CArc::IsPoint(double x,double y,double jl, double blc)
{
	//����CCircle���IsPoint�ж��Ƿ�ѡ����Բ
	if(CCircle::IsPoint(x,y,jl,blc))
	{
		//���е㵽Բ�ĵľ���
		double xx=CalDisp(x,y,m_CirceX,m_CirceY);
		//���е������Բ�ĵĻ���
		double ang=(double)acos((x-m_CirceX)/xx);

		if(y-m_CirceY<0)
			ang=(double)pi*2-ang;

		//������еĵ������Բ�ĵĻ�����Բ���Ļ��ȷ�Χ�ڣ��򷵻�TRUE
		if(IsInArc(ang))
			return TRUE;
	}
	return FALSE;//���û�е���Բ��������0
}

//����Բ������ת
void CArc::Rotate(double baseX , double baseY , double angle)
{
	//pProgram->
	RotatePoint(baseX,baseY,angle, &this->m_CirceX, &this->m_CirceY);
	this->m_Angle1+=angle;
	this->m_Angle2+=angle;
}

//����Բ��������
void  CArc::Scale (double x_Base , double y_Base , double x_Scale , double y_Scale )
{
	m_CirceX = x_Base + (m_CirceX - x_Base) * x_Scale;
	m_CirceY = y_Base + (m_CirceY - y_Base) * y_Scale;
	m_CirceR = m_CirceR * x_Scale;
}

//��������Ͽ�ѡ��ͼ��
BOOL CArc::IsLayerCombox(CString Lyaer)
{
	if (m_Layer == Lyaer)
	{
		return TRUE;
	}
	return FALSE;
}

//Ԫ�ض������ɫ����
void CArc::BlueBox(CDC* pDC)
{
	CPen pen , *pOldPen;
	pen.CreatePen(0,1,RGB(128,128,0));
	pOldPen=pDC->SelectObject(&pen);
	CBrush brush ,*pOldBrush;
	brush.CreateSolidBrush(RGB(255,255,0));
	pOldBrush = pDC->SelectObject(&brush);

	int x,y,r;
	DPtoVP(m_CirceX,m_CirceY,&x,&y);  //����õ�Բ�ĵ��߼�����
	r = DLtoVL(m_CirceR);             //����õ��뾶���߼����곤��

	pDC->Rectangle(x-4,y-4,x+4,y+4);
	pDC->Rectangle(x-r-4,y-4,x-r+4,y+4);
	pDC->Rectangle(x+r-4,y-4,x+r+4,y+4);
	pDC->Rectangle(x-4,y-r-4,x+4,y-r+4);
	pDC->Rectangle(x-4,y+r-4,x+4,y+r+4);

	pDC->SelectObject(pOldPen);     //�ָ�����
	pen.DeleteObject();
	pDC->SelectObject(pOldBrush); //�ָ�ˢ��
	brush.DeleteObject();
}

IMPLEMENT_SERIAL(CArc,CObject,1)
	void CArc::Serialize(CArchive& ar)
{
	CCircle::Serialize(ar);
	if (ar.IsStoring())
	{
		ar<<m_Angle1<<m_Angle2;
	} 
	else
	{
		ar>>m_Angle1>>m_Angle2;
	}
}


//**********************************************************************************************************************************


//���ã�����㣨xx,yy)���߶Σ�x1,y1)(x2,y2)�ľ��룬���ؼ���ľ���ֵ
double CDraw::PointLine(double xx,double yy,double x1,double y1,double x2,double y2)
{
	double a,b,c,ang1,ang2,ang;
	//���������ߵľ���
	a=CalDisp(x1,y1,xx,yy);
	if(a==0.0)return 0.0;
	b=CalDisp(x2,y2,xx,yy);
	if(b==0.0)return 0.0;
	c=CalDisp(x1,y1,x2,y2);

	//�����x1,y1)��(x2,y2)��������ͬ�����߶���һ���㣬�˳����������ؾ���
	if(c==0.0) return a;
	
	if(a<b) //���(xx,yy)�ĵ�(x1,y1�������߽϶�
	{
		if(y1==y2) //���ֱ�߶�AB��ˮƽ�ߣ��õ�ֱ�߶�AB�Ļ���
		{
			if(x1<x2)
				ang1=0;
			else
				ang1=(double)pi;
		}
		else  //���ֱ�߶�AB����ˮƽ�ߣ��õ�ֱ�߶�AB��X������нǵĻ���
		{
			ang1=(double)acos((x2-x1)/c);
			if(y1>y2)ang1=(double)pi*2-ang1;  //ֱ��(x1,y1)-(x2,y2)�Ļ���
		}
		ang2=(double)acos((xx-x1)/a);
		if(y1>yy)ang2=(double)pi*2-ang2;  //ֱ��(x1,y1)-(xx,yy)�Ļ���
		ang=ang2-ang1;
		if(ang<0)ang=-ang;
		if(ang>pi) ang=(double)pi*2-ang;  //���ǵĴ�С
		if(ang>pi/2) return a;    //���Ϊ�۽ǣ�ֱ�ӷ��ؾ���
		else 
			return (a*(double)sin(ang)); //���򷵻ؼ���õ��ľ��� 
	}

	else //���(xx,yy)�ĵ�(x2,y2�������߽϶�
	{
		if(y1==y2) //������������������ͬ����ֱ�ӵõ�ֱ��б�ʵĻ���
		{
			if(x1<x2)
				ang1=(double)pi;
			else
				ang1=0;
		}
		else //���������������겻ͬ
		{
			ang1=(double)acos((x1-x2)/c);     //ֱ�ߣ�x2,y2)-(x1,y1)��б�ʵĻ���
			if(y2>y1)ang1=(double)pi*2-ang1;
		}
		ang2=(double)acos((xx-x2)/b);     //ֱ�ߣ�x2,x1��-(xx,yy)��б�ʵĻ���
		if(y2>yy)ang2=(double)pi*2-ang2;
		ang=ang2-ang1;
		if(ang<0) ang=-ang;
		if(ang>pi) ang=(double)pi*2-ang;  //���ǵĴ�С ?
		if(ang>pi/2)  return b;   //���Ϊ�۽ǣ�ֱ�ӷ��ؾ���
		else 
			return(b*(double)sin(ang));  //���򷵻ؼ���õ��ľ���
	}
}

//�����(x1,y1)���(x2,y2)��ľ���
double CDraw::CalDisp(double x1,double y1,double x2,double y2)
{
	return (double)sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}


//����PointRgn���ã��ж�һ�����Ƿ�����������������
//������Numble-����ζ�����Ŀ��x,y�ǵ�����꣬����ζ���������ڽṹ����PointList��
//���أ�1-���ڶ��������  0-����
BOOL CDraw::PointRgn(double x,double y,int Numble,PointStruct *PointList,double blc)
{
	//����һ��CRgn��
	CRgn rgn;         
	int x1,y1,i;
	// ����һ����ṹָ��
	POINT* pPoint;      

	//��������Ŀ<3������һ�����򷵻ز��ɹ���־
	if(Numble<3) 
		return 0;

	 //���ṹָ�붯̬����Numble����
	pPoint = new POINT[Numble];

	//�����е������ת�����Ļ����Ҳ������������
	x1=(int)(x/blc);
	y1=(int)(y/blc);

	//��������򣬾��Ƕ�������򣬸��������ת����Ļ����Ҳ������������
	for(i=0;i<Numble;i++)
	{
		pPoint[i].x=(int)((PointList[i].x)/blc);
		pPoint[i].y=(int)((PointList[i].y)/blc);
	}

	//���Ի�һ�����������
	rgn.CreatePolygonRgn(pPoint,Numble,1);
	//�����������j=1,����j=0;
	i=(rgn.PtInRegion(x1,y1)!= 0);//***********************??????????***************
	//ɾ�������rgn����
	rgn.DeleteObject();	
	delete pPoint;
	return i;
}

//����Ϊ�˼���ֱ�߶κ�ֱ�߶��ཻ��ϵ�ĺ���
int CDraw::LineLine(double m_X1 , double m_Y1 , double m_X2 , double m_Y2, double XXX1 , double YYY1 , double XXX2 , double YYY2)
{
	double xx1,xx2,yy1,yy2;//��һ��ֱ��
	double x1,x2,y1,y2,n1,n2,n3,n4;//�ڶ���ֱ��
	double k1 , k2;//����ֱ�ߵ�б��

	if (m_X1>m_X2)
	{
		xx1 = m_X2;
		xx2 = m_X1;
		yy1 = m_Y2;
		yy2 = m_Y1;
	}
	else
	{
		xx1 = m_X1;
		xx2 = m_X2;
		yy1 = m_Y1;
		yy2 = m_Y2;

	}
	if (fabs(xx1- xx2)<=0.001 && fabs(yy1- yy2)<= 0.001)
	{
		return 0;
	}

	if (XXX1 > XXX2)
	{
		x1 = XXX2;
		x2 = XXX1;
		y1 = YYY2;
		y2 = YYY1;
	}
	else
	{
		x1 = XXX1;
		x2 = XXX2;
		y1 = YYY1;
		y2 = YYY2;
	}
	if (fabs(x1 - x2 )<= 0.001  && fabs( y1- y2)<=0.001)
	{
		return 0;
	}
	if (fabs(x1 - x2)<= 0.001 && fabs( xx1 - xx2)<=0.001 )
	{
		if (x1 != xx1)
		{
			return 0;
		}

		n1 = min(yy1,yy2);
		n2 = max(yy1,yy2);
		n3 = min(y1,y2);
		n4 = max(y1,y2);
		if (n1>n4 || n2<n3)
		{
			return 0;
		}
		else if (n1 == n4 || n2 == n3)
		{
			if (n1 == n4)
			{
				return 1;
			}
			if (n2 == n3)
			{
				return 1;
			}
		}
		else
		{
			return 2;
		}
	}

	else if (fabs(x1 - x2)<= 0.001 && xx1 != xx2)
	{
		if (!(x1>=xx1 && x1<=xx2))
		{
			return 0;
		}

		double nn = yy1+(x1-xx1)/(xx2-xx1)*(yy2-yy1);
		if (nn>=y1 && nn<=y2 || nn>= y2 && nn<=y1)
		{
			return 1;
		}
		else
		{
			return 0 ;
		}
	}
	else if(x1!=x2 && fabs(xx1 - xx2)<=0.001)
	{
		if (!(xx1>=x1 && xx1<=x2))
		{
			return 0;
		}
		double nn1 = y1+(xx1 - x1)/(x2 - x1)*(y2 - y1);
		if (nn1>=yy1 && nn1<=yy2 || nn1>=yy2 && nn1<=yy1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	else
	{
		k1 = (y2 - y1)/(x2 -x1);
		k2 = (yy2 - yy1)/(xx2 - xx1);
		if (k1 == k2)
		{
			if (x1>xx2 || x2<xx1)
			{
				return 0;
			}
			else
			{
				n1 = y1+ (0-x1)*k1;
				n2 = yy1+ (0-x1)*k2;
				if (n1 !=n2)
				{
					return 0;
				}
				else
				{
					n3 = max(xx1,x1);
					n4 = min(xx2,x2);

					double nn2 = y1+(n3-x1)*k1;
					if (n3 == n4)
					{
						return 1;
					}

					return 2;

				}
			}
		}
		else
		{
			double nn2 = (yy1-y1+x1*k1-xx1*k2)/(k1-k2);
			if ((nn2>=x1 && nn2<=x2) && (nn2>=xx1 && nn2<=xx2))
			{
				return 1;
			}
			else
				return 0;
		}
	}
	return 0;
}

//����ֱ�߶�������ཻ
BOOL CDraw:: LineRan(double xx1 , double yy1 , double xx2 , double yy2,PointStruct * pPoint,double blc)
{
	double x1 ,y1,x2,y2;
	for (int i = 0 ;i < 4 ;i++)
	{
		x1 = pPoint[i].x;
		y1 = pPoint[i].y;
		x2 = pPoint[i+1].x;
		y2 = pPoint[i+1].y;

		//����ཻ��LineLineΪ��0���ͷ���TRUE
		if (LineLine(xx1,yy1,xx2,yy2,x1,y1,x2,y2))
		{
			return TRUE;
			break;
		}	
	}
	return FALSE;
}

//��������������ཻ
BOOL CDraw:: PLineRan(int PointNunb,PointStruct * pPLine, PointStruct * pPoint,double blc)
{
	double x1,y1,x2,y2;
	for (int i=0 ; i<PointNunb-1 ; i++)//�Զ��߶ε�����ֱ��ֱ��ѭ��
	{
		x1 = pPLine[i].x;
		y1 = pPLine[i].y;
		x2 = pPLine[i+1].x;
		y2 = pPLine[i+1].y;

		//������߶��е�ֱ������ε��ཻ��ϵ
		if (LineRan(x1,y1,x2,y2,pPoint,blc))
		{
			//����ཻ���ͷ���TRUE,��ֹͣѭ��
			return TRUE;
			break;
		}
	}
	return FALSE;	
}


//�����߶ζ˵��ǲ����ھ���������----�������
BOOL CDraw::PointRgnRec(double x ,double y, PointStruct *PointList,double blc)
{
	//����һ��rgn����
	CRgn rgn;
	int p_x1,p_y1,x1,y1,x2,y2;
	p_x1 = (int)(x/blc);
	p_y1 = (int)(y/blc);
	x1 = (int)((PointList[0].x )/blc) ;
	y1 = (int)((PointList[0].y )/blc) ;
	x2 = (int)((PointList[2].x )/blc) ;
	y2 = (int)((PointList[2].y )/blc) ;

	//��ʼ��һ�����������
	rgn.CreateRectRgn(x1,y1,x2,y2);
	//������������Ϊ����ֵ������Ϊ0
	BOOL YN = rgn.PtInRegion(p_x1,p_y1);
	//ɾ�������rgn����
	rgn.DeleteObject();
	return YN;
}



//*****************************************************************************************************************
//��ȡͼ����ɫ
COLORREF CLoadDXF::GetLayerColor(CString Layer)
{
	CMainFrame* pFrame=(CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CFMDoc *pDoc=(CFMDoc *)pFrame->GetActiveDocument();
	int nSize = pDoc->m_LayerArray.GetSize();
	if (nSize<=0)
	{
		AfxMessageBox(_T("û�з���ͼ������"));
		return RGB(255,255,255);
	}
	for (int i = 0; i< nSize ;i++)
	{
		if (Layer == pDoc->m_LayerArray.GetAt(i)->m_Layer)
		{
			return pDoc->m_LayerArray.GetAt(i)->m_Color;
		}
	}
	return RGB(255,255,255);
}
//�õ�ͼ����ʾ���ر�־
BOOL CLoadDXF:: GetLayerDelete(CString Layer)
{
	CMainFrame* pFrame=(CMainFrame*)(AfxGetApp()->m_pMainWnd);
	CFMDoc *pDoc=(CFMDoc *)pFrame->GetActiveDocument();
	int nSize = pDoc->m_LayerArray.GetSize();
	if (nSize<=0)
	{
		AfxMessageBox(_T("û�з���ͼ����"));
		return TRUE;       //b_Delete����TRUE
	}
	for (int i = 0; i< nSize ;i++)
	{
		if (Layer == pDoc->m_LayerArray.GetAt(i)->m_Layer)
		{
			return pDoc->m_LayerArray.GetAt(i)->b_Delete;
		}

	}
	return TRUE; //b_Delete����TRUE
}

//RGBת��
COLORREF CLoadDXF::LoadColorreF(int Index)
{
	if(Index>255||Index<0)
	{
		return RGB(255,255,255);
	}
	int r,g,b;
	switch(Index)
	{
	case 1:r=255;g=0;b=0;break;
	case 2:r=255;g=255;b=0;break;
	case 3:r=0;g=255;b=0;break;
	case 4:r=0;g=255;b=255;break;
	case 5:r=0;g=0;b=255;break;
	case 6:r=255;g=0;b=255;break;
	case 7:r=255;g=255;b=255;break;
	case 8:r=128;g=128;b=128;break;
	case 9:r=192;g=192;b=192;break;
	case 10:r=255;g=0;b=0;break;
	case 11:r=255;g=127;b=127;break;
	case 12:r=204;g=0;b=0;break;
	case 13:r=204;g=102;b=102;break;
	case 14:r=153;g=0;b=0;break;
	case 15:r=153;g=76;b=76;break;
	case 16:r=127;g=0;b=0;break;
	case 17:r=127;g=63;b=63;break;
	case 18:r=76;g=0;b=0;break;
	case 19:r=76;g=38;b=38;break;
	case 20:r=255;g=63;b=0;break;
	case 21:r=255;g=159;b=127;break;
	case 22:r=204;g=51;b=0;break;
	case 23:r=204;g=127;b=102;break;
	case 24:r=153;g=38;b=0;break;
	case 25:r=153;g=95;b=76;break;
	case 26:r=127;g=31;b=0;break;
	case 27:r=127;g=79;b=63;break;
	case 28:r=76;g=19;b=0;break;
	case 29:r=76;g=47;b=38;break;
	case 30:r=255;g=127;b=0;break;
	case 31:r=255;g=191;b=127;break;
	case 32:r=204;g=102;b=0;break;
	case 33:r=204;g=153;b=102;break;
	case 34:r=153;g=76;b=0;break;
	case 35:r=153;g=114;b=76;break;
	case 36:r=127;g=63;b=0;break;
	case 37:r=127;g=95;b=63;break;
	case 38:r=76;g=38;b=0;break;
	case 39:r=76;g=57;b=38;break;
	case 40:r=255;g=191;b=0;break;
	case 41:r=255;g=223;b=127;break;
	case 42:r=204;g=153;b=0;break;
	case 43:r=204;g=178;b=102;break;
	case 44:r=153;g=114;b=0;break;
	case 45:r=153;g=133;b=76;break;
	case 46:r=127;g=95;b=0;break;
	case 47:r=127;g=111;b=63;break;
	case 48:r=76;g=57;b=0;break;
	case 49:r=76;g=66;b=38;break;
	case 50:r=255;g=255;b=0;break;
	case 51:r=255;g=255;b=127;break;
	case 52:r=204;g=204;b=0;break;
	case 53:r=204;g=204;b=102;break;
	case 54:r=153;g=153;b=0;break;
	case 55:r=153;g=153;b=76;break;
	case 56:r=127;g=127;b=0;break;
	case 57:r=127;g=127;b=63;break;
	case 58:r=76;g=76;b=0;break;
	case 59:r=76;g=76;b=38;break;
	case 60:r=191;g=255;b=0;break;
	case 61:r=223;g=255;b=127;break;
	case 62:r=153;g=204;b=0;break;
	case 63:r=178;g=204;b=102;break;
	case 64:r=114;g=153;b=0;break;
	case 65:r=133;g=153;b=76;break;
	case 66:r=95;g=127;b=0;break;
	case 67:r=111;g=127;b=63;break;
	case 68:r=57;g=76;b=0;break;
	case 69:r=66;g=76;b=38;break;
	case 70:r=127;g=255;b=0;break;
	case 71:r=191;g=255;b=127;break;
	case 72:r=102;g=204;b=0;break;
	case 73:r=153;g=204;b=102;break;
	case 74:r=76;g=153;b=0;break;
	case 75:r=114;g=153;b=76;break;
	case 76:r=63;g=127;b=0;break;
	case 77:r=95;g=127;b=63;break;
	case 78:r=38;g=76;b=0;break;
	case 79:r=57;g=76;b=38;break;
	case 80:r=63;g=255;b=0;break;
	case 81:r=159;g=255;b=127;break;
	case 82:r=51;g=204;b=0;break;
	case 83:r=127;g=204;b=102;break;
	case 84:r=38;g=153;b=0;break;
	case 85:r=95;g=153;b=76;break;
	case 86:r=31;g=127;b=0;break;
	case 87:r=79;g=127;b=63;break;
	case 88:r=19;g=76;b=0;break;
	case 89:r=47;g=76;b=38;break;
	case 90:r=0;g=255;b=0;break;
	case 91:r=127;g=255;b=127;break;
	case 92:r=0;g=204;b=0;break;
	case 93:r=102;g=204;b=102;break;
	case 94:r=0;g=153;b=0;break;
	case 95:r=76;g=153;b=76;break;
	case 96:r=0;g=127;b=0;break;
	case 97:r=63;g=127;b=63;break;
	case 98:r=0;g=76;b=0;break;
	case 99:r=38;g=76;b=38;break;
	case 100:r=0;g=255;b=63;break;
	case 101:r=127;g=255;b=159;break;
	case 102:r=0;g=204;b=51;break;
	case 103:r=102;g=204;b=127;break;
	case 104:r=0;g=153;b=38;break;
	case 105:r=76;g=153;b=95;break;
	case 106:r=0;g=127;b=31;break;
	case 107:r=63;g=127;b=79;break;
	case 108:r=0;g=76;b=19;break;
	case 109:r=38;g=76;b=47;break;
	case 110:r=0;g=255;b=127;break;
	case 111:r=127;g=255;b=191;break;
	case 112:r=0;g=204;b=102;break;
	case 113:r=102;g=204;b=153;break;
	case 114:r=0;g=153;b=76;break;
	case 115:r=76;g=153;b=114;break;
	case 116:r=0;g=127;b=63;break;
	case 117:r=63;g=127;b=95;break;
	case 118:r=0;g=76;b=38;break;
	case 119:r=38;g=76;b=57;break;
	case 120:r=0;g=255;b=191;break;
	case 121:r=127;g=255;b=223;break;
	case 122:r=0;g=204;b=153;break;
	case 123:r=102;g=204;b=178;break;
	case 124:r=0;g=153;b=114;break;
	case 125:r=76;g=153;b=133;break;
	case 126:r=0;g=127;b=95;break;
	case 127:r=63;g=127;b=111;break;
	case 128:r=0;g=76;b=57;break;
	case 129:r=38;g=76;b=66;break;
	case 130:r=0;g=255;b=255;break;
	case 131:r=127;g=255;b=255;break;
	case 132:r=0;g=204;b=204;break;
	case 133:r=102;g=204;b=204;break;
	case 134:r=0;g=153;b=153;break;
	case 135:r=76;g=153;b=153;break;
	case 136:r=0;g=127;b=127;break;
	case 137:r=63;g=127;b=127;break;
	case 138:r=0;g=76;b=76;break;
	case 139:r=38;g=76;b=76;break;
	case 140:r=0;g=191;b=255;break;
	case 141:r=127;g=223;b=255;break;
	case 142:r=0;g=153;b=204;break;
	case 143:r=102;g=178;b=204;break;
	case 144:r=0;g=114;b=153;break;
	case 145:r=76;g=133;b=153;break;
	case 146:r=0;g=95;b=127;break;
	case 147:r=63;g=111;b=127;break;
	case 148:r=0;g=57;b=76;break;
	case 149:r=38;g=66;b=76;break;
	case 150:r=0;g=127;b=255;break;
	case 151:r=127;g=191;b=255;break;
	case 152:r=0;g=102;b=204;break;
	case 153:r=102;g=153;b=204;break;
	case 154:r=0;g=76;b=153;break;
	case 155:r=76;g=114;b=153;break;
	case 156:r=0;g=63;b=127;break;
	case 157:r=63;g=95;b=127;break;
	case 158:r=0;g=38;b=76;break;
	case 159:r=38;g=57;b=76;break;
	case 160:r=0;g=63;b=255;break;
	case 161:r=127;g=159;b=255;break;
	case 162:r=0;g=51;b=204;break;
	case 163:r=102;g=127;b=204;break;
	case 164:r=0;g=38;b=153;break;
	case 165:r=76;g=95;b=153;break;
	case 166:r=0;g=31;b=127;break;
	case 167:r=63;g=79;b=127;break;
	case 168:r=0;g=19;b=76;break;
	case 169:r=38;g=47;b=76;break;
	case 170:r=0;g=0;b=255;break;
	case 171:r=127;g=127;b=255;break;
	case 172:r=0;g=0;b=204;break;
	case 173:r=102;g=102;b=204;break;
	case 174:r=0;g=0;b=153;break;
	case 175:r=76;g=76;b=153;break;
	case 176:r=0;g=0;b=127;break;
	case 177:r=63;g=63;b=127;break;
	case 178:r=0;g=0;b=76;break;
	case 179:r=38;g=38;b=76;break;
	case 180:r=63;g=0;b=255;break;
	case 181:r=159;g=127;b=255;break;
	case 182:r=51;g=0;b=204;break;
	case 183:r=127;g=102;b=204;break;
	case 184:r=38;g=0;b=153;break;
	case 185:r=95;g=76;b=153;break;
	case 186:r=31;g=0;b=127;break;
	case 187:r=79;g=63;b=127;break;
	case 188:r=19;g=0;b=76;break;
	case 189:r=47;g=38;b=76;break;
	case 190:r=127;g=0;b=255;break;
	case 191:r=191;g=127;b=255;break;
	case 192:r=102;g=0;b=204;break;
	case 193:r=153;g=102;b=204;break;
	case 194:r=76;g=0;b=153;break;
	case 195:r=114;g=76;b=153;break;
	case 196:r=63;g=0;b=127;break;
	case 197:r=95;g=63;b=127;break;
	case 198:r=38;g=0;b=76;break;
	case 199:r=57;g=38;b=76;break;
	case 200:r=191;g=0;b=255;break;
	case 201:r=223;g=127;b=255;break;
	case 202:r=153;g=0;b=204;break;
	case 203:r=178;g=102;b=204;break;
	case 204:r=114;g=0;b=153;break;
	case 205:r=133;g=76;b=153;break;
	case 206:r=95;g=0;b=127;break;
	case 207:r=111;g=63;b=127;break;
	case 208:r=57;g=0;b=76;break;
	case 209:r=66;g=38;b=76;break;
	case 210:r=255;g=0;b=255;break;
	case 211:r=255;g=127;b=255;break;
	case 212:r=204;g=0;b=204;break;
	case 213:r=204;g=102;b=204;break;
	case 214:r=153;g=0;b=153;break;
	case 215:r=153;g=76;b=153;break;
	case 216:r=127;g=0;b=127;break;
	case 217:r=127;g=63;b=127;break;
	case 218:r=76;g=0;b=76;break;
	case 219:r=76;g=38;b=76;break;
	case 220:r=255;g=0;b=191;break;
	case 221:r=255;g=127;b=223;break;
	case 222:r=204;g=0;b=153;break;
	case 223:r=204;g=102;b=178;break;
	case 224:r=153;g=0;b=114;break;
	case 225:r=153;g=76;b=133;break;
	case 226:r=127;g=0;b=95;break;
	case 227:r=127;g=63;b=111;break;
	case 228:r=76;g=0;b=57;break;
	case 229:r=76;g=38;b=66;break;
	case 230:r=255;g=0;b=127;break;
	case 231:r=255;g=127;b=191;break;
	case 232:r=204;g=0;b=102;break;
	case 233:r=204;g=102;b=153;break;
	case 234:r=153;g=0;b=76;break;
	case 235:r=153;g=76;b=114;break;
	case 236:r=127;g=0;b=63;break;
	case 237:r=127;g=63;b=95;break;
	case 238:r=76;g=0;b=38;break;
	case 239:r=76;g=38;b=57;break;
	case 240:r=255;g=0;b=63;break;
	case 241:r=255;g=127;b=159;break;
	case 242:r=204;g=0;b=51;break;
	case 243:r=204;g=102;b=127;break;
	case 244:r=153;g=0;b=38;break;
	case 245:r=153;g=76;b=95;break;
	case 246:r=127;g=0;b=31;break;
	case 247:r=127;g=63;b=79;break;
	case 248:r=76;g=0;b=19;break;
	case 249:r=76;g=38;b=47;break;
	case 250:r=51;g=51;b=51;break;
	case 251:r=91;g=91;b=91;break;
	case 252:r=132;g=132;b=132;break;
	case 253:r=173;g=173;b=173;break;
	case 254:r=214;g=214;b=214;break;
	case 255:r=255;g=255;b=255;break;
	}
	return RGB(r,g,b);
}