#include <graphics.h>              // ����ͼ�ο�ͷ�ļ�
#include <conio.h>
#include <windows.h>				//�õ��˶�ʱ����sleep()
#include <math.h>
#include <string.h>

#include <iostream>
#include "stdio.h"
#include "CfgFileParms.h"
#include "function.h"

/****

int r[3][4] = { {30,20,130,60},{170,20,220,60},{260,20,310,60} };//������ť�Ķ�ά����
float alpha, R, h0, sim_t;//��ײʱ��������ʧ�ʣ���İ뾶����ʼ�߶ȡ�����ʱ��

MOUSEMSG m;

//��ť�жϺ���
int button_judge(int x, int y)
{
	if (x > r[0][0] && x<r[0][2] && y>r[0][1] && y < r[0][3])return 1;
	if (x > r[1][0] && x<r[1][2] && y>r[1][1] && y < r[1][3])return 2;
	if (x > r[2][0] && x<r[2][2] && y>r[2][1] && y < r[2][3])return 3;
	return 0;
}
//��ʼ��ͼ��
void init_figure()
{
	int i;
	setrop2(R2_COPYPEN);//��ǰ��ɫ
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID);//ʵ��
	rectangle(30, 100, 420, 330);//�����
	setlinestyle(PS_DOT);//����
	for (i = 30 + 39; i < 420; i += 39)
	{
		line(i, 100, i, 330);//��ֱ������
	}
	for (i = 100 + 23; i < 330; i += 23)
	{
		line(30, i, 420, i);//ˮƽ������
	}
}



void show_main_win(void)
{
	short win_width, win_height;//���崰�ڵĿ�Ⱥ͸߶�
	win_width = 480; win_height = 360;
	initgraph(win_width, win_height);//��ʼ�����ڣ�������
	
	RECT R1 = { r[0][0],r[0][1],r[0][2],r[0][3] };
	RECT R2 = { r[1][0],r[1][1],r[1][2],r[1][3] };
	RECT R3 = { r[2][0],r[2][1],r[2][2],r[2][3] };

	LOGFONT f;//������ʽָ��
	gettextstyle(&f);					//��ȡ������ʽ
	_tcscpy(f.lfFaceName, _T("����"));	//��������Ϊ����
	f.lfQuality = ANTIALIASED_QUALITY;    // �������Ч��Ϊ�����  
	settextstyle(&f);                     // ����������ʽ
	settextcolor(BLACK);				//BLACK��graphic.hͷ�ļ����汻����Ϊ��ɫ����ɫ����
	drawtext("������", &R1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�ھ�������R1���������֣�ˮƽ���У���ֱ���У�������ʾ
	drawtext("��ͼƬ", &R2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�ھ�������R2���������֣�ˮƽ���У���ֱ���У�������ʾ
	drawtext("������", &R3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�ھ�������R3���������֣�ˮƽ���У���ֱ���У�������ʾ
	setlinecolor(BLACK);
	rectangle(r[0][0], r[0][1], r[0][2], r[0][3]);
	rectangle(r[1][0], r[1][1], r[1][2], r[1][3]);
	rectangle(r[2][0], r[2][1], r[2][2], r[2][3]);
	
}

int win_case_judge(void)
{
	m = GetMouseMsg();

	switch (button_judge(m.x, m.y))
	{
		//��ԭ��ťԭ��
	case 1:
		char dst[1];
		int dst;
		InputBox(dst, 1, "���������Ӷ���");
		sscanf(dst, "%d", &dst);//�������ַ�������ɨ�赽ȫ�ֱ�������
		FlushMouseMsgBuffer();//�����¼�����������Ϣ
		break;
	case 2:
		
		FlushMouseMsgBuffer();//�����¼�����������Ϣ
		break;
	case 3:
		
		FlushMouseMsgBuffer();//�����¼�����������Ϣ
	default:
		FlushMouseMsgBuffer();//�����¼�����������Ϣ
		//printf("\r\n(%d,%d)",m.x,m.y);//��ӡ������꣬�������ʱȷ������
		break;
	}

}


void txt_in(char* s)
{
	InputBox(s, 100, "���������");
}


****/