//apptester�Ĺ����ļ�
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include<string.h>
#include <graphics.h>              // ����ͼ�ο�ͷ�ļ�
#include <windows.h>
#include "winsock.h"
#include "stdio.h"
#include "CfgFileParms.h"
#include "function.h"

//#pragma comment (lib,"EasyXa.lib")

#define TOTAL_RECV_WIN_SIZE 1000  // ���մ��ڴ�С
#define MAX_SEND_PACK_SIZE 100    // һ��������������غ�,���ܴ���107����Űɣ��ַ�������
using namespace std;

U8* autoSendBuf;        //������֯�������ݵĻ��棬��СΪMAX_BUFFER_SIZE,���������������������ƣ��γ��ʺϵĽṹ��������û��ʹ�ã�ֻ������һ��
int printCount = 0; //��ӡ����
int spin = 1;  //��ӡ��̬��Ϣ����

//------�����ķָ��ߣ�һЩͳ���õ�ȫ�ֱ���------------
int iSndTotal = 0;  //������������
int iSndTotalCount = 0; //���������ܴ���
int iSndErrorCount = 0;  //���ʹ������
int iRcvTotal = 0;     //������������
int iRcvTotalCount = 0; //ת�������ܴ���
int iRcvUnknownCount = 0;  //�յ�������Դ�����ܴ���

//**************SPIRIT******************
int OK_to_commu = 0;  //��¼��ǰ�������
int handshake_case = 0;  //��¼�������
int handshake_load;  //��¼��һ���Լ������ĸ�λ��
U8* handshake_Recvbuf = (U8*)malloc(2);  //һ���ֽڵĴ洢�ռ䣬�洢����ʱ�������,��һ���������ǿ����ֶΣ��ڶ���������
int handshake_break = 0;  // ��¼û�ж೤ʱ��û���յ���Ϣ�ˣ���200��20�룩�ͻ������ӶϿ�;��OK_to_commnu��Ϊ1��ʱ��ÿ���յ���Ӧ��Ӧ�ͻ���������
int handshake_load_updataed = 0;
int commu_mod = 7;  // 7�ǳ�ʼ������ʾmodʲô�����ǣ�0��ʾ����ģʽ��1��ʾ����ģʽ
int dst_IP;         // ��¼�Է���˭
int DATA_type = 7;  // ��¼��ǰͨ�ŵ��������ͣ�7��ʾʲô�����ǣ�0��ͼ��1���ı�

// ���ӻ�����
int r[5][4] = { {30,20,130,60},{170,20,220,60},{260,20,310,60},{30,80,130,120},{170,80,220,120} };//������ť�Ķ�ά����
MOUSEMSG m;




//����ģʽ
struct
{
	int ack_Timeout = 0;       //������ʱack
	int front;                  //����ǰ
	int tail;                   //���Ժ�
	long int front_seq;         //ǰ��Ӧ��seq
	long int tail_seq;          //���Ӧ��seq
	int w_p;               //д��λָ��
	int err_tail;          //��front�����ʧ֡��β��
	int total_win_size;    //�ܴ���С������
	int write_time;        //д���ڴ�����������֡һ��ack
	int win_size;          //ʣ�ര��
	int file_open;         //�ļ��Ƿ��
	int start_time;        //��¼���ݴ��俪ʼ��ʱ��printCount�Ƕ���
	int finish_time;       //��¼���ݴ������ʱ���printfCount�Ƕ���
	int dst_IP;            //��¼˭�ں���ͨ��
	int Info_init = 7;
	int OK_to_fin;       //����Ͽ�

}RecvInfo;                 //���շ�����Ϣ��
U8 RecvWin[TOTAL_RECV_WIN_SIZE];
FILE* fp;

//����ģʽ
struct
{
	int Info_init = 7;
	long int abs_front_seq;     //����ǰ�˶�Ӧ��seq
	long int abs_tail_seq;      //����β�˶�Ӧ��seq
	long int ready_seq;         //�����ݵ�seq
	int linear;                 //ָ�������������Ľ��ޣ�ӵ������
	int max_pack_size;          //һ�����ݰ������غɲ�������ֽ���
	int sendTimeout = 0;        //ʹ�����֮��ķ�����ʱ���࣬sendCounter��ʵҲ��������Ͷ˵��Լ��ļ�����


	int winCounter;             //��ǰ���ڳ�ʱ�ط�����
	int winTimeout;             // ��¼����ʲôʱ��ﵽ��ʱ
	long int recv_ack;          //��¼��һ��ack
	U8 recv_len;                //��¼��һ��ack����len
	int recv_win_size;          //��¼��һ��ack���е�ʣ�ര�ڴ�С
	int resend_switch;          //�����ж��Ƿ���Ҫִ���ط�ĳ������
	int file_open;              //���ڼ�¼�ļ���û�б���
	long int file_size;         //���ڼ�¼�ļ���С
	long int scale;             //���ڼ�¼��˭�Ӽ���
	int OK_to_fin = 0;            //�յ������FIN�����ǿ����ֶ�4���ͻῪ��
	int dst_IP;                 //��¼˭�ں���ͨ��


}SendInfo;                      //���ͷ�����Ϣ��
U8 DATA_send[MAX_SEND_PACK_SIZE];              //���ڻ��淢������
U8 DATA_resend[MAX_SEND_PACK_SIZE];            //���ڻ����ط�����
FILE* fr;
//**************GABRIEL*****************
// https://blog.csdn.net/weixin_43488958/article/details/103892744?utm_medium=distribute.pc_relevant.none-task-blog-baidujs_title-0&spm=1001.2101.3001.4242
// ����base64 �� ͼƬ��ת���֣�������վ�ϵģ���������Ի�ο���**************************************************
unsigned int image_size_max = 100000; // �о��е�������
const char* base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char* base64_encode(const unsigned char* bindata, char* base64, int binlength);
int base64_decode(const char* base64, unsigned char* bindata); // ��ô���ڱ���û�ж��壿��Ϊʲô��ȥ�������һ�δ��룿��
void code_image(void);
void decode_image(void);




void print_statistics();
void menu();

int SA_DATA(U8* sabuf, U8* DATA, U8 dst, U8 control, long int* seq, long int* ack, U8 len);
void connection_break(void);
void recv_connection_break(void);

//***************��Ҫ��������******************************
//���ƣ�InitFunction
//���ܣ���ʼ�������棬��main�����ڶ��������ļ�����ʽ������������ǰ����
//���룺
//�����
void InitFunction(CCfgFileParms& cfgParms)
{
	int i;
	int retval;

	retval = cfgParms.getValueInt(autoSendTime, (char*)"autoSendTime");
	if (retval == -1 || autoSendTime == 0) {
		autoSendTime = DEFAULT_AUTO_SEND_TIME;
	}
	retval = cfgParms.getValueInt(autoSendSize, (char*)"autoSendSize");
	if (retval == -1 || autoSendSize == 0) {
		autoSendSize = DEFAULT_AUTO_SEND_SIZE;
	}

	autoSendBuf = (char*)malloc(MAX_BUFFER_SIZE);
	if (autoSendBuf == NULL) {
		cout << "�ڴ治��" << endl;
		//����������Ҳ̫���˳���
		exit(0);
	}
	for (i = 0; i < MAX_BUFFER_SIZE; i++) {
		autoSendBuf[i] = 'a'; //��ʼ������ȫΪ�ַ�'a',ֻ��Ϊ�˲���
	}
	return;
}
//***************��Ҫ��������******************************
//���ƣ�EndFunction
//���ܣ����������棬��main�������յ�exit������������˳�ǰ����
//���룺
//�����
void EndFunction()
{
	if (autoSendBuf != NULL)
		free(autoSendBuf);
	return;
}
//
//
//
//
//
//
//
//
//
//
//***************��Ҫ��������******************************
//���ƣ�TimeOut
//���ܣ�������������ʱ����ζ��sBasicTimer�����õĳ�ʱʱ�䵽�ˣ�
//      �������ݿ���ȫ���滻Ϊ������Լ����뷨
//      ������ʵ���˼���ͬʱ���й��ܣ����ο�
//      1)����iWorkMode����ģʽ���ж��Ƿ񽫼�����������ݷ��ͣ������Զ����͡������������ʵ��Ӧ�ò��
//        ��Ϊscanf�����������¼�ʱ���ڵȴ����̵�ʱ����ȫʧЧ������ʹ��_kbhit()������������ϵ��ڼ�ʱ�Ŀ������жϼ���״̬�������Get��û��
//      2)����ˢ�´�ӡ����ͳ��ֵ��ͨ����ӡ���Ʒ��Ŀ��ƣ�����ʼ�ձ�����ͬһ�д�ӡ��Get��
//      3)�����iWorkMode����Ϊ�Զ����ͣ���ÿ����autoSendTime * DEFAULT_TIMER_INTERVAL ms����ӿ�0����һ��
//���룺ʱ�䵽�˾ʹ�����ֻ��ͨ��ȫ�ֱ�����������
//���������Ǹ�����Ŭ���ɻ����ʵ����
void TimeOut() // 100ms����һ��
{
	int iSndRetval;
	int len;
	U8 sendBuf[3];
	int i;

	printCount++;
	handshake_break++;
	if (commu_mod == 1)
		RecvInfo.ack_Timeout++;
	//printf("hb��%d",handshake_break);
	if (commu_mod == 0)
		SendInfo.sendTimeout++;


	if ((OK_to_commu == 1) || (handshake_case == 1) || (handshake_case == 2) || (handshake_case == 3))
	{
		if (SendInfo.OK_to_fin == 1) // һ�����ݴ������
		{
			SendInfo.Info_init = 7;  // ��δ��ʼ��
			DATA_type = 7;
			SendInfo.OK_to_fin = 0;
			commu_mod = 6;
			if (SendInfo.file_open == 1)
			{
				fclose(fr);
				printf("�ر�txt�ļ�\n");
				SendInfo.file_open = 0;
			}
		}
		if (RecvInfo.OK_to_fin == 1)
		{
			RecvInfo.OK_to_fin = 0;
			commu_mod = 6;
			RecvInfo.Info_init = 7;
			if (RecvInfo.file_open == 1)
			{
				fclose(fp);
				printf("�ر�txt�ļ�\n");
				RecvInfo.finish_time = printCount;
				if (DATA_type == 0) // ������ͼ
				{
					decode_image();
					printf("�ѽ���������Ϊjpg\n");
					char pic_name[80] = "D:\\wangke\\COMPUTER_NETWORKS\\project1_code\\output.jpg";
					char cmd[100];
					sprintf(cmd, "mspaint %s", pic_name);
					system(cmd); // ��ʾͼƬ
					/*initgraph(450, 450);
					IMAGE img;
					loadimage(&img, "D:\\wangke\\COMPUTER_NETWORKS\\project1_code\\output.jpg",450,450);
					putimage(0, 0, &img);*/
				}
				if (DATA_type == 1)
				{
					fp = fopen("D:\\wangke\\COMPUTER_NETWORKS\\project1_code\\recv.txt", "r");
					fseek(fp, 0, SEEK_END);
					int f_len = ftell(fp);
					fseek(fp, 0, SEEK_SET);
					U8* s = (U8*)malloc(f_len + 4);
					/*U8* s_copy = (U8*)malloc(f_len + 20);
					sprintf(s_copy+10, " %s", s);
					s_copy[0] = (char)dst_IP;*/
					s[0] = dst_IP + 48;
					s[1] = ':';
					s[2] = ' ';
					if (s == NULL)
						return;
					fread(s + 3, f_len, 1, fp);
					s[f_len + 3] = '\0';
					MessageBox(NULL, s, "��Ϣ", MB_OK | MB_ICONINFORMATION);
					printf("\n*********************************************");
					printf("\n%dԻ��%s\n", dst_IP, s + 3);
					printf("*********************************************\n");
					free(s);
					fclose(fp);
				}

				//printf("����ͨ��ƽ�����ݴ�������ԼΪ%.1f Kbs\n", RecvInfo.front_seq * 8 * 10 / (float)(RecvInfo.finish_time - RecvInfo.start_time - 50) / 1000.0);


			}
			RecvInfo.file_open = 0;
			DATA_type = 7;

		}
		if (SendInfo.winCounter > 5 || handshake_break % 2000 == 0)
			//�����ط���ô��Σ������ϣ�   ��ʱ��˵���������ϣ�
		{
			OK_to_commu = 0;
			handshake_case = 0;  //���ӳ�ʱ�Ͽ�
			iWorkMode = 0;
			commu_mod = 7;
			SendInfo.Info_init = 7;  // ��δ��ʼ��
			handshake_load_updataed = 0;


			RecvInfo.Info_init = 7;


			if (SendInfo.file_open == 1)
			{
				fclose(fr);
				printf("�ر�txt�ļ�\n");
			}
			if (RecvInfo.file_open == 1)
			{
				fclose(fp);
				printf("�ر�txt�ļ�\n");
				RecvInfo.finish_time = printCount;
				if (DATA_type == 0) // ������ͼ
				{
					decode_image();
					printf("�ѽ���������Ϊjpg\n");
				}

				//printf("����ͨ��ƽ�����ݴ�������ԼΪ%.1f Kbs\n",RecvInfo.front_seq*8*10/(float)(RecvInfo.finish_time-RecvInfo.start_time-50)/1000.0);


			}
			SendInfo.file_open = 0;
			RecvInfo.file_open = 0;
			DATA_type = 7;
			printf("����,handshake_case:%d, OK_to_commu:%d\n", handshake_case, OK_to_commu);
			connection_break();
		}
	}

	if (_kbhit()) {
		//�����ж���������˵�ģʽ
		//menu();
		win_case_judge();
		while (_kbhit())
		{
			_getch();
		}

	}



	//���ͷ���************************************************************************
	//���ͷ���û�п���������������ʼ����Ϣ������������Ϣ���ĳ�ʼ��������txt���洢��Ҫ�����͵�����
	if (commu_mod == 0 && iWorkMode == 0 && SendInfo.Info_init == 1)  //��δ���������ģʽ����ѡ���˷�����Ƭ
	{
		//����txt������Ϣ
		SendInfo.Info_init = 0;
		if (DATA_type == 0)
		{
			code_image();  // ����ͼƬ
		}
		fr = fopen("D:\\wangke\\COMPUTER_NETWORKS\\project1_code\\send.txt", "r");  // ��������txt
		if (fr == NULL)
		{
			printf("send�ļ���ȡʧ��\n");
			iWorkMode = 0;
			SendInfo.Info_init = 1;   // �����ʼ��
			return;
		}
		//commu_mod = 0;
		printf("���ͷ�����Ϣ����ʼ��\n");
		fseek(fr, 0, SEEK_END);
		SendInfo.file_size = ftell(fr);// ��֪�ļ��е����ݳ���
		printf("���ݳ���:%ld", SendInfo.file_size);
		fseek(fr, 0, SEEK_SET);
		SendInfo.abs_front_seq = 1;    // ָ��DATA�ĵ�һ���ֽڣ���Ȼ��һ��ʼ���򣡣�
		SendInfo.abs_tail_seq = 2;     // ָ��ǰ����β��������ֽ�������������棬�������ͣ�
		SendInfo.ready_seq = 1;        // ָ�������ݵ�ͷ
		SendInfo.file_open = 1;        // �ļ�����
		SendInfo.linear = 400;         // ��ʼ��ָ-�߽���Ϊ200�ֽ�
		SendInfo.max_pack_size = MAX_SEND_PACK_SIZE;  // ��������غ�Ϊ100�ֽ�
		SendInfo.recv_ack = -1;        // ��ʼ��һ������ֵֹĶ���
		SendInfo.recv_len = 0;
		SendInfo.recv_win_size = 450;  // ����ϣ�������ʣ�ര���㹻�Ĵ�
		SendInfo.resend_switch = 0;    // ��û���ط��ز�
		SendInfo.sendTimeout = 0;      // ��5�ı����ͻᷢһ��������
		SendInfo.winCounter = 0;       // sendCounter��Ҫ�ط��������ݣ�������������Ҫ�󴰿��ط�����Ȼ�ͻ�Ͽ�����
		SendInfo.winTimeout = 0;       // ��¼���ڿ�ס��ʱ��
		SendInfo.scale = 0;            // ���ڼ�¼��һ�������Ӽ�����ack
		SendInfo.OK_to_fin = 0;      //һ��ʼ�϶����öϿ���
		handshake_break = 0;           //�ȱ��Զ�����
		//��ʼ��������
		memset(DATA_send, (char)-1, MAX_SEND_PACK_SIZE);
		memset(DATA_resend, (char)-1, MAX_SEND_PACK_SIZE);


	}
	//�����뷢�Ͱ�
	if (iWorkMode == 7 && handshake_break % 50 == 0 && OK_to_commu == 1)
	{
		sendBuf[1] = 32;  // 0010,0000
		sendBuf[2] = 0;
		sendBuf[0] = dst_IP;
		SendtoLower(sendBuf, 3, 0);
		printf("����ͼƬ������...\n");
	}
	if (iWorkMode == 3 && handshake_break % 50 == 0 && OK_to_commu == 1)
	{
		sendBuf[1] = 64;  // 0100,0000
		sendBuf[2] = 0;
		sendBuf[0] = dst_IP;
		SendtoLower(sendBuf, 3, 0);
		printf("��������������...\n");
	}


	//��ʱack
	if (OK_to_commu == 1 && RecvInfo.ack_Timeout % 8 == 0 && commu_mod == 1)
	{
		// ��ʱǰ��дһ���ļ�
		for (i = 0; i < RecvInfo.total_win_size - 1; i++)
		{
			//��һ��Ϊ'?'�Ͳ���ǰ����
			if (RecvWin[(RecvInfo.front + 1) % RecvInfo.total_win_size] == (char)-1)
			{
				//printf("frontǰ��һ��Ϊ'?'\n");
				break;
			}
			//��һ��ΪtailҲ����ǰ��
			if ((RecvInfo.front + 1) % RecvInfo.total_win_size == RecvInfo.tail)
			{
				//printf("frontǰ��һ��Ϊtail\n");
				break;
			}
			// ǰ��
			//printf("frontǰ��һ��\n");
			RecvInfo.front = (RecvInfo.front + 1) % RecvInfo.total_win_size;
			RecvInfo.front_seq += 1;
			//printf("����front��%d,д���ļ���%c ",RecvInfo.front, *(RecvWin + RecvInfo.front));
			fwrite(RecvWin + RecvInfo.front, sizeof(U8), 1, fp); //д��һ���ֽ�
			RecvWin[RecvInfo.front] = (char)-1;  // ���ڳ�ʼ��
		}

		printf("ackTimeout\n");
		long int seq = RecvInfo.front - RecvInfo.tail - 1;
		if (seq < 0)
			seq = RecvInfo.total_win_size + seq; // ʣ�ര�ڴ�С
		long int ack = RecvInfo.front_seq + 1;
		int len = RecvInfo.err_tail - RecvInfo.front;
		if (len < 0)
			len = len + RecvInfo.total_win_size;
		if (len > 100)
			len = 100;
		U8* sabuf = (U8*)malloc(19);
		if (sabuf == NULL)
			return;
		SA_DATA(sabuf, NULL, (char)dst_IP, 1, &seq, &ack, (U8)len);
		printf("��ʱack����ackΪ%ld\n", ack);
		free(sabuf);
	}




	//���ܷ�****************************************************************************
	//���շ���Ϣ��ʼ��
	if (OK_to_commu == 1 && commu_mod == 1 && RecvInfo.Info_init == 1) // ���뷢��״̬��
	{
		RecvInfo.Info_init = 0;        // ��ֹ�����Ϣ���ĳ�ʼ��
		//�������ļ���������Ϣ
		fp = fopen("D:\\wangke\\COMPUTER_NETWORKS\\project1_code\\recv.txt", "wb+");
		if (fp == NULL)
		{
			printf("�����ļ�ʧ��\n");
			commu_mod = 7;            //��ȥ����
			RecvInfo.Info_init = 1;   //�������³�ʼ����Ϣ��
			return;
		}
		//��ʼ����Ϣ��
		RecvInfo.file_open = 1;
		//�������ģʽ��֮�󲻻������Ϣ����ʼ��
		commu_mod = 1;
		handshake_break = 0;
		printf("�������ģʽ\n");
		//��ʼ����Ϣ��
		printf("��ʼ��������Ϣ����\n");
		RecvInfo.ack_Timeout = 0;
		RecvInfo.err_tail = 1;
		RecvInfo.front = 0;
		RecvInfo.front_seq = 0;
		RecvInfo.tail = 1;
		RecvInfo.tail_seq = 1;
		RecvInfo.total_win_size = TOTAL_RECV_WIN_SIZE;  // ���մ��ڴ�С
		RecvInfo.write_time = 0;
		RecvInfo.w_p = 0;
		RecvInfo.win_size = TOTAL_RECV_WIN_SIZE - 1;
		RecvInfo.start_time = 0;
		RecvInfo.finish_time = 0;
		RecvInfo.OK_to_fin = 0;   // ���öϿ�
		RecvInfo.start_time = printCount;   //��¼��ʼʱ��
		//��ʼ������
		memset(RecvWin, (char)-1, RecvInfo.total_win_size);
		//test
		/*U8* sabuf = (U8*)malloc(118);
		U8  _DATA[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		long int seq = 1;
		long int ack = 0;
		U8 len = 100;
		if (sabuf == NULL)
			return;
		SA_DATA(sabuf, _DATA, 2, &seq, &ack, len);
		free(sabuf);*/
	}
	//���ͷ���Timeout
	if (OK_to_commu == 1 && commu_mod == 0 && SendInfo.Info_init == 0) // ���ֳɹ����Ϊ����ȡ������Ϣ���Ѿ�����ʼ����ǰ����
	{
		int i;
		//���ڳ�ʱ�ط�Ӧ��д�������
		if (SendInfo.ready_seq >= SendInfo.abs_tail_seq && SendInfo.ready_seq < SendInfo.file_size)  // ���Ǵ��ڿ��������,ֻҪfront�ܶ���winCounter��winTimeout��������
		{
			SendInfo.winTimeout++;
			if (SendInfo.winTimeout % 200 == 0)  //���ڳ�ʱ
			{
				SendInfo.ready_seq = SendInfo.abs_front_seq; // ָ��ز�
				printf("ָ��ز�\n");
				SendInfo.winCounter++;
				SendInfo.winTimeout = 0;
			}
		}

		//�Ƿ���Ҫ���͡��¡�����
		if (SendInfo.ready_seq < SendInfo.abs_tail_seq && SendInfo.sendTimeout % 5 == 0)  // �������ݷ��ͣ����ҵ��˷��°���ʱ����
		{
			/*int send_time = 2;
			if (SendInfo.recv_win_size < 4)
				send_time = 4;*/

			if (SendInfo.sendTimeout % 1 == 0)
			{
				//ͳ�Ʒ�����Ϣ��д����
				U8 control = 2; // ����һ��ʹseq�ֶ���Ч�İ�
				long int seq = SendInfo.ready_seq;
				long int ack = 0;
				U8 base = 0;
				U8 len;
				if (SendInfo.abs_tail_seq - SendInfo.ready_seq >= SendInfo.max_pack_size)
					len = SendInfo.max_pack_size;
				else
					len = (U8)(SendInfo.abs_tail_seq - SendInfo.ready_seq);
				if (SendInfo.file_size - SendInfo.ready_seq < 100 && SendInfo.abs_tail_seq >= SendInfo.file_size)
					len = SendInfo.file_size - SendInfo.ready_seq + 1;
				U8* sabuf = (U8*)malloc(len + 19); // ׼�������ڷ��͵�sabuf
				if (sabuf == NULL)
					return;
				if (SendInfo.recv_ack > SendInfo.file_size)// || SendInfo.ready_seq > SendInfo.file_size)
				{
					printf("������ϣ��յ���ack�����ļ���С��ready��%d\n", SendInfo.ready_seq);
					//Ϊ�˷�ֹ�����ȣ�������һ����ʾ���������
					U8* sabuf = (U8*)malloc(19);//��װ��һ��ͷ������
					if (sabuf == NULL) return;
					U8 control = 4;             // 0000,0100
					long int seq = 0;
					long int ack = 0;
					SA_DATA(sabuf, NULL, (U8)dst_IP, control, &seq, &ack, 0);
					free(sabuf);
					return;                              // ����Ĺ�������������İ�
				}
				//����Ӧ��Ϣд�뻺��DATA��
				//��ʹֻ���ļ�ָ��ƫ�Ƶ�ready_seq��
				fseek(fr, SendInfo.ready_seq - 1, SEEK_SET);  // ע���ͷ��ʼƫ�ƣ�����Ҫ��ȥ1����Ϊseq��1��ʼ
				//Ȼ��д�뻺��
				fread(DATA_send, (int)len, 1, fr); // ��fr����һ���ֽ�Ϊ��λ��len��ô��������д��DATA_send��
				//printf("������seqΪ%ld�������غɳ���lenΪ:%d,���ڵ�ͷ��%ld,���ڵ�β��%ld,ready��%ld\n", seq, len,SendInfo.abs_front_seq,SendInfo.abs_tail_seq,SendInfo.ready_seq);
				SA_DATA(sabuf, DATA_send, (U8)dst_IP, control, &seq, &ack, len);   //����ͷ�����Է���
				free(sabuf);
				SendInfo.ready_seq += (int)len;  // readyָ�����ǰ����
				handshake_break = 0;
			}


		}

		//Ȼ����ack��
		//���ڿ����ƶ���
		if (SendInfo.recv_ack > SendInfo.abs_front_seq)
		{
			long int tmp = SendInfo.abs_front_seq;
			SendInfo.abs_front_seq = SendInfo.recv_ack;
			SendInfo.abs_tail_seq += (SendInfo.recv_ack - tmp); // ��֤���ڴ�С������ƶ�
			if (SendInfo.abs_tail_seq > SendInfo.file_size)            // β����ô���ܳ����ļ�β����
				SendInfo.abs_tail_seq = SendInfo.file_size + 1;
			if (SendInfo.abs_front_seq >= SendInfo.abs_tail_seq)
			{
				printf("������ϣ�ͷ��%ld,β��%ld,ͷ���ڵ���β\n", SendInfo.abs_front_seq, SendInfo.abs_tail_seq);
				//Ϊ�˷�ֹ�����ȣ�������һ����ʾ���������
				U8* sabuf = (U8*)malloc(19);//��װ��һ��ͷ������
				if (sabuf == NULL) return;
				U8 control = 4;             // 0000,0100
				long int seq = 0;        // Ƥ����
				long int ack = 0;
				SA_DATA(sabuf, NULL, (U8)dst_IP, control, &seq, &ack, 0);
				free(sabuf);
				SendInfo.OK_to_fin = 1;
				//return;                          // ����Ĺ�������������İ�
			}
			SendInfo.winTimeout = 0;
			SendInfo.winCounter = 0;
		}
		//������Ҫ�ش���
		if (SendInfo.resend_switch >= 3) //���ack���˺ö����
		{
			//printf("���ڴ����ش�......\n");
			long int resend_wp = SendInfo.recv_ack;
			U8* sabuf = (U8*)malloc(19 + (SendInfo.recv_len));
			U8 control = 2;
			long int seq = resend_wp;
			long int ack = 0;
			U8 len = SendInfo.recv_len;
			if (len > SendInfo.max_pack_size)
				len = SendInfo.max_pack_size;
			if (resend_wp + len >= SendInfo.ready_seq)
				len = SendInfo.ready_seq - resend_wp; // resend_wp���ܳ���ready
			if (sabuf == NULL)
				return;
			//����Ӧ��Ϣд�뻺��DATA��
			//��ʹֻ���ļ�ָ��ƫ�Ƶ�ready_seq��
			fseek(fr, resend_wp - 1, SEEK_SET);  // ע���ͷ��ʼƫ�ƣ�����Ҫ��ȥ1
			//Ȼ��д�뻺��
			fread(DATA_resend, (int)len, 1, fr); // ��fr����һ���ֽ�Ϊ��λ��len��ô��������д��DATA_send��
			SA_DATA(sabuf, DATA_resend, (U8)dst_IP, control, &seq, &ack, len);   //����ͷ�����Է���
			free(sabuf);
			SendInfo.resend_switch = 0;
			//handshake_break = 0;
		}
		//����ack������Ϣ�����ڴ�С
		if (SendInfo.abs_tail_seq - SendInfo.abs_front_seq > SendInfo.linear && SendInfo.scale != SendInfo.recv_ack)
			// �����ǰ���ڴ���ָ-�߽��ޣ����󴰿�ֻ�����ԣ����Ǽ��ٴ����Ǳ�����
		{
			SendInfo.scale = SendInfo.recv_ack;
			if (SendInfo.recv_win_size <= 200 || SendInfo.winCounter > 3 || SendInfo.recv_win_size < 4)
			{
				SendInfo.abs_tail_seq = (SendInfo.abs_front_seq + SendInfo.abs_tail_seq) / 2 + 1;   // ��������
				//printf("��������\n");

				if (SendInfo.linear < 10)
					SendInfo.linear = 10;    //����̫С
				else
					SendInfo.linear -= 10;
			}
			if (SendInfo.recv_win_size > 200 && SendInfo.resend_switch < 1)
			{
				//printf("΢С�ӱ�\n");
				SendInfo.abs_tail_seq += 10;
			}
		}
		if ((SendInfo.abs_tail_seq - SendInfo.abs_front_seq <= SendInfo.linear && SendInfo.scale != SendInfo.recv_ack))//(SendInfo.abs_tail_seq - SendInfo.abs_front_seq <= SendInfo.linear) // ��ʱ�������Ǽӱ���
		{
			SendInfo.scale = SendInfo.recv_ack;
			if (SendInfo.recv_win_size > 400 && SendInfo.resend_switch < 1)
			{
				//printf("�ӱ�ǰͷ����%ld��β����%ld ", SendInfo.abs_front_seq, SendInfo.abs_tail_seq);
				SendInfo.abs_tail_seq = SendInfo.abs_tail_seq + (SendInfo.abs_tail_seq - SendInfo.abs_front_seq);  // �����ӱ�
				//printf("�����ӱ�,β������%ld\n", SendInfo.abs_tail_seq);
			}
		}


	}





	//�����֣����Ƕ��Ǻ�����**************************************************************
	switch (iWorkMode / 10) // iWorkModeʮλ 
	{
	case 0:  // ���������˵����徫���
		if (handshake_case == 2 && handshake_break % 20 == 0) // �����������ְ���ʱ�ش������Ƕ��ղ���33���ĳ�ʱ�ش�
		{
			sendBuf[2] = handshake_load + (handshake_Recvbuf[1] + 1) * 10; // ���յ�������һȻ��ŵ�ʮλ
			sendBuf[1] = 22;  // �����ֶ�Ϊ22����ʾ���͵��ǵڶ������ְ�
			sendBuf[0] = dst_IP;
			SendtoLower(sendBuf, 3, 0);
			//printf("\n��������ʱ�ش������͵���%d\n",sendBuf[3]);
		}
		break;
	case 1:
		// ѡ��1����ʼ��������
		switch (OK_to_commu)
		{
		case 1:  //�Ѿ�����������
			break;
		case 0:  //��û��������
			switch (handshake_case)
			{
			case 0:
				if (handshake_load_updataed == 0)
				{
					handshake_load = rand() % 8;  //����һ��[0,7]��int��
					handshake_load_updataed = 1;
				}


				sendBuf[2] = handshake_load;
				sendBuf[1] = 11;  // �����ֶ�Ϊ11����ʾ���͵��ǵ�һ�����ְ�������ȷ������
				sendBuf[0] = dst_IP;
				SendtoLower(sendBuf, 3, 0);
				handshake_case = 1;
				handshake_break = 0;
				break;
			case 1: // ��������������ȡ���ϰ�
				if (handshake_break % 10 == 0)
				{
					sendBuf[2] = handshake_load;
					sendBuf[1] = 11;  // �����ֶ�Ϊ11����ʾ���͵��ǵ�һ�����ְ�������ȷ������
					sendBuf[0] = dst_IP;
					SendtoLower(sendBuf, 3, 0);
					//printf("\n���ֳ�ʱ�ش�,�ش�����%d\n",sendBuf[2]);
				}
				break;

			case 2:  //���ڱ�������iWorkmodeû�����ã��������caseֻ���������Ż�����
				sendBuf[2] = handshake_Recvbuf[1] % 10 + 1;
				sendBuf[1] = 33;  // �����ֶ�Ϊ33����ʾ���͵��ǵ��������ְ�
				sendBuf[0] = dst_IP;
				SendtoLower(sendBuf, 3, 0);
				handshake_case = 3;
				OK_to_commu = 1;  //���������������ӵ�һ����������Ϊ���ӽ�����
				handshake_break = 0;
				printf("������������OK�����͵���%d\n", sendBuf[2]);
				iWorkMode = 0;
				break;
			}
			break;
		case 3:
			break;
		default:
			break;
		}
		//��ʱ����, ÿ���autoSendTime * DEFAULT_TIMER_INTERVAL ms ����һ��
		/*if (printCount % autoSendTime == 0) {
			for (i = 0; i < min(autoSendSize, 8); i++) {
				//��ͷ�����ֽ���26����ĸ�м����������ڹ۲�
				autoSendBuf[i] = 'a' + printCount % 26;
			}

			len = autoSendSize; //ÿ�η�������
			if (lowerMode[0] == 0) {
				//�Զ�����ģʽ�£�ֻ��ӿ�0����
				bufSend = (U8*)malloc(len * 8);
				//�²�ӿ��Ǳ���������
				iSndRetval = ByteArrayToBitArray(bufSend, len * 8, autoSendBuf, len);
				iSndRetval = SendtoLower(bufSend, iSndRetval, 0);

				free(bufSend);
			}
			else {
				//�²�ӿ����ֽ����飬ֱ�ӷ���
				for (i = 0; i < min(autoSendSize, 8); i++) {
					//��ͷ�����ֽ���26����ĸ�м����������ڹ۲�
					autoSendBuf[i] = 'a' + printCount % 26;
				}
				iSndRetval = SendtoLower(autoSendBuf, len, 0);
				iSndRetval = iSndRetval * 8; //�����λ
			}
			//����ͳ��
			if (iSndRetval > 0) {
				iSndTotalCount++;
				iSndTotal += iSndRetval;
			}
			else {
				iSndErrorCount++;
			}
			//��Ҫ��Ҫ��ӡ����
			switch (iWorkMode % 10) {
			case 1:
				print_data_bit(autoSendBuf, len, 1);
				break;
			case 2:
				print_data_byte(autoSendBuf, len, 1);
				break;
			case 0:
				break;
			}
		}
		*/
		break;
	}

	/*if (OK_to_commu == 1) // ����һ��ͨ�ŵ�ǰ��
	{
		switch (commu_mod)
		{
		case 1: // ����ģʽ
			break;

		case 0: // ����ģʽ
			break;

		default:
			break;
		}
	}*/



	//���ڴ�ӡͳ������
	//print_statistics();

}
//------------�����ķָ��ߣ����������ݵ��շ�,--------------------------------------------

//***************��Ҫ��������******************************
//���ƣ�RecvfromUpper
//���ܣ�������������ʱ����ζ���յ�һ�ݸ߲��·�������
//      ��������ȫ�������滻��������Լ���
//      ���̹��ܽ���
//         1)ͨ���Ͳ�����ݸ�ʽ����lowerMode���ж�Ҫ��Ҫ������ת����bit�����鷢�ͣ�����ֻ�����Ͳ�ӿ�0��
//           ��Ϊû���κοɹ��ο��Ĳ��ԣ���������Ӧ�ø���Ŀ�ĵ�ַ�ڶ���ӿ���ѡ��ת���ġ�
//         2)�ж�iWorkMode�������ǲ�����Ҫ�����͵��������ݶ���ӡ������ʱ���ԣ���ʽ����ʱ�����齫����ȫ����ӡ��
//���룺U8 * buf,�߲㴫���������ݣ� int len�����ݳ��ȣ���λ�ֽ�
//�����
void RecvfromUpper(U8* buf, int len)
{
	//Ӧ�ò㲻���յ����߲㡱�����ݣ������Լ�����
}
//***************��Ҫ��������******************************
//���ƣ�RecvfromLower
//���ܣ�������������ʱ����ζ�ŵõ�һ�ݴӵͲ�ʵ��ݽ�����������
//      ��������ȫ�������滻���������Ҫ������
//      ���̹��ܽ��ܣ�
//          1)����ʵ����һ���򵥴ֱ���������Ĳ��ԣ����дӽӿ�0�����������ݶ�ֱ��ת�����ӿ�1�����ӿ�1�������Ͻ����߲㣬������ô����
//          2)ת�����Ͻ�ǰ���ж��ս����ĸ�ʽ��Ҫ���ͳ�ȥ�ĸ�ʽ�Ƿ���ͬ��������bite��������ֽ�������֮��ʵ��ת��
//            ע����Щ�жϲ������������ݱ�����������������������ļ������������ļ��Ĳ���д���ˣ��ж�Ҳ�ͻ�ʧ��
//          3)����iWorkMode���ж��Ƿ���Ҫ���������ݴ�ӡ
//���룺U8 * buf,�Ͳ�ݽ����������ݣ� int len�����ݳ��ȣ���λ�ֽڣ�int ifNo ���Ͳ�ʵ����룬�����������ĸ��Ͳ�
//�����
void RecvfromLower(U8* buf, int len, int ifNo)
{
	int retval = 0;
	if (buf == NULL)
		return;

	if (lowerMode[ifNo] == 0)
	{
		//�Ͳ���bit�������ʽ����Ҫת�����ŷ����ӡ
		/*bufRecv = (U8*)malloc(len / 8 + 1);
		if (bufRecv == NULL) {
			return;
		}
		//����ӿ�0�Ǳ��������ʽ����ת�����ֽ����飬�����ϵݽ�
		retval = BitArrayToByteArray(buf, len, bufRecv, len / 8 + 1);
		retval = len;*/
	}
	else
	{  //������APP�㣬��һ����NET(��LNK)���������ֽ��������ʽ
		switch (OK_to_commu)
		{
		case 1:
			if (buf[1] == 22)  // �յ���Ӧ��
			{
				printf("handshake_load:%d��buf[2]:%d\n", handshake_load, buf[2]);
				// �ж��Ƿ������������
				if (buf[2] / 10 == handshake_load + 1)  // �ж�ʮλ�ǲ���ԭ������ȥ������1
				{
					handshake_Recvbuf[0] = buf[1];
					handshake_Recvbuf[1] = buf[2];  // ������
					handshake_case = 2;  //���󷽽���2�׶�
					OK_to_commu = 0;
					iWorkMode = 10;
					printf("���󷽷��ؽ���2�׶�\n");
				}
			}
			break;
		case 0: // ��ʱ������δȷ��
			switch (handshake_case)
			{
			case 0:
				if (buf[1] == 11)  // �յ��������������ӵ����ְ�
				{
					handshake_Recvbuf[0] = buf[1];
					handshake_Recvbuf[1] = buf[2];  // ������
					dst_IP = buf[0];                // ����˭�ں���˵��
					// Ȼ����22��
					handshake_load = rand() % 8;  //����һ��[0,7]��int��
					U8* sendBuf = (U8*)malloc(3);
					if (sendBuf == NULL)
						return;
					sendBuf[2] = handshake_load + (handshake_Recvbuf[1] + 1) * 10; // ���յ�������һȻ��ŵ�ʮλ
					sendBuf[1] = 22;  // �����ֶ�Ϊ22����ʾ���͵��ǵڶ������ְ�
					sendBuf[0] = dst_IP;
					printf("����������2�׶�,���͵���%d\n", sendBuf[2]);
					SendtoLower(sendBuf, 3, 0);
					free(sendBuf);
					handshake_case = 2;  // �շ�����2�׶�
					handshake_break = 0;

				}
				break;
			case 1:  //�������󷽲Ż����Ľ׶�
				if (buf[1] == 22)  // �յ���Ӧ��
				{
					// �ж��Ƿ������������
					if (buf[2] / 10 == handshake_load + 1)  // �ж�ʮλ�ǲ���ԭ������ȥ������1
					{
						handshake_Recvbuf[0] = buf[1];
						handshake_Recvbuf[1] = buf[2];  // ������
						handshake_case = 2;  //���󷽽���2�׶�
						handshake_break = 0;
						printf("���󷽽���2�׶�,�յ�����%d\n", handshake_Recvbuf[1]);
					}
				}
				break;
			case 2: // �����շ���2�׶ε���
				if (buf[1] == 33) // �յ��������33��
				{
					if (buf[2] == handshake_load + 1)
					{
						handshake_case = 3;
						OK_to_commu = 1;  // �յ������֮���շ�������Ϊ����ȷ����
						handshake_break = 0;
						printf("������������OK\n");
						iWorkMode = 0;
					}
				}
				if (buf[1] == 22)
				{
					// �ж��Ƿ������������
					if (buf[2] / 10 == handshake_load + 1)  // �ж�ʮλ�ǲ���ԭ������ȥ������1
					{
						handshake_Recvbuf[0] = buf[1];
						handshake_Recvbuf[1] = buf[2];  // ������
						handshake_case = 2;  //���󷽽���2�׶�
						handshake_break = 0;
						printf("�����ش������2�׶�\n");
					}
				}
				if (buf[1] == 11)
				{
					handshake_Recvbuf[0] = buf[1];
					handshake_Recvbuf[1] = buf[2];  // ������
					// Ȼ����22��

					U8* sendBuf = (U8*)malloc(3);
					if (sendBuf == NULL)
						return;
					sendBuf[2] = handshake_load + (handshake_Recvbuf[1] + 1) * 10; // ���յ�������һȻ��ŵ�ʮλ
					sendBuf[1] = 22;  // �����ֶ�Ϊ22����ʾ���͵��ǵڶ������ְ�
					sendBuf[0] = dst_IP;
					//printf("������û�гɹ���Ӧ11�������½���2�׶�,���͵���%d\n", sendBuf[2]);
					SendtoLower(sendBuf, 3, 0);
					free(sendBuf);
					handshake_case = 2;  // �շ�����2�׶�
					handshake_break = 0;
				}
				break;
			case 3:
				break;
			default:
				break;
			}
			break;
		}

		retval = len * 8;//�����λ,����ͳ��
	}

	//Recv�����յ�ack*******************************************************
	//�����ֳɹ��󣬴���ģʽ�£��յ���ȡ������󣬽��뷢��״̬
	if (OK_to_commu == 1 /*&& commu_mod == 7*/ && (buf[1] == 32 || buf[1] == 64) && buf[0] == (U8)dst_IP)    //֪�����漴������ͼƬ��Ҫ���Ի�Ӧ��������Recv��Ϣ����ʼ��
	{
		U8 control;
		if (buf[1] == 32)
		{
			DATA_type = 0;    // ������ͼ
			control = 35;  // 35��Ϊ��Ӧ�����öԷ���������ͼƬ
		}
		else
		{
			DATA_type = 1;
			control = 67;
		}
		commu_mod = 1;            // �������ģʽ
		RecvInfo.Info_init = 1;  // ������Ϣ���ĳ�ʼ��
		printf("�����յ���Ϣ\n");
		//��Ӧ�Է����öԷ�����
		U8* sabuf = (U8*)malloc(19);

		long int seq = 0;
		long int ack = 0;
		if (sabuf == NULL)
			return;
		SA_DATA(sabuf, NULL, (U8)dst_IP, control, &seq, &ack, 0);
	}




	//Send���յ�35�����Ż�ͣ�·���"����ͼƬ�����"�����ǲ�������Ϣ��ʼ��
	if (OK_to_commu == 1 /*&& commu_mod == 7*/ && (buf[1] == 35 || buf[1] == 67) && buf[0] == (U8)dst_IP) // �յ��˿����ֶ�35
	{
		printf("��������\n");
		SendInfo.Info_init = 1;  // �����ʼ��
		iWorkMode = 0;           // ������
		commu_mod = 0;
	}
	//Send���յ�FIN��
	if (OK_to_commu == 1 && (commu_mod == 0 || commu_mod == 6) && buf[1] == 4 && buf[0] == (U8)dst_IP) // �յ��˿����ֶ�4
	{
		printf("��Ϣ�������......\n");
		SendInfo.OK_to_fin = 1;  // �����Ͽ�
	}
	//Send���յ�ACK��
	if (OK_to_commu == 1 && commu_mod == 0 && buf[1] == 1 && SendInfo.OK_to_fin != 1 && buf[0] == (U8)dst_IP)   //��Ϊ����ȡ�����յ�ack����,������
	{
		//��ack����
		if (*((long int*)(buf + 10)) == SendInfo.recv_ack)  // ���ack�ǲ��Ǻ�֮ǰ��һ��
		{
			SendInfo.resend_switch++;
		}
		SendInfo.recv_ack = *((long int*)(buf + 10));
		SendInfo.recv_win_size = *((long int*)(buf + 1));  //����seq����ʣ�ര�ڴ�С
		SendInfo.recv_len = (*(buf + 18));
		//printf("�յ�ack%ld\n", SendInfo.recv_ack);
	}
	if ((SendInfo.recv_ack >= SendInfo.file_size || SendInfo.abs_front_seq >= SendInfo.abs_tail_seq) && OK_to_commu == 1 && buf[0] == (U8)dst_IP && buf[1] == 1)
	{

		printf("�����Ѿ���ϣ�����ҷ�ack�ˣ��ٷ�fin\n");
		//Ϊ�˷�ֹ�����ȣ�������һ����ʾ���������
		U8* sabuf = (U8*)malloc(19);//��װ��һ��ͷ������
		if (sabuf == NULL) return;
		U8 control = 4;             // 0000,0100
		long int seq = 0;        // Ƥ����
		long int ack = 0;
		SA_DATA(sabuf, NULL, (U8)dst_IP, control, &seq, &ack, 0);
		free(sabuf);
		//return;                          // ����Ĺ�������������İ�

	}




	//Recv��*************************************************************************
	//���ֺ�OK_to_commu = 1����Ϊ���շ���commu_mod == 1
	if (OK_to_commu == 1 && (commu_mod == 1 || commu_mod == 6) && buf[1] == 4 && buf[0] == (U8)dst_IP)  //����Ľ�����
	{
		//printf("�յ��������������Ͽ����ӣ�������������������\n");
		U8* sabuf = (U8*)malloc(19);//��װ��һ��ͷ������
		if (sabuf == NULL) return;
		U8 control = 4;             // 0000,0100
		long int seq = 0;
		long int ack = 0;
		SA_DATA(sabuf, NULL, (U8)dst_IP, control, &seq, &ack, 0);
		free(sabuf);              //���շ���Ҳ�ᷢ�����ֶ�4������Ӧ
		RecvInfo.OK_to_fin = 1;
	}
	if (commu_mod == 1 && OK_to_commu == 1 && buf[1] == 2 && (*((long int*)(buf + 2)) >= RecvInfo.front_seq))
		//�����ֶ�Ϊ00000010 ����ʾ����DATA���Ͱ�����������Ҫ�յİ�Ҫ��front֮����������д�ļ�
	{
		long int pack_seq;
		U8  DATA_len;
		U8  base = 0;
		long int w_move;
		int i;
		int w_p = RecvInfo.front; // дָ����front������

		handshake_break = 0;
		// ���
		//printf("�����\n");
		pack_seq = *((long int*)(buf + 2));  // ����seq
		DATA_len = *((U8*)(buf + 18));   //���ݳ���
		U8* DATA = buf + 19;
		/*for (i = 0; i < DATA_len + 1; i++)
		{
			printf("%c ", DATA[i]);
		}*/

		//tail�ƶ�
		long int temp = RecvInfo.tail_seq;
		if (RecvInfo.tail_seq < pack_seq + (DATA_len - base))
		{
			//printf("\npack_seq��%d, (DATA_len - base)��%d��RecvInfo.tail_seq��%d\n", pack_seq, DATA_len - base, RecvInfo.tail_seq);
			for (i = 0; i < pack_seq + (DATA_len - base) - temp; i++)
			{
				//printf("tail�ƶ�%d��\n",i+1);
				if ((RecvInfo.tail + 1) % RecvInfo.total_win_size == RecvInfo.front)
				{
					//printf("tail���ܴ�Խfront\n");
					break;
				}
				RecvInfo.tail = (RecvInfo.tail + 1) % RecvInfo.total_win_size; // ѭ������
				RecvInfo.tail_seq++;
			}
		}
		//д���ڣ���Ҫд�������
		w_move = pack_seq - RecvInfo.front_seq;
		if (w_move < RecvInfo.total_win_size - 1)
		{
			w_p = (w_p + w_move) % RecvInfo.total_win_size; // ѭ������,w_p�ҵ�д��λ��
			//printf("д������\n");
			for (i = 0; i < (DATA_len - base); i++)
			{
				RecvWin[w_p] = DATA[i];
				//printf("��%dд����%d ", w_p, RecvWin[w_p]);
				w_p = (w_p + 1) % RecvInfo.total_win_size; // w_pǰ��1�ֽ�
				if (w_p == RecvInfo.tail) // дλ�ò��ܳ�Խtail
				{
					//printf("д��λ�ò��ܳ�Խtail\n");
					break;
				}
			}
			RecvInfo.write_time++; // д����������һ����ʵû��д��Ҳ���
		}
		//��������ǰʮ��
		/*for (i = 0; i < 10; i++)
		{
			printf("%c ", RecvWin[i]);
		}*/


		//err_tail�ƶ�
		//printf("\nerr_tail�ƶ���\n");
		for (i = 0; i < RecvInfo.total_win_size - 1; i++)
		{
			if (RecvInfo.err_tail == RecvInfo.tail)//���ᳬ��tail
			{
				//printf("err_tail���ᳬ��tail\n");
				break;
			}
			//��ǰΪ'?'������һ��λ�ò���'?'
			if (RecvWin[RecvInfo.err_tail] == (char)-1 && RecvWin[(RecvInfo.err_tail + 1) % RecvInfo.total_win_size] != (char)-1)
			{
				//printf("��ǰΪ'?'������һ��λ�ò���'?'\n");
				break;
			}
			RecvInfo.err_tail = (RecvInfo.err_tail + 1) % RecvInfo.total_win_size;
			//printf("err_tail�ƶ�%d��\n",i+1);
		}

		//front�ƶ���ͬʱд�ļ����ƶ���Ĵ���Ϊ'?'
		//printf("front�ƶ���\n");
		for (i = 0; i < RecvInfo.total_win_size - 1; i++)
		{
			//��һ��Ϊ'?'�Ͳ���ǰ����
			if (RecvWin[(RecvInfo.front + 1) % RecvInfo.total_win_size] == (char)-1)
			{
				//printf("frontǰ��һ��Ϊ'?'\n");
				break;
			}
			//��һ��ΪtailҲ����ǰ��
			if ((RecvInfo.front + 1) % RecvInfo.total_win_size == RecvInfo.tail)
			{
				//printf("frontǰ��һ��Ϊtail\n");
				break;
			}
			// ǰ��
			//printf("frontǰ��һ��\n");
			RecvInfo.front = (RecvInfo.front + 1) % RecvInfo.total_win_size;
			RecvInfo.front_seq += 1;
			//printf("����front��%d,д���ļ���%c ",RecvInfo.front, *(RecvWin + RecvInfo.front));
			fwrite(RecvWin + RecvInfo.front, sizeof(U8), 1, fp); //д��һ���ֽ�
			RecvWin[RecvInfo.front] = (char)-1;  // ���ڳ�ʼ��
		}
		//����ackǰ���ó�ackҪ����ȥ����Ϣ
		//printf("ack���ĳ�ʼ����\n");
		long int seq = RecvInfo.front - RecvInfo.tail - 1;
		if (seq < 0)
			seq = RecvInfo.total_win_size + seq;
		seq = seq + 1;
		long int ack = RecvInfo.front_seq + 1;
		int len = RecvInfo.err_tail - RecvInfo.front;
		if (len < 0)
			len = len + RecvInfo.total_win_size;
		if (len > 100)
			len = 100;

		//����ack
		if (RecvInfo.write_time % 2 == 1) // �����ͻᷢ
		{
			printf("ack���ķ���,ack=%ld,seq(ʣ�ര�ڴ�С)=%ld\n", ack, seq);
			RecvInfo.ack_Timeout = 0;
			U8* sabuf = (U8*)malloc(19);
			if (sabuf == NULL)
				return;
			SA_DATA(sabuf, NULL, (U8)dst_IP, 1, &seq, &ack, (U8)len);
			free(sabuf);
		}







	}
	if (OK_to_commu == 1 && buf[1] == 8 && buf[0] == (U8)dst_IP)  // �յ�ǿ�ϰ���8��
	{
		recv_connection_break();
	}











	iRcvTotal += retval;
	iRcvTotalCount++;

	/*if (bufRecv != NULL) {
		free(bufRecv);
	}*/








	//��ӡ
	/*switch (iWorkMode % 10) {
	case 1:
		cout <<endl<< "���սӿ� " <<ifNo <<" ���ݣ�"<<endl;
		print_data_bit(buf, len, lowerMode[ifNo]);
		break;
	case 2:
		cout << endl << "���սӿ� " << ifNo << " ���ݣ�" << endl;
		print_data_byte(buf, len, lowerMode[ifNo]);
		break;
	case 0:
		break;
	}*/
}
//��ӡͳ����Ϣ
void print_statistics()
{
	if (printCount % 10 == 0) {
		switch (spin) {
		case 1:
			printf("\r-");
			break;
		case 2:
			printf("\r\\");
			break;
		case 3:
			printf("\r|");
			break;
		case 4:
			printf("\r/");
			spin = 0;
			break;
		}
		cout << "������ " << iSndTotal << " λ," << iSndTotalCount << " ��," << "���� " << iSndErrorCount << " �δ���;";
		cout << " ������ " << iRcvTotal << " λ," << iRcvTotalCount << " ��";
		spin++;
	}
}
void menu()
{
	int selection = 666;
	char s_selection[10];
	char dst[10];
	unsigned short port;
	int iSndRetval;
	char kbBuf[100];
	int len;
	U8* bufSend;
	InputBox(s_selection, 10, "���������1-�������ӶԷ�;2-�Ӽ������뷢��;3-���͸�����һ��ͼ;4-�Ͽ�����;0-�˳�;");
	sscanf(s_selection, "%d", &selection);
	if (selection == 2)
	{
		selection = 3;
	}
	else if (selection == 3)
	{
		selection = 7;
	}
	else if (selection == 4)
	{
		selection = 8;
	}
	//����|��ӡ��[���Ϳ��ƣ�0���ȴ��������룻1���Զ���][��ӡ���ƣ�0�������ڴ�ӡͳ����Ϣ��1����bit����ӡ���ݣ�2���ֽ�����ӡ����]
	/*cout << endl << endl << "�豸��:" << strDevID << ",    ���:" << strLayer << ",    ʵ���:" << strEntity;
	cout << endl << "1-�������ӶԷ�;" << endl << "2-ˢ��; " << endl << "3-�Ӽ������뷢��; ";
	cout << endl << "4-����ӡͳ����Ϣ; " << endl << "5-����������ӡ��������;" << endl << "6-���ֽ�����ӡ��������;"<<endl << "7-���͸�����һ��ͼ;";
	cout << endl << "0-ȡ��" << endl << "����������ѡ�����";
	cin >> selection;*/
	switch (selection) {
	case 0:

		break;
	case 1:
		InputBox(dst, 10, "�Է��豸����ʲô��");
		sscanf(dst, "%d", &dst_IP);
		//cout << endl << "�Է��豸����ʲô�� ";
		//cin >> dst_IP;
		iWorkMode = 10 + iWorkMode % 10;
		break;
	case 2:
		iWorkMode = iWorkMode % 10;
		break;
	case 3:
		if (OK_to_commu == 1)
		{
			//cout << "�����ַ���(������100�ַ�)��";
			//cin >> kbBuf;
			FILE* f_menu = fopen("D://wangke//COMPUTER_NETWORKS//project1_code//send.txt", "wb+");
			InputBox(kbBuf, 100, "���������");
			int kbBuf_len = strlen(kbBuf);
			fwrite(kbBuf, kbBuf_len, 1, f_menu);
			fclose(f_menu);
			DATA_type = 1;
			iWorkMode = 3;
		}


		break;
	case 4:
		iWorkMode = (iWorkMode / 10) * 10 + 0;
		break;
	case 5:
		iWorkMode = (iWorkMode / 10) * 10 + 1;
		break;
	case 6:
		iWorkMode = (iWorkMode / 10) * 10 + 2;
		break;
	case 7:          // ��Ҫ����һ��ͼ
		if (OK_to_commu == 1)
		{
			iWorkMode = 7;
			DATA_type = 0;
		}
		break;
	case 8:
		connection_break();
		break;
	default:
		break;
	}

}

int SA_DATA(U8* sabuf, U8* DATA, U8 dst, U8 control, long int* seq, long int* ack, U8 len) // Ŀ�ģ���sabuf�����
{
	if (sabuf == NULL)
		return -1;
	sabuf[0] = dst;

	sabuf[1] = control;

	sabuf[2] = *((char*)seq);
	sabuf[3] = *((char*)seq + 1);
	sabuf[4] = *((char*)seq + 2);
	sabuf[5] = *((char*)seq + 3);
	sabuf[6] = *((char*)seq + 4);
	sabuf[7] = *((char*)seq + 5);
	sabuf[8] = *((char*)seq + 6);
	sabuf[9] = *((char*)seq + 7);

	sabuf[10] = *((char*)ack);
	sabuf[11] = *((char*)ack + 1);
	sabuf[12] = *((char*)ack + 2);
	sabuf[13] = *((char*)ack + 3);
	sabuf[14] = *((char*)ack + 4);
	sabuf[15] = *((char*)ack + 5);
	sabuf[16] = *((char*)ack + 6);
	sabuf[17] = *((char*)ack + 7);

	sabuf[18] = len;             // ͷ��19�ֽ� 

	char base = 0;
	int int_len = len - base;
	for (int i = 0; i < int_len; i++)
	{
		if (DATA == NULL)
			break;
		sabuf[19 + i] = DATA[i]; // DATA��һ�����ݵĻ��棬���Դ�0��ʼ���ɣ�����seq
	}
	int ret;
	if (DATA != NULL)
	{
		ret = SendtoLower(sabuf, int_len + 19, 0);
	}
	else
		ret = SendtoLower(sabuf, 19, 0); // û�����ݣ��ͷ�����ͷ
	return ret;
}













// ����base64 �� ͼƬ��ת����****************************************************
//�����ڷ��ͷ������һ��send.txt�����㴫��
void code_image(void)  // ��ͼƬ�����Base64��ʽ,������Ҫ���������һ��input.txt�ļ���������void��
{
	FILE* fpointer = NULL;
	FILE* ftxt;
	unsigned int imageSize;        //ͼƬ�ֽ���
	char* imageBin;
	char* imageBase64;

	size_t result;
	//char* ret;
	//unsigned int base64StrLength;
	errno_t err;

	ftxt = fopen("D:\\wangke\\COMPUTER_NETWORKS\\project1_code\\send.txt", "w");
	err = fopen_s(&fpointer, "D:\\wangke\\COMPUTER_NETWORKS\\project1_code\\input.jpg", "rb");   //������ͼƬ
	if (NULL == fpointer)
	{
		printf("failed to open a file\n");
		return;
	}
	//��ȡͼƬ��С
	fseek(fpointer, 0L, SEEK_END);
	imageSize = ftell(fpointer);
	fseek(fpointer, 0L, SEEK_SET);

	//�����ڴ�洢����ͼƬ
	imageBin = (char*)malloc(sizeof(char) * imageSize);
	if (NULL == imageBin)
	{
		printf("malloc failed");
		return;
	}

	//��ȡͼƬ
	result = fread(imageBin, 1, imageSize, fpointer);
	if (result != imageSize)
	{
		printf("file reading failed");
		return;
	}

	//��������ͼƬ����buffer
	imageBase64 = (char*)malloc(sizeof(char) * imageSize * 2);//��Ϊ����һ����Դ���ݴ�1/3�����ӣ�����ֱ������Դ�ļ�һ���Ŀռ�
	if (NULL == imageBase64)
	{
		printf("malloc failed");
		return;
	}

	//base64����
	base64_encode((const unsigned char*)imageBin, imageBase64, imageSize);
	fwrite(imageBase64, strlen(imageBase64), 1, ftxt);
	fclose(ftxt);
	fclose(fpointer);
	//base64StrLength = strlen(imageBase64);
	//printf("base64 str length:%d\n", base64StrLength);
	//printf("\ncode_pic->\n%s\n", imageBase64);
	//return imageBase64;
}

//��������ȡ�����յ���txt�ļ�Ϊoutput.txt
void decode_image(void) // ��Base64�����ͼƬ�������Զ���д����·����output.txt�ļ�תΪͼƬoutput.jpg��
{
	FILE* ftxt = fopen("D:\\wangke\\COMPUTER_NETWORKS\\project1_code\\recv.txt", "r"); // ֻ��
	long int txt_size;        // base��ĳ��� 
	fseek(ftxt, 0, SEEK_END);
	txt_size = ftell(ftxt);
	fseek(ftxt, 0, SEEK_SET);
	FILE* fpointer = NULL;
	errno_t err_1;
	err_1 = fopen_s(&fpointer, "D:\\wangke\\COMPUTER_NETWORKS\\project1_code\\output.jpg", "wb");
	if (fpointer == NULL)
	{
		printf("faile to open a file\n");
		return;
	}
	char* imageBase64 = (char*)malloc(sizeof(char) * txt_size);
	if (imageBase64 == NULL)
		return;
	fread(imageBase64, txt_size, 1, ftxt);



	//ͼƬ�ֽ���
	//char* imageBin;
	char* imageOutput;
	//����洢��������buffer
	imageOutput = (char*)malloc(sizeof(char) * txt_size);//�����Ӧ�ú�ԴͼƬ��Сһ��
	if (imageOutput == NULL) return;
	if (NULL == imageBase64)
	{
		printf("malloc failed");
		//return -1;
	}
	base64_decode(imageBase64, (unsigned char*)imageOutput);

	fwrite(imageOutput, 1, txt_size, fpointer);
	fclose(fpointer);
	fclose(ftxt);

	//free(imageBin);
	free(imageBase64);
	free(imageOutput);


}

char* base64_encode(const unsigned char* bindata, char* base64, int binlength)
{
	int i, j;
	unsigned char current;

	for (i = 0, j = 0; i < binlength; i += 3)
	{
		current = (bindata[i] >> 2);
		current &= (unsigned char)0x3F;
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(bindata[i] << 4)) & ((unsigned char)0x30);
		if (i + 1 >= binlength)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 1] >> 4)) & ((unsigned char)0x0F);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(bindata[i + 1] << 2)) & ((unsigned char)0x3C);
		if (i + 2 >= binlength)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 2] >> 6)) & ((unsigned char)0x03);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)bindata[i + 2]) & ((unsigned char)0x3F);
		base64[j++] = base64char[(int)current];
	}
	base64[j] = '\0';
	return base64;
}

int base64_decode(const char* base64, unsigned char* bindata)
{
	int i, j;
	unsigned char k;
	unsigned char temp[4];
	for (i = 0, j = 0; base64[i] != '\0'; i += 4)
	{
		memset(temp, 0xFF, sizeof(temp));
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i])
				temp[0] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 1])
				temp[1] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 2])
				temp[2] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 3])
				temp[3] = k;
		}

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2)) & 0xFC)) |
			((unsigned char)((unsigned char)(temp[1] >> 4) & 0x03));
		if (base64[i + 2] == '=')
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4)) & 0xF0)) |
			((unsigned char)((unsigned char)(temp[2] >> 2) & 0x0F));
		if (base64[i + 3] == '=')
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6)) & 0xF0)) |
			((unsigned char)(temp[3] & 0x3F));
	}
	return j;
}




// ���ڿ��ӻ��ĺ�������
//��ť�жϺ���
int button_judge(int x, int y)
{
	if (x > r[0][0] && x<r[0][2] && y>r[0][1] && y < r[0][3])return 1;
	if (x > r[1][0] && x<r[1][2] && y>r[1][1] && y < r[1][3])return 2;
	if (x > r[2][0] && x<r[2][2] && y>r[2][1] && y < r[2][3])return 3;
	if (x > r[3][0] && x<r[3][2] && y>r[3][1] && y < r[3][3])return 4;
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
	const char DevID[2] = { strDevID[0] ,'\0' };
	//DevID[0] = strDevID[0] - 48;
	//DevID[1] = '\0';

	short win_width, win_height;//���崰�ڵĿ�Ⱥ͸߶�
	win_width = 480; win_height = 200;
	initgraph(win_width, win_height);//��ʼ�����ڣ�������

	setbkcolor(RGB(250, 250, 250));
	cleardevice();//������ȡ���ڱ���ɫ��

	RECT R1 = { r[0][0],r[0][1],r[0][2],r[0][3] };
	RECT R2 = { r[1][0],r[1][1],r[1][2],r[1][3] };
	RECT R3 = { r[2][0],r[2][1],r[2][2],r[2][3] };
	RECT R4 = { r[3][0],r[3][1],r[3][2],r[3][3] };
	RECT R5 = { r[4][0],r[4][1],r[4][2],r[4][3] };

	LOGFONT f;//������ʽָ��
	gettextstyle(&f);					//��ȡ������ʽ
	_tcscpy(f.lfFaceName, _T("����"));	//��������Ϊ����
	f.lfQuality = ANTIALIASED_QUALITY;    // �������Ч��Ϊ�����  
	settextstyle(&f);                     // ����������ʽ
	settextcolor(BLACK);				//BLACK��graphic.hͷ�ļ����汻����Ϊ��ɫ����ɫ����
	drawtext("������", &R1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�ھ�������R1���������֣�ˮƽ���У���ֱ���У�������ʾ
	drawtext("������", &R2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�ھ�������R2���������֣�ˮƽ���У���ֱ���У�������ʾ
	drawtext("��ͼƬ", &R3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�ھ�������R3���������֣�ˮƽ���У���ֱ���У�������ʾ
	drawtext("������", &R4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�ھ�������R3���������֣�ˮƽ���У���ֱ���У�������ʾ
	drawtext(DevID, &R5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//�ھ�������R3���������֣�ˮƽ���У���ֱ���У�������ʾ
	setlinecolor(BLACK);
	rectangle(r[0][0], r[0][1], r[0][2], r[0][3]);
	rectangle(r[1][0], r[1][1], r[1][2], r[1][3]);
	rectangle(r[2][0], r[2][1], r[2][2], r[2][3]);
	rectangle(r[3][0], r[3][1], r[3][2], r[3][3]);
	rectangle(r[4][0], r[4][1], r[4][2], r[4][3]);

}

int win_case_judge(void)
{

	m = GetMouseMsg();


	switch (m.uMsg)
	{
	case WM_LBUTTONDOWN:
		switch (button_judge(m.x, m.y))
		{
			//��ԭ��ťԭ��
		case 1:
			char dst[10];
			//int dst;
			InputBox(dst, 10, "���������Ӷ���");
			sscanf(dst, "%d", &dst_IP);//�������ַ�������ɨ�赽ȫ�ֱ�������
			iWorkMode = 10;
			FlushMouseMsgBuffer();//�����¼�����������Ϣ
			break;
		case 2:
			if (OK_to_commu == 1)
			{
				char kbBuf[100];
				FILE* f_menu = fopen("D://wangke//COMPUTER_NETWORKS//project1_code//send.txt", "wb+");
				InputBox(kbBuf, 100, "���������");
				int kbBuf_len = strlen(kbBuf);
				fwrite(kbBuf, kbBuf_len, 1, f_menu);
				fclose(f_menu);
				DATA_type = 1;
				iWorkMode = 3;
			}
			FlushMouseMsgBuffer();//�����¼�����������Ϣ
			break;
		case 3:
			if (OK_to_commu == 1)
			{
				MessageBox(NULL, "��������ͼƬ", "��Ϣ", MB_OK | MB_ICONINFORMATION);
				iWorkMode = 7;
				DATA_type = 0;
			}
			FlushMouseMsgBuffer();//�����¼�����������Ϣ
			break;
		case 4:
			connection_break();
			FlushMouseMsgBuffer();//�����¼�����������Ϣ
			break;
		default:
			FlushMouseMsgBuffer();//�����¼�����������Ϣ
			//printf("\r\n(%d,%d)",m.x,m.y);//��ӡ������꣬�������ʱȷ������
			break;
		}
		return 0;
		break;
	default:
		FlushMouseMsgBuffer();//�����¼�����������Ϣ
		break;
	}



	return 0;
}


void txt_in(char* s)
{
	InputBox(s, 100, "���������");
}

void connection_break(void)  // ǿ�ж���
{
	if (OK_to_commu == 1)
	{
		U8 break_pack[2];
		break_pack[0] = (U8)dst_IP;
		break_pack[1] = 8;
		SendtoLower(break_pack, 2, 0);
		SendtoLower(break_pack, 2, 0);
		SendtoLower(break_pack, 2, 0);
		OK_to_commu = 0;
		dst_IP = 0;
		commu_mod = 7;
		iWorkMode = 0;
		handshake_case = 0;
		RecvInfo.Info_init = 7;
		SendInfo.Info_init = 7;
		SendInfo.file_open = 0;
		RecvInfo.file_open = 0;
		DATA_type = 7;
		MessageBox(NULL, "�ѶϿ�����", "��Ϣ", MB_OK | MB_ICONINFORMATION);
	}
}

void recv_connection_break(void)
{
	OK_to_commu = 0;
	dst_IP = 0;
	commu_mod = 7;
	iWorkMode = 0;
	handshake_case = 0;
	RecvInfo.Info_init = 7;
	SendInfo.Info_init = 7;
	SendInfo.file_open = 0;
	RecvInfo.file_open = 0;
	DATA_type = 7;
	MessageBox(NULL, "�Է��ѶϿ�����", "��Ϣ", MB_OK | MB_ICONINFORMATION);
}