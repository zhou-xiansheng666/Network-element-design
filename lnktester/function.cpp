//Nettester �Ĺ����ļ�
#include <iostream>
#include <conio.h>
#include <cstring>
#include<cmath>
#include<algorithm>
#include "winsock.h"
#include "stdio.h"
#include "CfgFileParms.h"
#include "function.h"
using namespace std;

//����Ϊ��Ҫ�ı���
U8* sendbuf;        //������֯�������ݵĻ��棬��СΪMAX_BUFFER_SIZE,���������������������ƣ��γ��ʺϵĽṹ��������û��ʹ�ã�ֻ������һ��
int printCount = 0; //��ӡ����
int spin = 0;  //��ӡ��̬��Ϣ����

//------�����ķָ��ߣ�һЩͳ���õ�ȫ�ֱ���------------
int iSndTotal = 0;  //������������
int iSndTotalCount = 0; //���������ܴ���
int iSndErrorCount = 0;  //���ʹ������
int iRcvForward = 0;     //ת����������
int iRcvForwardCount = 0; //ת�������ܴ���
int iRcvToUpper = 0;      //�ӵͲ�ݽ��߲���������
int iRcvToUpperCount = 0;  //�ӵͲ�ݽ��߲������ܴ���
int iRcvUnknownCount = 0;  //�յ�������Դ�����ܴ���

struct list {
	int to, next, ifNo;
}tree_list[10];
int tree_top,K=0;
//��ӡͳ����Ϣ
void print_statistics();
void menu();
//***************��Ҫ��������******************************
//���ƣ�InitFunction
//���ܣ���ʼ�������棬��main�����ڶ��������ļ�����ʽ������������ǰ����
//���룺
//�����
void InitFunction(CCfgFileParms& cfgParms)
{
	sendbuf = (char*)malloc(MAX_BUFFER_SIZE);
	if (sendbuf == NULL ) {
		cout << "�ڴ治��" << endl;
		//����������Ҳ̫���˳���
		exit(0);
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
//	if(sendbuf != NULL)
//		free(sendbuf);
	return;
}

//***************��Ҫ��������******************************
//���ƣ�TimeOut
//���ܣ�������������ʱ����ζ��sBasicTimer�����õĳ�ʱʱ�䵽�ˣ�
//      �������ݿ���ȫ���滻Ϊ������Լ����뷨
//      ������ʵ���˼���ͬʱ���й��ܣ����ο�
//      1)����iWorkMode����ģʽ���ж��Ƿ񽫼�����������ݷ��ͣ�
//        ��Ϊscanf�����������¼�ʱ���ڵȴ����̵�ʱ����ȫʧЧ������ʹ��_kbhit()������������ϵ��ڼ�ʱ�Ŀ������жϼ���״̬�������Get��û��
//      2)����ˢ�´�ӡ����ͳ��ֵ��ͨ����ӡ���Ʒ��Ŀ��ƣ�����ʼ�ձ�����ͬһ�д�ӡ��Get��
//���룺ʱ�䵽�˾ʹ�����ֻ��ͨ��ȫ�ֱ�����������
//���������Ǹ�����Ŭ���ɻ����ʵ����
int bag_size = 10;
int IP2MAC[1000];
int treetable[1000],is_send_ok[1000],is_tree_cre=0;
int tree_fa,tree_de=-1;
int add_bag(U8* buf, int len, int seq, U8* send_buf) {
	
	send_buf[0] = 126;
	int x = strEntity[0] - '0';
	send_buf[1] = buf[0] * 10 + x;
	if (IP2MAC[buf[1]] == 0) {
		send_buf[2] = buf[1] * 10 + 9;
	}
	else send_buf[2] = IP2MAC[buf[1]];
	send_buf[3] = 3;//����
	send_buf[4] = seq;//���
	send_buf[5] = buf[3];//ʣ�µ�֡��
	send_buf[6] = len;//ʣ�µ��ֽڳ���
	send_buf[7] = 0;
	send_buf[8] = 0;
	send_buf[len + bag_size - 1] = 126;//��β
	for (int i = 0; i < len; i++) {
		send_buf[bag_size-1 + i] = buf[i];
	}
	return len + bag_size;
}
int add_bag_chen(U8* buf, int len, U8* send_buf) {
	send_buf[0] = 126;
	int x = strEntity[0] - '0';
	send_buf[1] = buf[0] * 10 + x;
	if (IP2MAC[buf[1]] == 0) {
		send_buf[2] = buf[1] * 10 + 9;
	}
	else send_buf[2] = IP2MAC[buf[1]];
	send_buf[3] = 3;//����
	send_buf[4] = 0;//���
	send_buf[5] = 0;//ʣ�µ�֡��
	send_buf[6] = len-1;//ʣ�µ��ֽڳ���
	send_buf[7] = 0;
	send_buf[8] = 0;

	//ɾ���أ�
	send_buf[len + bag_size - 2] = 126;//��β
	send_buf[9] = buf[0];
	for (int i = 2; i < len; i++) {
		send_buf[bag_size - 2 + i] = buf[i];
	}
	return len + bag_size-1;
	//��ɾ���أ�
	/*send_buf[len + bag_size - 1] = 126;//��β
	for (int i = 0; i < len; i++) {
		send_buf[bag_size - 1 + i] = buf[i];
	}
	return len + bag_size;*/
}
int add_bag_tongbu(U8* buf, int len, U8* send_buf) {
	send_buf[0] = 126;
	send_buf[len + 1] = 126;
	for (int i = 0; i < len; i++) {
		send_buf[i + 1] = buf[i];
	}
	return len + 2;
}

int add_bag_tree(U8* buf,int cost) {
	int x = strDevID[0] - '0';
	int y = strEntity[0] - '0';
	int my_dr = x * 10 + y;
	buf[0] = 126;
	buf[1] = my_dr;
	buf[2] = 99;
	buf[3] = 2;//����Ϊ2����������֡
	buf[4] = cost;//����
	buf[5] = 0;//ʣ�µ�֡��
	buf[6] = 0;//ʣ����ֽ���
	buf[7] = 0;
	buf[8] = 0;//CRCУ��
	buf[bag_size - 1] = 126;
	return bag_size;
}
void add_bag_tree_ack(U8* buf,int ds_dr) {
	int x = strDevID[0] - '0';
	int y = strEntity[0] - '0';
	int my_dr = x * 10 + y;
	buf[0] = 126;
	buf[1] = my_dr;
	buf[2] = ds_dr;
	buf[3] = 1;//����ȷ��֡
	buf[4] = 0;
	buf[5] = 0;//ʣ��֡��
	buf[6] = 0;//ʣ���ֽ�
	buf[7] = 0;
	buf[8] = 0;//CRC
	buf[bag_size - 1] = 126;
}
void add_bag_tree_ack_pc(U8* buf, int ds_dr) {
	int x = strDevID[0] - '0';
	int y = strEntity[0] - '0';
	int my_dr = x * 10 + y;
	buf[0] = 126;
	buf[1] = my_dr;
	buf[2] = ds_dr;
	buf[3] = 1;//����ȷ��֡
	buf[4] = 1;//�Ƿ���pc����ȷ��֡
	buf[5] = 0;//ʣ��֡��
	buf[6] = 0;//ʣ���ֽ�
	buf[7] = 0;
	buf[8] = 0;//CRC
	buf[bag_size - 1] = 126;
}
int add_bag_ack(U8* buf,int dst, int ack) {
	buf[0] = 126;
	int x = strDevID[0] - '0';
	int y = strEntity[0] - '0';
	int my_dr = x * 10 + y;
	buf[1] = my_dr;
	buf[2] = dst;
	buf[3] = 4;//ackȷ��
	buf[4] = ack;
	buf[5] = 0;
	buf[6] = 0;
	buf[7] = 0;
	buf[8] = 0;
	buf[9] = 126;
	return bag_size;
}
void add_bag_arp(U8* buf, int src, int dst) {
	buf[0] = 126;
	buf[1] = src;
	buf[2] = dst;
	buf[3] = 0;//arp����
	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;
	buf[7] = 0;
	buf[8] = 0;//CRC
	buf[bag_size - 1] = 126;
}
void add_bag_arp_ack(U8* buf, int src, int dst,int net_dst) {
	buf[0] = 126;
	buf[1] = src;
	buf[2] = dst;
	buf[3] = 0;//arp����
	buf[4] = 1;//ackȷ��
	buf[5] = net_dst;
	buf[6] = 0;
	buf[7] = 0;
	buf[8] = 0;//CRC
	buf[bag_size - 1] = 126;
}
void win_send(U8* buf, int len,int ifNo) {
	int iSndRetval;
	if (lowerMode[ifNo]== 0) {
		U8* bufSend;
		bufSend = (char*)malloc(len * 8);
		if (bufSend == NULL) {
			return;
		}
		iSndRetval = ByteArrayToBitArray(bufSend, len * 8, buf, len);
		//����
		iSndRetval = SendtoLower(bufSend, iSndRetval, ifNo); //��������Ϊ���ݻ��壬���ȣ��ӿں�
	}
	else {
		//�²����ֽ�����ӿڣ���ֱ�ӷ���
		iSndRetval = SendtoLower(buf, len, 0);
		iSndRetval = iSndRetval * 8;//�����λ
	}
}
void add_bag_net(U8* buf, int len, U8* send_buf) {
	send_buf[0] = 126;

	send_buf[1] = buf[0];
	send_buf[2] = buf[1];
	send_buf[3] = 5;//����
	send_buf[4] = 0;//���
	send_buf[5] = 0;//ʣ�µ�֡��
	send_buf[6] = len;//ʣ�µ��ֽڳ���
	send_buf[7] = 0;
	send_buf[8] = 0;
	send_buf[len + bag_size - 1] = 126;//��β
	for (int i = 0; i < len; i++) {
		send_buf[bag_size-1 + i] = buf[i];
	}
}

int window_len = 4;
int clk[4], ack[4], win_len[4] = {0,0,0,0};
U8* win_data[4];

int da_fr_up_sz = 450, da_fr_up_fg = 0, da_fr_up_ff = 0, seq_size = 7, seq = 0;
int da_fr_up_len[500];
U8* da_fr_up[500];

void TimeOut()
{
	int time_out = 100;
	printCount++;/*ԭ���벿��
	for (int i = 0; i < window_len; i++) {
		if (win_len[i] == 0) {
			if (da_fr_up_len[da_fr_up_fg] != 0) {//д������������²��ֱ��ж��ǽ������Ļ������ڷ��ͻ���pc���Ĵ��ڣ����ݴ���һ����������ת�����У�����Ҫ�Ӱ�ͷ������ֻ����������IPmac��ת��
				if (lowerNumber == 1) {
					if (treetable[IP2MAC[da_fr_up[da_fr_up_fg][1]]]) {
						win_data[i] = (U8*)malloc(sizeof(U8*) * (da_fr_up_len[da_fr_up_fg] + bag_size));
						win_len[i] = add_bag(da_fr_up[da_fr_up_fg], da_fr_up_len[da_fr_up_fg], seq++, win_data[i]);
						seq %= seq_size;
						win_send(win_data[i], win_len[i], treetable[IP2MAC[da_fr_up[da_fr_up_fg][1]]]-1);

						free(da_fr_up[da_fr_up_fg]);
						da_fr_up_len[da_fr_up_fg] = 0;
						da_fr_up_fg = (da_fr_up_fg + 1) % da_fr_up_sz;
			//			printf("���ڷ������ݣ�");
			//			print_data_byte(win_data[i], win_len[i], 1);
					}
					else {
						U8* buf_arp;
						int buf_arp_len = bag_size;
						buf_arp = (U8*)malloc(sizeof(U8) * buf_arp_len);
						int x = strDevID[0] - '0';
						int y = strEntity[0] - '0';
						int my_dr = x * 10 + y;
						add_bag_arp(buf_arp, my_dr, da_fr_up[da_fr_up_fg][1] * 10 + 9);
						if (lowerNumber == 1) {//�����Ĵ���
							if (lowerMode[0] == 0) {
								U8* buf_arp_send;
								int buf_arp_send_len = buf_arp_len * 8;
								buf_arp_send = (U8*)malloc(sizeof(U8) * buf_arp_send_len);
								ByteArrayToBitArray(buf_arp_send, buf_arp_send_len, buf_arp, buf_arp_len);
								printf("�����������ˣ�");
								SendtoLower(buf_arp_send, buf_arp_send_len, 0);
							}
							else {
								SendtoLower(buf_arp, buf_arp_len, 0);
							}
						}
					}
					clk[i] = printCount;
				}/*
				else if (lowerNumber > 1) {
					printf("��������ʼ�������ݰ�");
					printf("%d#%d!%d\n", treetable[da_fr_up[da_fr_up_fg][1]], da_fr_up[da_fr_up_fg][0], da_fr_up[da_fr_up_fg][1]);
					if (treetable[da_fr_up[da_fr_up_fg][1]]) {

						win_data[i] = (U8*)malloc(sizeof(U8*) * (da_fr_up_len[da_fr_up_fg] + 2));
						win_len[i] = add_bag_tongbu(da_fr_up[da_fr_up_fg], da_fr_up_len[da_fr_up_fg], win_data[i]);

						win_send(win_data[i], win_len[i], treetable[da_fr_up[da_fr_up_fg][1]]-1);
			//			seq %= seq_size;
						free(da_fr_up[da_fr_up_fg]);
						da_fr_up_len[da_fr_up_fg] = 0;
						da_fr_up_fg = (da_fr_up_fg + 1) % da_fr_up_sz;
			//			printf("���ڷ������ݣ�");
			//			print_data_byte(win_data[i], win_len[i], 1);
					}
					clk[i] = printCount;
				}
				
			}
		}
		if (printCount - clk[i] > time_out) {
			if (win_len[i] != 0) {
		//		printf("��ʱ�ش���\n");
		//		print_data_byte(win_data[i], win_len[i], 1);
				win_send(win_data[i], win_len[i],treetable[win_data[i][2]]-1);
				clk[i] = printCount;
			}
		}
	}
	*///��ʱ�ش�����
	if (_kbhit()) {
		//�����ж���������˵�ģʽ
		menu();
	}

	print_statistics();
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
	printf("\n�յ��ϲ�����\n");
//	print_data_byte(buf, len, 1);
//	printf("!!%d %d\n", buf[1],(unsigned char)buf[1]);
//	while (1)printf("2 ");
	if (((unsigned char)buf[1] == 240 || (unsigned char)buf[1] == 241)) {
		U8* buf_send;
		int buf_send_len = len + bag_size;
		buf_send = (U8*)malloc(sizeof(U8) * buf_send_len);
		printf("\n�յ��ϲ�%d��������ת��,�˿�Ϊ��0",(unsigned char)buf[1]);
		add_bag_net(buf, len, buf_send);
	//	print_data_byte(buf_send, bag_size, 1);
		win_send(buf_send, buf_send_len,0);
	}
	else if ((unsigned char)buf[2] == 225) {
		U8* buf_send;
		int buf_send_len = bag_size;
		buf_send = (U8*)malloc(sizeof(U8) * buf_send_len);
		int x = strDevID[0] - '0';
		int y = strEntity[0] - '0';
		int my_dr = x * 10 + y;
		printf("\n�յ��ϲ�arpȷ�ϣ�����ת��,�˿�Ϊ��0");
		add_bag_arp_ack(buf_send, my_dr, buf[1],x);
	//	print_data_byte(buf_send, bag_size, 1);
		win_send(buf_send, buf_send_len, 0);
	}
	else if (treetable[IP2MAC[buf[1]]]) {
		
		U8* buf_data;
		int buf_data_len = len + bag_size;//ɾ���ؾ�-1
		buf_data = (U8*)malloc(sizeof(U8) * buf_data_len);
		buf_data_len=add_bag_chen(buf, len, buf_data);
		printf("\nת���ϲ������,�˿�Ϊ��0");
	//	print_data_byte(buf_data, bag_size,1);
		win_send(buf_data, buf_data_len, treetable[IP2MAC[buf[1]]] - 1);
		//			printf("���ڷ������ݣ�");
		//			print_data_byte(win_data[i], win_len[i], 1);

	}
	else {
		U8* buf_arp;
		int buf_arp_len = bag_size;
		buf_arp = (U8*)malloc(sizeof(U8) * buf_arp_len);
		int x = strDevID[0] - '0';
		int y = strEntity[0] - '0';
		int my_dr = x * 10 + y;
		add_bag_arp(buf_arp, my_dr, buf[1] * 10 + 9);
		if (lowerNumber == 1) {//�����Ĵ���
			if (lowerMode[0] == 0) {
				U8* buf_arp_send;
				int buf_arp_send_len = buf_arp_len * 8;
				buf_arp_send = (U8*)malloc(sizeof(U8) * buf_arp_send_len);
				ByteArrayToBitArray(buf_arp_send, buf_arp_send_len, buf_arp, buf_arp_len);
				printf("\n����arp��,�˿�Ϊ��0");
		//		print_data_byte(buf_arp, bag_size,1);
				SendtoLower(buf_arp_send, buf_arp_send_len, 0);
			}
			else {
				SendtoLower(buf_arp, buf_arp_len, 0);
			}
		}
	}

	/*ԭ���벿��
	da_fr_up[da_fr_up_ff] = (U8*)malloc(len * sizeof(U8));//��������Ӧ����da_fr_up_ff�ȡ��ʱ���fg��ȡ
	for (int i = 0; i < len; i++) {
		da_fr_up[da_fr_up_ff][i] = buf[i];
	}
	da_fr_up_len[da_fr_up_ff] = len;
	printf("���ϲ��յ����ݣ�");
	print_data_byte(da_fr_up[da_fr_up_ff], da_fr_up_len[da_fr_up_ff], 1);
	da_fr_up_ff = (da_fr_up_ff + 1) % da_fr_up_sz;*/


//	printf("%d\n")
	/*
	if (!IP2MAC[buf[1]]) {
		U8* buf_arp;
		int buf_arp_len=bag_size;
		buf_arp = (U8*)malloc(sizeof(U8) * buf_arp_len);
		int x = strDevID[0] - '0';
		int y = strEntity[0] - '0';
		int my_dr = x * 10 + y;
		add_bag_arp(buf_arp, my_dr, buf[1] * 10 + 9);
		print_data_byte(buf_arp, buf_arp_len, 1);
		if (lowerNumber == 1) {//�����Ĵ���
			if (lowerMode[0] == 0) {
				U8* buf_arp_send;
				int buf_arp_send_len = buf_arp_len * 8;
				printf("������arp��Ϊ��");
				print_data_byte(buf_arp, buf_arp_len, 1);
				buf_arp_send = (U8*)malloc(sizeof(U8) * buf_arp_send_len);
				ByteArrayToBitArray(buf_arp_send, buf_arp_send_len, buf_arp, buf_arp_len);
				SendtoLower(buf_arp_send, buf_arp_send_len, 0);
			}
			else {
				SendtoLower(buf_arp, buf_arp_len, 0);
			}
		}
		else {
			//·������ӵĴ���
		}
	}*/

/*
	if (!is_tree_cre) {
		U8* cre_buf;
		cre_buf = (U8*)malloc(sizeof(U8) * bag_size);
		add_bag_tree(cre_buf, 0);
		tree_fa = my_dr;
		lowerNumber
	}
	if (!IP2MAC[buf[1]]) {
		if (is_tree_cre) {

		}
	}
*/
	/*
	int iSndRetval;
	U8* bufSend = NULL;
	//�Ǹ߲����ݣ�ֻ�ӽӿ�0����ȥ,�߲�ӿ�Ĭ�϶����ֽ������ݸ�ʽ
	if (lowerMode[0] == 0) {
		//�ӿ�0��ģʽΪbit���飬��ת����bit���飬�ŵ�bufSend��
		bufSend = (char*)malloc(len * 8);
		if (bufSend == NULL) {
			return;
		}
		iSndRetval = ByteArrayToBitArray(bufSend, len * 8, buf, len);
		//����
		iSndRetval = SendtoLower(bufSend, iSndRetval, 0); //��������Ϊ���ݻ��壬���ȣ��ӿں�
	}
	else {
		//�²����ֽ�����ӿڣ���ֱ�ӷ���
		iSndRetval = SendtoLower(buf, len, 0);
		iSndRetval = iSndRetval * 8;//�����λ
	}
	//����������ͣ��Э����ش�Э�飬���������Ҫ����������Ӧ����������ռ䣬��buf��bufSend�����ݱ����������Ա��ش�
	if (bufSend != NULL) {
		//����bufSend���ݣ�CODES NEED HERE

		//������û������ش�Э�飬����Ҫ�������ݣ����Խ��ռ��ͷ�
		free(bufSend);
	}
	else {
		//����buf���ݣ�CODES NEED HERE

		//������û������ش�Э�飬����Ҫ�������ݣ�buf�����������Ҳ����Ҫ�ͷſռ�

	}

	//ͳ��
	if (iSndRetval <= 0) {
		iSndErrorCount++;
	}
	else {
		iSndTotal += iSndRetval;
		iSndTotalCount++;
	}
	//printf("\n�յ��ϲ����� %d λ�����͵��ӿ�0\n", retval * 8);
	//��ӡ
	switch (iWorkMode % 10) {
	case 1:
		cout << endl << "�߲�Ҫ����ӿ� " << 0 << " �������ݣ�" << endl;
		print_data_bit(buf, len, 1);
		break;
	case 2:
		cout << endl << "�߲�Ҫ����ӿ� " << 0 << " �������ݣ�" << endl;
		print_data_byte(buf, len, 1);
		break;
	case 0:
		break;
	}
*/
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
void adde(int to, int ifNo) {
	tree_list[++K].next = tree_top;
	tree_list[K].to = to;
	tree_list[K].ifNo = ifNo;
	tree_top = K;
	treetable[to] = ifNo+1;//ifno+1!!!!!
	is_tree_cre = 1;
	is_send_ok[ifNo] = 1;
	printf("\n�˿ڿ��Է��������");
	for (int i = 0; i < lowerNumber; i++) {
		printf("%d:%d\n", i, is_send_ok[i]);
	}
	printf("\n��ַת����\n");
	for (int i = 0; i <= 99; i++) {
		if (treetable[i]) {
			printf("%02x��%d ", (unsigned char)i,treetable[i]-1);
		}
	}
}/*
U8 crc[17] = { 1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1 };
int crc_len = 17;
void cala_crc(U8* buf, int len) {
	for (int i = 0; i < len; i++) {
		if (buf[i] != 0 && i + crc_len <= len) {
			for (int j = 0; j < crc_len; j++) {
				buf[i + j] ^= crc[j];
			}
		}
	}
}*/

int tongbu(U8* buf, int len) {
	U8 s[8] = { 0,1,1,1,1,1,1,0 };
	int tot = 0;

	int ff = 0;
	int fg = 0;
	for (int i = 0; i + 7 < len; i++) {
		int flag = 0;
		for (int j = 0; j <= 7; j++) {
			if (buf[i + j] != s[j]) {
				flag = 1;
				break;
			}
		}
		if (flag == 0) {
			i = i + 8;
			fg = 1;
			U8* bag_bit;
			U8* bag_byte;
			int bag_bit_len = (bag_size - 2) * 8,bag_byte_len= bag_size - 2;
			bag_bit = (U8*)malloc(sizeof(U8) * bag_bit_len);
			bag_byte = (U8*)malloc(sizeof(U8) * bag_byte_len);
			for (int j = 0; j < bag_bit_len; j++) {
				bag_bit[j] = buf[i + j];
				buf[tot++] = buf[i + j];
			}
			BitArrayToByteArray(bag_bit, bag_bit_len, bag_byte,bag_byte_len );
		//	printf("�յ������˿�Ϊ����ͷΪ��");
		//	print_data_byte(bag_byte, bag_byte_len, 1);
			i = i + bag_bit_len;
			if (bag_byte[5]<0 || bag_byte[5] * 8 + bag_bit_len>len) {
				return 0;
			}
			for (int j = 0; j < bag_byte[5]*8; j++) {
				buf[tot++] = buf[i + j];
			}
			
			U8* data_bit;
			int data_bit_len = bag_byte[5] * 8 + bag_bit_len;
			data_bit = (U8*)malloc(sizeof(U8) * data_bit_len);
			for (int j = 0; j < data_bit_len; j++) {
				if (j < (bag_size-4) * 8) {
					data_bit[j] = buf[j];
				}
				else if ((bag_size - 4) * 8 <= j && j < data_bit_len-16) {
					data_bit[j] = buf[j + 16];
				}
				else {
					data_bit[j] = buf[j - (data_bit_len - 16) + (bag_size - 4) * 8];
				}
			}
			cala_crc(data_bit, data_bit_len);
			for (int j = 0; j < 16; j++) {
				if (data_bit[data_bit_len - 16 + j] != 0) {
					return 0;
				}
			}
			return tot;
		}
	}
//	printf("֡ͬ������");
	return 0;
}
int ACK = 0;
void RecvfromLower(U8* buf, int len, int ifNo)
{
	int iSndRetval;
	U8* bufSend = NULL;
/*	if (ifNo == 0 && lowerNumber > 1) {
		//�ӽӿ�0�յ������ݣ�ֱ��ת�����ӿ�1 ���� �������ڲ���
		if (lowerMode[0] == lowerMode[1]) {
			//�ӿ�0��1�����ݸ�ʽ��ͬ��ֱ��ת��
			iSndRetval = SendtoLower(buf, len, 1);
			if (lowerMode[0] == 1) {
				iSndRetval = iSndRetval * 8;//����ӿڸ�ʽΪbit���飬ͳһ�����λ�����ͳ��
			}
		}
		else {
			//�ӿ�0��ӿ�1�����ݸ�ʽ��ͬ����Ҫת�����ٷ���
			if (lowerMode[0] == 1) {
				//�ӽӿ�0���ӿ�1���ӿ�0���ֽ����飬�ӿ�1�Ǳ������飬��Ҫ����8��ת��
				bufSend = (U8*)malloc(len * 8);
				if (bufSend == NULL) {
					cout << "�ڴ�ռ䲻������������û�б�����" << endl;
					return;
				}
				//byte to bit
				iSndRetval = ByteArrayToBitArray(bufSend, len * 8, buf, len);
				iSndRetval = SendtoLower(bufSend, iSndRetval, 1);
			}
			else {
				//�ӽӿ�0���ӿ�1���ӿ�0�Ǳ������飬�ӿ�1���ֽ����飬��Ҫ��С�˷�֮һת��
				bufSend = (U8*)malloc(len / 8 + 1);
				if (bufSend == NULL) {
					cout << "�ڴ�ռ䲻������������û�б�����" << endl;
					return;
				}
				//bit to byte
				iSndRetval = BitArrayToByteArray(buf, len, bufSend, len / 8 + 1);
				iSndRetval = SendtoLower(bufSend, iSndRetval, 1);
				
				iSndRetval = iSndRetval * 8;//�����λ����ͳ��

			}
		}
		//ͳ��
		if (iSndRetval <= 0) {
			iSndErrorCount++;
		}
		else {
			iRcvForward += iSndRetval;
			iRcvForwardCount++;
		}
	}
	else {*/
		//�ǽӿ�0�����ݣ����ߵͲ�ֻ��1���ӿڵ����ݣ������ϵݽ�
		if (lowerMode[ifNo] == 0) {
			int send_len;
			U8* buf_send;
			 
			send_len = tongbu(buf, len);
			
			if (send_len == 0) {
				printf("\n֡����\n");
				return;
			}
			buf_send = (U8*)malloc(sizeof(U8) * send_len);/////////////////////////////////////??????????????????????????????????????

			for (int i = 0; i < send_len; i++)buf_send[i] = buf[i];
			
			//����ӿ�0�Ǳ��������ʽ���߲�Ĭ�����ֽ����飬��ת�����ֽ����飬�����ϵݽ�
			bufSend = (U8*)malloc(send_len / 8+1);
			if (bufSend == NULL) {
				cout << "�ڴ�ռ䲻������������û�б�����" << endl;
				return;
			}
			iSndRetval = BitArrayToByteArray(buf_send, send_len, bufSend, send_len / 8+1);
			int bufSend_len=iSndRetval;
		//	printf("\n�յ������˿�Ϊ��%d",ifNo);
		//	print_data_byte(bufSend, bag_size - 2, 1);
		//	printf("�յ��²����ݣ�");
		//	print_data_byte(bufSend, iSndRetval, 1);
			if ((unsigned char)bufSend[1] == 240 || (unsigned char)bufSend[1] == 241) {
				if (lowerNumber == 1) {
					bufSend = bufSend + (bag_size - 2);
					iSndRetval = iSndRetval - (bag_size - 2);
					printf("\n�յ�%d��,���ϵݽ�֡Ϊ",(unsigned char)bufSend[1]);
				//	print_data_byte(bufSend, iSndRetval, 1);
					iSndRetval = SendtoUpper(bufSend, iSndRetval);
					iSndRetval = iSndRetval * 8;//�����λ,����ͳ��
				}
				else if (lowerNumber > 1) {
					if (!is_tree_cre) {
						U8* cre_buf;
						int cre_buf_len = bag_size;
						cre_buf = (U8*)malloc(sizeof(U8) * bag_size);
						add_bag_tree(cre_buf, 0);
						is_tree_cre = 1;
						int x = strDevID[0] - '0';
						int y = strEntity[0] - '0';
						int my_dr = x * 10 + y;
						tree_fa = my_dr;
						tree_de = 0;
						for (int i = 0; i < lowerNumber; i++) {
							if (lowerMode[i] == 0) {
								printf("\n���ͽ����������İ�,�˿�Ϊ��%d", i);
							//	print_data_byte(cre_buf, cre_buf_len, 1);
								U8* cre_buf_send;
								int cre_buf_send_len = cre_buf_len * 8;
								cre_buf_send = (U8*)malloc(sizeof(U8) * cre_buf_send_len);
								ByteArrayToBitArray(cre_buf_send, cre_buf_send_len, cre_buf, cre_buf_len);
								SendtoLower(cre_buf_send, cre_buf_send_len, i);
							}
							else {
								SendtoLower(cre_buf, cre_buf_len, i);
							}
						}
					}
					else 
					for (int i = 0; i < lowerNumber; i++) {
						if (i != ifNo && is_send_ok[i]) {//Դ���룺��  
							U8* buf_arp;
							int buf_arp_len = bufSend_len+2;
							buf_arp = (U8*)malloc(sizeof(U8) * buf_arp_len);
							add_bag_tongbu(bufSend, bufSend_len, buf_arp);
							printf("\n������ת��%d��,�˿�Ϊ��%d",(unsigned char)bufSend[1], i);
						//	print_data_byte(buf_arp, buf_arp_len, 1);
							if (lowerMode[i] == 0) {
								U8* buf_arp_send;
								int buf_arp_send_len = buf_arp_len * 8;
								buf_arp_send = (U8*)malloc(sizeof(U8) * buf_arp_send_len);
								ByteArrayToBitArray(buf_arp_send, buf_arp_send_len, buf_arp, buf_arp_len);
								SendtoLower(buf_arp_send, buf_arp_send_len, i);
							}
							else {
								SendtoLower(buf_arp, buf_arp_len, i);
							}
						}
					}
				}
			}
			else if (bufSend[2] == 0) {//arp
				if (lowerNumber == 1) {
					if (bufSend[3] == 0) {//ѯ��arp
						printf("\n�յ�arp��,�˿�Ϊ��0");
						IP2MAC[bufSend[0] / 10] = bufSend[0];
						treetable[bufSend[0]] = ifNo+1;
						U8* buf_arp;
						int buf_arp_len=3;
						buf_arp = (U8*)malloc(sizeof(U8) * 3);
						if (buf_arp == NULL)return;
						buf_arp[0] = bufSend[1] / 10;
						buf_arp[1] = bufSend[0];
						buf_arp[2] = 224;
						SendtoUpper(buf_arp, 3);
						free(buf_arp);
						/*
						if (bufSend[1] / 10 == strDevID[0] - '0') {
							U8* buf_arp;
							int buf_arp_len = bag_size;
							buf_arp = (U8*)malloc(sizeof(U8) * buf_arp_len);
							int x = strDevID[0] - '0';
							int y = strEntity[0] - '0';
							int my_dr = x * 10 + y;
							add_bag_arp_ack(buf_arp, my_dr, bufSend[0]);
							if (lowerMode[ifNo] == 0) {
								U8* buf_arp_send;
								int buf_arp_send_len = buf_arp_len * 8;
								buf_arp_send = (U8*)malloc(sizeof(U8) * buf_arp_send_len);
								ByteArrayToBitArray(buf_arp_send, buf_arp_send_len, buf_arp, buf_arp_len);
								printf("\n����ȷ��arp֡,�˿�Ϊ��%d����ͷΪ��",ifNo);
								print_data_byte(buf_arp, buf_arp_len, 1);
								SendtoLower(buf_arp_send, buf_arp_send_len, ifNo);
							}
							else {
								SendtoLower(buf_arp, buf_arp_len, ifNo);
							}
						}*/
					}
					else if (bufSend[3] == 1) {//ȷ��arp
						printf("\n�յ�arpȷ�ϰ�,�˿�Ϊ��0");
						IP2MAC[bufSend[4]] = bufSend[0];
						treetable[bufSend[0]] = ifNo+1;
					}
					
				}
				else if (lowerNumber > 1) {
					printf("\n�յ�arp��,�˿�Ϊ��%d",ifNo);
			//		if (treetable[bufSend[0]]) {// 1,0��ʾû��ֵ,.....�ж�����������û�������ߣ�û�еĻ����Ͳ�������㲥�İ�
						treetable[bufSend[0]] = ifNo+1;
						if (!is_tree_cre) {
							U8* cre_buf;
							int cre_buf_len = bag_size;
							cre_buf = (U8*)malloc(sizeof(U8) * bag_size);
							add_bag_tree(cre_buf, 0);
							is_tree_cre = 1;
							int x = strDevID[0] - '0';
							int y = strEntity[0] - '0';
							int my_dr = x * 10 + y;
							tree_fa = my_dr;
							tree_de = 0;
							for (int i = 0; i < lowerNumber; i++) {
								if (lowerMode[i] == 0) {
									printf("\n���ͽ����������İ�,�˿�Ϊ��%d", i);
								//	print_data_byte(cre_buf, cre_buf_len, 1);
									U8* cre_buf_send;
									int cre_buf_send_len = cre_buf_len * 8;
									cre_buf_send = (U8*)malloc(sizeof(U8) * cre_buf_send_len);
									ByteArrayToBitArray(cre_buf_send, cre_buf_send_len, cre_buf, cre_buf_len);
									SendtoLower(cre_buf_send, cre_buf_send_len, i);
								}
								else {
									SendtoLower(cre_buf, cre_buf_len, i);
								}
							}
						}
						else {
							if (treetable[bufSend[1]]) {
								
								int dk = treetable[bufSend[1]] - 1;
								U8* buf_arp;
								int buf_arp_len = bufSend_len + 2;
								buf_arp = (U8*)malloc(sizeof(U8) * buf_arp_len);
								add_bag_tongbu(bufSend, bufSend_len,buf_arp);
								if (lowerMode[dk] == 0) {
									U8* buf_arp_send;
									int buf_arp_send_len = buf_arp_len * 8;
									buf_arp_send = (U8*)malloc(sizeof(U8) * buf_arp_send_len);
									printf("\nת��arp��,�˿�Ϊ��%d",dk);
								//	print_data_byte(buf_arp, buf_arp_len, 1);
									ByteArrayToBitArray(buf_arp_send, buf_arp_send_len, buf_arp, buf_arp_len);
									SendtoLower(buf_arp_send, buf_arp_send_len, dk);
									
								}
								else {
									SendtoLower(buf_arp, buf_arp_len, dk);
								}
							}
							else {
								for (int i = 0; i < lowerNumber; i++) {
									if (i != ifNo && is_send_ok[i]) {//Դ���룺��  
										U8* buf_arp;
										int buf_arp_len = bufSend_len+2;
										buf_arp = (U8*)malloc(sizeof(U8) * buf_arp_len);
										add_bag_tongbu(bufSend, bufSend_len, buf_arp);
										printf("\nת��arp��,�˿�Ϊ��%d",i);
									//	print_data_byte(buf_arp, buf_arp_len, 1);
										if (lowerMode[i] == 0) {
											U8* buf_arp_send;
											int buf_arp_send_len = buf_arp_len * 8;
											buf_arp_send = (U8*)malloc(sizeof(U8) * buf_arp_send_len);
											ByteArrayToBitArray(buf_arp_send, buf_arp_send_len, buf_arp, buf_arp_len);
											SendtoLower(buf_arp_send, buf_arp_send_len, i);
										}
										else {
											SendtoLower(buf_arp, buf_arp_len, i);
										}
									}
								}
							}
						}
			//		}
				}
				
			}
			else if (bufSend[2] == 1) {//����������ȷ��֡
				if (bufSend[3] == 0) {
					adde(bufSend[0], ifNo);
				}
				else if (bufSend[3] == 1) {
					treetable[bufSend[0]] = ifNo+1;//ifNo+1!!!!!!
					is_send_ok[ifNo] = 1;
					printf("\n�˿ڿ��Է��������");
					for (int i = 0; i < lowerNumber; i++) {
						printf("\n%d:%d\n", i, is_send_ok[i]);
					}
					printf("\n��ַת����\n");
					for (int i = 0; i <= 99; i++) {
						if (treetable[i]) {
							printf("%02x��%d ", (unsigned char)i, treetable[i] - 1);
						}
					}
				}
			}
			else if (bufSend[2] == 2) {//����������֡
				is_tree_cre = 1;
				printf("�յ�������������,�˿�Ϊ��%d",ifNo);
				if (lowerNumber == 1) {//PC�յ���������������
					U8* buf_ack_pc;
					int buf_ack_pc_len=bag_size;
					buf_ack_pc = (U8*)malloc(sizeof(U8) * buf_ack_pc_len);
					add_bag_tree_ack_pc(buf_ack_pc,bufSend[0]);
					if (lowerMode[ifNo] == 0) {
						U8* buf_ack_pc_send;
						int buf_ack_pc_send_len = buf_ack_pc_len * 8;
						buf_ack_pc_send = (U8*)malloc(sizeof(U8) * buf_ack_pc_send_len);
						printf("����������ȷ�ϰ�,�˿�Ϊ��%d", ifNo);
						ByteArrayToBitArray(buf_ack_pc_send, buf_ack_pc_send_len, buf_ack_pc, buf_ack_pc_len);
						SendtoLower(buf_ack_pc_send, buf_ack_pc_send_len, ifNo);
					}
				}
				else if (lowerNumber > 1) {//�������յ���������������
					if (tree_de == -1) {
						tree_de = bufSend[3] + 1;
						tree_fa = bufSend[0];
						
						adde(bufSend[0], ifNo);
						U8* tree_ack;
						int tree_ack_len = bag_size;
						tree_ack = (U8*)malloc(sizeof(U8) * bag_size);//���ɽ���ȷ��֡
						add_bag_tree_ack(tree_ack, bufSend[0]);
						if (lowerMode[ifNo] == 0) {
							U8* tree_ack_send;
							int tree_ack_send_len = 8 * tree_ack_len;
							tree_ack_send = (U8*)malloc(sizeof(U8) * tree_ack_send_len);
							printf("����������ȷ�ϰ�,�˿�Ϊ��%d", ifNo);
							ByteArrayToBitArray(tree_ack_send, tree_ack_send_len, tree_ack, tree_ack_len);
							SendtoLower(tree_ack_send, tree_ack_send_len, ifNo);
						}
						else {
							SendtoLower(tree_ack, tree_ack_len, ifNo);
						}//ȷ��֡�������
						for (int i = 0; i < lowerNumber; i++) {
							if (i != ifNo) {
								U8* cre_tree;
								int cre_tree_len = bag_size;
								printf("����������������,�˿�Ϊ��%d", i);
								cre_tree = (U8*)malloc(sizeof(U8) * cre_tree_len);
								add_bag_tree(cre_tree, tree_de);
								if (lowerMode[i] == 0) {
									U8* cre_tree_send;
									int cre_tree_send_len = cre_tree_len * 8;
									cre_tree_send = (U8*)malloc(sizeof(U8) * cre_tree_send_len);
									ByteArrayToBitArray(cre_tree_send, cre_tree_send_len, cre_tree, cre_tree_len);
									SendtoLower(cre_tree_send, cre_tree_send_len, i);
								}
								else {
									SendtoLower(cre_tree, cre_tree_len, i);
								}
							}
						}
					}
					else if (tree_de > bufSend[3]) {
						//��������ӣ�������ɳ�cost�����
					}
				}
			}
			else if (bufSend[2] == 3) {//����Ϊ3ʱ��������֡
			/*	printf("\n��ַת����\n");
				for (int i = 0; i <= 99; i++) {
					if (treetable[i]) {
						printf("%02x��%d ", (unsigned char)i, treetable[i] - 1);
					}
				}*/
				if (lowerNumber == 1) {//
					bufSend = bufSend + (bag_size - 2);
					iSndRetval = iSndRetval - (bag_size - 2);
					printf("���ϵݽ�����֡");
				//	print_data_byte(bufSend, iSndRetval, 1);
					iSndRetval = SendtoUpper(bufSend, iSndRetval);
					iSndRetval = iSndRetval * 8;//�����λ,����ͳ��
					/*ԭ������������
					U8* buf_ack;
					int need_send=0;
					buf_ack = (U8*)malloc(sizeof(U8) * bag_size);
					int dst = bufSend[0];
					if (bufSend[3] == ACK) {
						ACK = (ACK + 1) % seq_size;
						bufSend = bufSend + (bag_size-2);
						iSndRetval = iSndRetval - (bag_size-2);
			//			printf("���ϵݽ�֡Ϊ��");
			//			print_data_byte(bufSend, iSndRetval, 1);
						iSndRetval = SendtoUpper(bufSend, iSndRetval);
						iSndRetval = iSndRetval * 8;//�����λ,����ͳ��
					}
					else need_send = 1;
					int ack_len = add_bag_ack(buf_ack, dst, ACK);
			//		printf("�յ�����֡������ȷ��֡Ϊ��");
			//		print_data_byte(buf_ack, bag_size, 1);
					if (1||need_send) {//�ط�ȷ��֡
						U8* buf_send_ack;
						int buf_send_ack_len;
						buf_send_ack = (U8*)malloc((ack_len * 8) * sizeof(U8));
						buf_send_ack_len = ByteArrayToBitArray(buf_send_ack, ack_len * 8, buf_ack, ack_len);
						buf_send_ack_len = SendtoLower(buf_send_ack, buf_send_ack_len, 0);
			//			printf("���ͳɹ�");
					}*/
				}
				else if (lowerNumber > 1) {//����������
					U8* buf_data;
					int buf_data_len = bufSend_len + 2;
					buf_data = (U8*)malloc(sizeof(U8) * buf_data_len);
					add_bag_tongbu(bufSend, bufSend_len, buf_data);
					if (treetable[buf_data[2]]) {
						if (lowerMode[treetable[buf_data[2]]-1]==0) {
							U8* buf_data_send;
							int buf_data_send_len = buf_data_len * 8;
							buf_data_send = (U8*)malloc(sizeof(U8) * buf_data_send_len);
							ByteArrayToBitArray(buf_data_send, buf_data_send_len, buf_data, buf_data_len);
							SendtoLower(buf_data_send, buf_data_send_len, treetable[buf_data[2]]-1);
							printf("���������������ˣ�������Ŀ��Ϊ��%d,���Ͷ˿�Ϊ��%d", buf_data[2], treetable[buf_data[2]] - 1);
						}
					}
					else {
						printf("�������Ҳ��� ·�ˣ�������������������������������������������������������������������");
					}
				}
			}
			else if (bufSend[2] == 4) {//ȷ��֡
				if (lowerNumber == 1) {
					//	printf("�յ�ȷ��֡");
					int win_l = -1, win_r = -1;
					for (int i = 0; i < window_len; i++) {
						if (win_len[i] != 0) {
							if (win_l == -1) {
								win_l = (win_data[i][4] + 1) % seq_size;
							}
							win_r = (win_data[i][4] + 1) % seq_size;
						}
					}
					int fg = 0;
					if (bufSend[3] >= win_l) {
						if (bufSend[3] <= win_r && win_r >= win_l) {
							fg = 1;
						}
						else if (bufSend[3] >= win_r && win_r <= win_l) {
							fg = 1;
						}
					}
					else {
						if (bufSend[3] <= win_r && win_r <= win_l) {
							fg = 1;
						}
					}
					printf("�������:%d,�����ұߣ�%d,ack:%d,fg:%d", win_l, win_r, bufSend[3], fg);
					if (fg == 1) {
						for (int i = 0; i < window_len; i++) {

							if (win_len[i] != 0 && win_data[i][4] != bufSend[3]) {
								free(win_data[i]);
								win_len[i] = 0;
								clk[i] = 0;
							}
							else {
								for (int j = i; j < window_len; j++) {
									swap(win_data[j], win_data[j - i]);
									swap(win_len[j], win_len[j - i]);
									swap(clk[j], clk[j - i]);
								}
								for (int j = window_len - i; j < window_len; j++) {
									if (da_fr_up_len[da_fr_up_fg] != 0) {
										if (lowerNumber == 1) {
											if (treetable[IP2MAC[da_fr_up[da_fr_up_fg][1]]]) {
												win_data[j] = (U8*)malloc(sizeof(U8*) * (da_fr_up_len[da_fr_up_fg] + bag_size));
												win_len[j] = add_bag(da_fr_up[da_fr_up_fg], da_fr_up_len[da_fr_up_fg], seq++, win_data[j]);
												seq %= seq_size;
												win_send(win_data[j], win_len[j], treetable[IP2MAC[da_fr_up[da_fr_up_fg][1]]] - 1);
												free(da_fr_up[da_fr_up_fg]);
												da_fr_up_len[da_fr_up_fg] = 0;
												da_fr_up_fg = (da_fr_up_fg + 1) % da_fr_up_sz;
												//				printf("���ڷ������ݣ�");
												//				print_data_byte(win_data[i], win_len[i], 1);

											}
											clk[j] = printCount;
										}
									}
								}
								break;
							}
						}
					}
					//		printf("ȷ��֡�������");
				}
				else if (lowerNumber > 1) {
					U8* buf_data;
					int buf_data_len = bufSend_len + 2;
					buf_data = (U8*)malloc(sizeof(U8) * buf_data_len);
					add_bag_tongbu(bufSend, bufSend_len, buf_data);
					if (lowerMode[treetable[buf_data[2]] - 1] == 0) {
						U8* buf_data_send;
						int buf_data_send_len = buf_data_len * 8;
						buf_data_send = (U8*)malloc(sizeof(U8) * buf_data_send_len);
						ByteArrayToBitArray(buf_data_send, buf_data_send_len, buf_data, buf_data_len);
						SendtoLower(buf_data_send, buf_data_send_len, treetable[buf_data[2]] - 1);
						printf("����������ȷ��֡�ˣ�������Ŀ��Ϊ��%d,���Ͷ˿�Ϊ��%d", buf_data[2], treetable[buf_data[2]] - 1);
					}
				}
			}
			else if (bufSend[2] == 5) {
				if (lowerNumber > 1) {
					for (int i = 0; i < lowerNumber; i++) {
						if (i != ifNo && is_send_ok[i]) {//Դ���룺��  
							U8* buf_arp;
							int buf_arp_len = bufSend_len+2;
							buf_arp = (U8*)malloc(sizeof(U8) * buf_arp_len);
							add_bag_tongbu(bufSend, bufSend_len, buf_arp);
							printf("\n������ת��5���Ͱ�,�˿�Ϊ��%d,Դ:%d,Ŀ�ģ�%d", i,(unsigned char)bufSend[0], (unsigned char)bufSend[1]);
						//	print_data_byte(buf_arp, buf_arp_len, 1);
							if (lowerMode[i] == 0) {
								U8* buf_arp_send;
								int buf_arp_send_len = buf_arp_len * 8;
								buf_arp_send = (U8*)malloc(sizeof(U8) * buf_arp_send_len);
								ByteArrayToBitArray(buf_arp_send, buf_arp_send_len, buf_arp, buf_arp_len);
								SendtoLower(buf_arp_send, buf_arp_send_len, i);
							}
							else {
								SendtoLower(buf_arp, buf_arp_len, i);
							}
						}
					}
				}
				else {
					bufSend = bufSend + (bag_size - 2);
					iSndRetval = iSndRetval - (bag_size - 2);
					printf("���ϵݽ�֡");
				//	print_data_byte(bufSend, iSndRetval, 1);
					iSndRetval = SendtoUpper(bufSend, iSndRetval);
					iSndRetval = iSndRetval * 8;
				}
			}
			iSndRetval = iSndRetval * 8;//�����λ,����ͳ��

		}
		else {
			//�Ͳ����ֽ�����ӿڣ���ֱ�ӵݽ�
			iSndRetval = SendtoUpper(buf, len);
			iSndRetval = iSndRetval * 8;//�����λ������ͳ��
		}
		//ͳ��
		if (iSndRetval <= 0) {
			iSndErrorCount++;
		}
		else {
			iRcvToUpper += iSndRetval;
			iRcvToUpperCount++;
		}
//	}
// 
	//�����Ҫ�ش��Ȼ��ƣ�������Ҫ��buf��bufSend�е�������������ռ仺������
/*	if (bufSend != NULL) {
		//����bufSend���ݣ�����б�Ҫ�Ļ�

		//��������û��ͣ��Э�飬bufSend�Ŀռ��������Ժ���Ҫ�ͷ�
		free(bufSend);
	}
	else {
		//����buf������ݣ�����б�Ҫ�Ļ�

		//buf�ռ䲻��Ҫ�ͷ�
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~");
	*/
	//��ӡ
	switch (iWorkMode % 10) {
	case 1:
		cout <<endl<< "���սӿ� " << ifNo << " ���ݣ�" << endl;
	//	print_data_bit(buf, len, lowerMode[ifNo]);
		break;
	case 2:
		cout << endl << "���սӿ� " << ifNo << " ���ݣ�" << endl;
	//	print_data_byte(buf, len, lowerMode[ifNo]);
		break;
	case 0:
		break;
	}
}
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
		cout << "��ת�� "<< iRcvForward<< " λ��"<< iRcvForwardCount<<" �Σ�"<<"�ݽ� "<< iRcvToUpper<<" λ��"<< iRcvToUpperCount<<" ��,"<<"���� "<< iSndTotal <<" λ��"<< iSndTotalCount<<" �Σ�"<< "���Ͳ��ɹ� "<< iSndErrorCount<<" ��,""�յ�������Դ "<< iRcvUnknownCount<<" �Ρ�";
		spin++;
	}

}
void menu()
{
	int x = strDevID[0] - '0';
	int y = strEntity[0] - '0';
	int my_dr = x * 10 + y;
	printf("%d %d@@%d\n", x, y,my_dr);
	int selection;
	unsigned short port;
	int iSndRetval;
	char kbBuf[100];
	int len;
	U8* bufSend;
	//����|��ӡ��[���Ϳ��ƣ�0���ȴ��������룻1���Զ���][��ӡ���ƣ�0�������ڴ�ӡͳ����Ϣ��1����bit����ӡ���ݣ�2���ֽ�����ӡ����]
	cout << endl << endl << "�豸��:" << strDevID << ",    ���:" << strLayer << ",    ʵ���:" << strEntity;
	cout << endl << "1-�����Զ�����(��Ч);" << endl << "2-ֹͣ�Զ����ͣ���Ч��; " << endl << "3-�Ӽ������뷢��; ";
	cout << endl << "4-����ӡͳ����Ϣ; " << endl << "5-����������ӡ��������;" << endl << "6-���ֽ�����ӡ��������;";
	cout << endl << "0-ȡ��" << endl << "����������ѡ�����";
	cin >> selection;
	switch (selection) {
	case 0:

		break;
	case 1:
		iWorkMode = 10 + iWorkMode % 10;
		break;
	case 2:
		iWorkMode = iWorkMode % 10;
		break;
	case 3:
		cout << "�����ַ���(,������100�ַ�)��";
		cin >> kbBuf;
		cout << "����Ͳ�ӿںţ�";
		cin >> port;

		len = (int)strlen(kbBuf) + 1; //�ַ�������и�������
		if (port >= lowerNumber) {
			cout << "û������ӿ�" << endl;
			return;
		}
		if (lowerMode[port] == 0) {
			//�²�ӿ��Ǳ���������,��ҪһƬ�µĻ�����ת����ʽ
			bufSend = (U8*)malloc(len * 8);

			iSndRetval = ByteArrayToBitArray(bufSend, len * 8, kbBuf, len);
			iSndRetval = SendtoLower(bufSend, iSndRetval, port);
			free(bufSend);
		}
		else {
			//�²�ӿ����ֽ����飬ֱ�ӷ���
			iSndRetval = SendtoLower(kbBuf, len, port);
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
		cout << endl << "��ӿ� " << port << " �������ݣ�" << endl;
		switch (iWorkMode % 10) {
		case 1:
			print_data_bit(kbBuf, len, 1);
			break;
		case 2:
			print_data_byte(kbBuf, len, 1);
			break;
		case 0:
			break;
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
	}
}