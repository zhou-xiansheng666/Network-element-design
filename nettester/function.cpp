//Nettester �Ĺ����ļ�
#include <iostream>
#include <conio.h>
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

/****SPIRIT****/
int count_to_200 = 105;
int count_to_410 = 0;
U8 RoutingTable[15][4];	// Ŀ�ĵأ��������ӿڣ�IP��
int pRT = 0;
int is_Init_RT = 0;	//���ڼ�¼�Ƿ��ʼ��·�ɱ�����������û��ô������
int gateway = 0;
int checkon[10] = { 0,0,0,0,0,0,0,0,0,0 };
int shutdown_port[10] = { 1,1,1,1,1,1,1,1,1,1 };

void SendHello(void);
void SendHelloBack(int ifNo);
void UpdateRT(U8* buf, int len, int ifNo);
void DeleteRT(void);
void ARPresponse(U8* buf,int len,int ifNo);
int is_ARP(U8* buf,int len,int ifNo);
/****GABRIEL****/






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
	if(sendbuf != NULL)
		free(sendbuf);
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
void TimeOut()
{
	count_to_200++;
	printCount++;
	count_to_410++;
	checkon[0]++;
	checkon[1]++;
	checkon[2]++;
	checkon[3]++;
	checkon[4]++;
	checkon[5]++;
	checkon[6]++;
	checkon[7]++;
	checkon[8]++;
	checkon[9]++;

	if (_kbhit()) {
		//�����ж���������˵�ģʽ
		menu();
	}

	if (!is_Init_RT)	//�����û��ʼ��·�ɱ�
	{
		memset(RoutingTable, 0, 60);
		pRT = 0;	//����·�ɱ�����0��
		is_Init_RT = 1;
	}

	if (count_to_200 >= 199)
	{
		SendHello();
		count_to_200 = 0;
	}

	if (count_to_410 >= 410 && lowerNumber > 1)
	{
		count_to_410 = 0;
		DeleteRT();
	}









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
	int iSndRetval;
	U8* bufSend = NULL;
	U8* buf_change = NULL;
	//�Ǹ߲����ݣ�ֻ�ӽӿ�0����ȥ,�߲�ӿ�Ĭ�϶����ֽ������ݸ�ʽ
	if (lowerMode[0] == 0) {
		//�ӿ�0��ģʽΪbit���飬��ת����bit���飬�ŵ�bufSend��
		
		iSndRetval = ByteArrayToBitArray(bufSend, len * 8, buf, len);
		//����
		iSndRetval = SendtoLower(bufSend, iSndRetval, 0); //��������Ϊ���ݻ��壬���ȣ��ӿں�
	}
	else {
		//�²����ֽ�����ӿڣ���ֱ�ӷ���
		printf("APP�������ݳ���:%d\n", len);
		len = len + 2;
		buf_change = (U8*)malloc(len);
		if (buf_change == NULL)
			return;
		

		for (int i = 0 ; i < len-2; i++)
		{
			buf_change[i+2] = buf[i];
		}
		buf_change[0] = strDevID[0]-48; // �����Լ����豸��
		buf_change[1] = gateway;
		if (gateway == 0)
			buf_change[1] = buf_change[2];
		

		/*bufSend = (U8*)malloc(len * 8);
		if (bufSend == NULL) {
			return;
		}*/
		printf("Recvfromupper����lowerǰһ�̵ĳ���:%d\n", len);


		iSndRetval = SendtoLower(buf_change, len, 0);
		
		iSndRetval = iSndRetval * 8;//�����λ
		free(buf_change);
		
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
	if (shutdown_port[ifNo] == 0)	// ����ӿ����ֶ��ر��ˣ��������κ����ݣ���
		return;
	checkon[ifNo] = 0;
	int iSndRetval = 0;
	U8* bufSend = NULL;
	printf("\n�豸�ţ�%d\n", strDevID[0]-48);
	if (is_ARP(buf,len,ifNo))
	{
		printf("�յ�ARP\n");
		ARPresponse( buf, len, ifNo);
	}
	if (lowerNumber > 1) 
	{
		// printf("����·������ǰ���ֽڣ�%d %d %hhu\n",buf[0],buf[1],buf[2]);
		printf("����·����\n");
		if ((unsigned char)buf[1] == 240)	//	�յ�hello����
		{
			printf("�յ�hello����, src:%d ",buf[0]);
			for (int i = 0; i < buf[2]; i++)
			{
				printf("|%d %d ", buf[3 + 2 * i], buf[3 + 2 * i + 1]);
			}
			printf("\n");
			UpdateRT(buf,len,ifNo);	// ����·�ɱ�
			SendHelloBack(ifNo);
			
		}
		else if ((unsigned char)buf[1] == 241)	// �յ�helloback����
		{
			printf("�յ�helloback����\n");
			UpdateRT(buf, len, ifNo);	// ����·�ɱ�
			
		}
		else // �����ܲ���ת��
		{
			for (int i = 0; i < pRT; i++)
			{
				if ((unsigned char)buf[1] == RoutingTable[i][0]) // �ҵ�һ����
				{
					U8* bufchange = (U8*)malloc(len + 1);
					if (bufchange == NULL)
						return;
					bufchange[0] = buf[0];
					bufchange[1] = RoutingTable[i][3];	// ��һ����˭
					for (int i = 1; i < len; i++)
					{
						bufchange[i + 1] = buf[i];
					}
					printf("ת��һ�����ģ��ӿ�%d����һ��%d\n", RoutingTable[i][2], RoutingTable[i][3]);
					SendtoLower(bufchange, len+1, RoutingTable[i][2]);
					free(bufchange);
				}
					
			}
		}
		
	}
	else // �����LNKʵ��ֻ��һ��
	{
		//�ǽӿ�0�����ݣ����ߵͲ�ֻ��1���ӿڵ����ݣ������ϵݽ�
		printf("��������\n");
		if ((unsigned char)buf[1] == 240)	//	�յ�hello����
		{
			SendHelloBack(ifNo);
			gateway = buf[0];

		}
		if (lowerMode[ifNo] == 0) {
			//����ӿ�0�Ǳ��������ʽ���߲�Ĭ�����ֽ����飬��ת�����ֽ����飬�����ϵݽ�
			bufSend = (U8*)malloc(len / 8 + 1);
			if (bufSend == NULL) {
				cout << "�ڴ�ռ䲻������������û�б�����" << endl;
				return;
			}
			iSndRetval = BitArrayToByteArray(buf, len, bufSend, len / 8 + 1);
			iSndRetval = SendtoUpper(bufSend, iSndRetval);
			iSndRetval = iSndRetval * 8;//�����λ,����ͳ��

		}
		else {
			//�Ͳ����ֽ�����ӿڣ���ֱ�ӵݽ�
			U8 src = buf[0];
			if (buf[1] == strDevID[0]-48)
			{
				
				printf("\nlen:%d\n",len);
				//U8* buf_change = NULL;
				len = len - 1;
				//buf_change = (U8*)malloc(len);
				//buf_change = buf;
				for (int i = 1; i < len; i++)
				{
					buf[i] = buf[i + 1]; // Ĩȥ���й����Լ��豸�ŵ�����
				}
				
				iSndRetval = SendtoUpper(buf, len);
				iSndRetval = iSndRetval * 8;//�����λ������ͳ��
			}
			//iSndRetval = SendtoUpper(buf, len);
			//iSndRetval = iSndRetval * 8;//�����λ������ͳ��
		}
		//ͳ��
		if (iSndRetval <= 0) {
			iSndErrorCount++;
		}
		else {
			iRcvToUpper += iSndRetval;
			iRcvToUpperCount++;
		}
	}
	//�����Ҫ�ش��Ȼ��ƣ�������Ҫ��buf��bufSend�е�������������ռ仺������
	if (bufSend != NULL) {
		//����bufSend���ݣ�����б�Ҫ�Ļ�

		//��������û��ͣ��Э�飬bufSend�Ŀռ��������Ժ���Ҫ�ͷ�
		free(bufSend);
	}
	else {
		//����buf������ݣ�����б�Ҫ�Ļ�

		//buf�ռ䲻��Ҫ�ͷ�
	}

	//��ӡ
	switch (iWorkMode % 10) {
	case 1:
		cout <<endl<< "���սӿ� " << ifNo << " ���ݣ�" << endl;
		print_data_bit(buf, len, lowerMode[ifNo]);
		break;
	case 2:
		cout << endl << "���սӿ� " << ifNo << " ���ݣ�" << endl;
		print_data_byte(buf, len, lowerMode[ifNo]);
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
	int selection;
	unsigned short port;
	int iSndRetval;
	char kbBuf[100];
	int len;
	int ret;
	U8* bufSend;
	//����|��ӡ��[���Ϳ��ƣ�0���ȴ��������룻1���Զ���][��ӡ���ƣ�0�������ڴ�ӡͳ����Ϣ��1����bit����ӡ���ݣ�2���ֽ�����ӡ����]
	cout << endl << endl << "�豸��:" << strDevID << ",    ���:" << strLayer << ",    ʵ���:" << strEntity ;
	cout << endl << "1-�رսӿ�;" << endl << "2-�򿪽ӿ�; " << endl << "3-�Ӽ������뷢��; ";
	cout << endl << "4-����ӡͳ����Ϣ; " << endl << "5-����������ӡ��������;" << endl << "6-���ֽ�����ӡ��������;";
	cout << endl << "0-ȡ��" << endl << "����������ѡ�����";
	cin >> selection;
	switch (selection) {
	case 0:

		break;
	case 1:
		printf("��Ҫ�ر��ĸ��˿ڣ� ");
		scanf_s("%d", &ret);
		shutdown_port[ret] = 0;
		printf("\nOK!\n");
		break;
	case 2:
		printf("��Ҫ���ĸ��˿ڣ� ");
		scanf_s("%d", &ret);
		shutdown_port[ret] = 1;
		printf("\nOK!\n");
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





// ����Hello���ģ�����·�ɱ����
void SendHello(void)
{
	if (lowerNumber > 1)	// ��Ϊ·������ʱ��
	{
		printf("����Hello�������£�\n");
		U8 src = strDevID[0] - 48;
		unsigned char dst = 240;	// 1111,0000
		U8 len = pRT;	// �Լ��ı�����
		printf("src:%d dst:%d ��Ŀ:%d ", src, dst, len);
		U8* hellopacket = (U8*)malloc(3 + len * 2);
		if (hellopacket == NULL)
			return;
		hellopacket[0] = src;
		hellopacket[1] = dst;
		hellopacket[2] = len;
		int i;
		for (i = 0; i < len; i++)
		{
			hellopacket[3 + i*2] = RoutingTable[i][0];
			hellopacket[3 + i*2 + 1] = RoutingTable[i][1];
			printf("|%d %d ", hellopacket[3 + i * 2], hellopacket[3 + i * 2 + 1]);
		}
		printf("\n");
		for (i = 0; i < lowerNumber; i++)
		{
			if(shutdown_port[i] != 0)
				SendtoLower(hellopacket, 3 + len * 2, i);	//	����Χ����ȥ
		}
		free(hellopacket);
	}
}

void SendHelloBack(int ifNo)
{
	U8 src = strDevID[0] - 48;
	unsigned char dst = 241;	// 1111,0001
	U8 len = pRT;	// �Լ��ı�����
	U8* hellobackpacket = (U8*)malloc(3 + len * 2);
	if (hellobackpacket == NULL)
		return;
	hellobackpacket[0] = src;
	hellobackpacket[1] = dst;
	hellobackpacket[2] = len;
	if (lowerNumber > 1)
	{
		for (int i = 0; i < len; i++)
		{
			hellobackpacket[3 + i * 2] = RoutingTable[i][0];
			hellobackpacket[3 + i * 2 + 1] = RoutingTable[i][1];
		}
	}
	SendtoLower(hellobackpacket, 3 + len * 2, ifNo);
	free(hellobackpacket);
}


//����·�ɱ�
void UpdateRT(U8* buf, int len, int ifNo)
{
	if (lowerNumber <= 1)	// �ٴ�ȷ���ⲻ������
		return;
	int i;
	int j;
	// ����ѧϰ
	int is_in = 0;
	// ������������ĵĽڵ��ڲ����Լ���·�ɱ�����
	for (i = 0; i < pRT; i++)
	{
		if (RoutingTable[i][0] == buf[0])
			is_in = 1;
	}
	if (!is_in)	// ������ھͷŽ�ȥ
	{
		RoutingTable[pRT][0] = buf[0];
		RoutingTable[pRT][1] = 1;	// ֱ��
		RoutingTable[pRT][2] = ifNo;
		RoutingTable[pRT][3] = buf[0];	// ֱ����������
		pRT++;	// ����һ����Ŀ
		if (pRT >= 15)
			pRT = 14; // ��Խ��
	}
	for (i = 0; i < buf[2]; i++)	// �鿴hello������ÿһ���������ܲ��ܸ���ԭ·�ɱ�
	{
		is_in = 0;
		for (j = 0; j < pRT; j++)
		{
			if (RoutingTable[j][0] == buf[3 + i * 2])	//	������
			{
				is_in = 1;
				if (RoutingTable[j][1] > buf[3 + i * 2 + 1] + 1) // ���������·�ɸ���
				{
					RoutingTable[j][1] = buf[3 + i * 2 + 1] + 1;
					RoutingTable[j][2] = ifNo;
					RoutingTable[j][3] = buf[0];	// ֱ����������
				}
			}
		}
		if (!is_in && buf[3+i*2] != strDevID[0] - '0')
		{
			RoutingTable[pRT][0] = buf[3 + i * 2];
			RoutingTable[pRT][1] = buf[3 + i * 2 + 1] + 1;
			RoutingTable[pRT][2] = ifNo;
			RoutingTable[pRT][3] = buf[0];	// ֱ����������
			pRT++;	// ����һ����Ŀ
			if (pRT >= 15)
				pRT = 14;
		}
	}
	printf("Update��ǰ·�ɱ�\n");
	for (int i = 0; i < pRT; i++)
	{
		printf("%d %d %d %d\n", RoutingTable[i][0], RoutingTable[i][1], RoutingTable[i][2], RoutingTable[i][3]);
	}
}


void DeleteRT(void)
{
	int i;
	for (i = 0; i < 10; i++)
	{
		if (checkon[i] >= 400)
		{
			for (int j = 0; j < pRT; j++)
			{
				if (RoutingTable[j][2] == i)
				{
					for (int k = j; k < pRT; k++)
					{
						RoutingTable[k][0] = RoutingTable[k + 1][0];
						RoutingTable[k][1] = RoutingTable[k + 1][1];
						RoutingTable[k][2] = RoutingTable[k + 1][2];
						RoutingTable[k][3] = RoutingTable[k + 1][3];
						pRT--;
					}
				}
			}
		}
	}
	printf("Delete��ǰ·�ɱ�\n");
	for (i = 0; i < pRT; i++)
	{
		printf("%d %d %d %d\n", RoutingTable[i][0], RoutingTable[i][1], RoutingTable[i][2], RoutingTable[i][3]);
	}
}


int is_ARP(U8* buf, int len, int ifNo)
{
	if (buf[0] == strDevID[0] - '0' && (unsigned char)buf[2] == 224)
	{
		// printf("ARP��������:%d %d %d\n", buf[0], buf[1], buf[2]);
		return 1;
	}
	else 
		return 0;
}


void ARPresponse(U8* buf, int len, int ifNo)
{
	buf[2] = 225;
	SendtoLower(buf, len, ifNo);
}
