#ifndef _CLIENT_INTERFACE_H
#define _CLIENT_INTERFACE_H

extern int LKJ_status_flag ;//LKJ_TAX_status׼���ú���LKJ_TAX_status_flag��֪ͨ��������
extern char LKJ_TAX_status ;//��������:1.��ȫƽ̨��LKJ��ͨ��״̬, ����ʱ��λLKJ_TAX_status�ĵ�0λ
					        //2.״̬��Ϣ��Ļ������ݰ汾�ͻ���������İ汾��Ϣ�Ƿ�һ��,һ��ʱ��λLKJ_TAX_status�ĵ�1λ
					        //3.�Ż����Ͱ�ȫƽ̨��ͨ��״̬,����ʱ��λLKJ_TAX_status�ĵ�2λ
							//4.���������ʾ��Ϣʱ����λLKJ_TAX_status

typedef struct _status_flag{
	int LKJ_status_flag;//������ʶ��ȫƽ̨��LKJ����ƥ��
	int jieshi_status_flag;//������ʶ��ʾ��Ϣ�Ѿ�׼����
} STATUS_FLAG;

extern STATUS_FLAG status_flag;

extern unsigned char cli_flag_g;    //ѡ����Ӧ��ģʽ: 0x00--��ʼ�� 0x01--���������ļ� 0x02--��վ���� 0x04--��ʾ��Ϣ 0xff--������־

void comm_client();
#endif
