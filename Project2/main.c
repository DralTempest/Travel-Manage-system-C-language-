#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <winbase.h>
#include <conio.h>
//#define WIN32_LEAN_AND_MEAN 
#include <WS2tcpip.h> 
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
//cJSON�ⲿ������
#include "cJSON/cJSON.h"



struct menu
{
	int n;
	char menu_s[100][100];
}menus[100];
enum level
{
	iron,
	bronze,
	silver,
	gold,
	diamond
};
enum destination
{
	Guangzhou,
	Litang,
	Shanghai,
	Jilin
};
struct time
{
	int month;
	int day;
};
struct node 
{
	int level;
	int all_day;
	int destination;
	double bill;
	char name[100];
	struct time date;
	struct node* next;
};
struct node* dummy = NULL; //�Ƶ�
struct node* head = NULL;
struct node* tail = NULL;
struct node* loadfileinfor();

void print(struct node* head);
void print_no_circle(struct node* p);

struct node* inforinput();

void allmenu(struct menu menus[]);
void menushow(int menuchoice, int funchocie);
void gotoxy(int x, int y);
// ͳ��
void sort_by_level(struct node* front);
void sort_by_bill(struct node* front);
//��ѯ
void search_by_level(int lev, struct node* check);
void search_by_level_and_date(int lev, struct node* check, int month, int day);
void search_by_name(char name_s[], struct node* check);
void search_by_destination(struct node* check, int p);
void search_by_destination_and_date(struct node* check, int p, int month, int day);
//����ʱ��
void print_time(int p);
int travel_time(int month, int day);

// api ��ʽ
// api.seniverse.com/v3/weather/daily.json?key=SPvsbW1tlsFrmR-p5&location=beijing&language=zh-Hans&unit=c&start=0&days=5
const char* host = "api.seniverse.com";
const char* key = "SPvsbW1tlsFrmR-p5";
const char location[5][12] = {"guangzhou", "lasa", "shanghai", "changchun"};
const char* language = "zh-Hans";
char gurl[100] = { 0 }; // ���ջ�ȡ��url��ַ
char* generateUrl(int n)
{
	// ����GET ����
	sprintf(gurl,"GET /v3/weather/daily.json?key=%s&location=%s&language=%s&unit=c&start=0&days=5\r\n\r\n",key,location[n], language);
	return gurl;
}


//��ʼ�������
void initSocket()
{
	WSADATA data;
	if (0 != WSAStartup(MAKEWORD(2, 2), &data))
	{
		printf("SWAStartup error!n,%d", WSAGetLastError());
	}
}

//���ӷ�����
SOCKET connectToHost(const char* host,unsigned short port)
{
	// ��ʼ�� addrinfo �ṹ��
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    // ���� IPv4 �� IPv6
	hints.ai_socktype = SOCK_STREAM; // ʹ�� TCP
	hints.ai_protocol = IPPROTO_TCP; // ָ�� TCP Э��

	// ���� getaddrinfo ������ȡ��ַ��Ϣ
	struct addrinfo* result;
	int status = getaddrinfo(host, "80", &hints, &result);
	if (status) {
		printf("getaddrinfo error: %s\n", gai_strerror(status));
		return INVALID_SOCKET;
	}

	// ���� addrinfo ������������
	struct addrinfo* rp;
	SOCKET fd = INVALID_SOCKET;  // ��ʼ�� fd Ϊ INVALID_SOCKET
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		// ���� Socket
		fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (fd == INVALID_SOCKET) {
			printf("socket creation error: %d\n", WSAGetLastError());
			continue;
		}

		// �������ӵ�������
		if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
			break;  // ���ӳɹ����˳�ѭ��
		}
		else {
			printf("connect error: %d\n", WSAGetLastError());
			closesocket(fd);  // �ر� Socket��������һ����ַ
			fd = INVALID_SOCKET;  // ���� fd Ϊ INVALID_SOCKET
		}
	}

	// ����Ƿ��ҵ���Ч�ĵ�ַ
	if (rp == NULL) {
		printf("Could not connect to %s\n", host);
		freeaddrinfo(result);
		return INVALID_SOCKET;
	}

	// �ͷ� addrinfo ����
	freeaddrinfo(result);

	// ��ʱ��fd �Ѿ����ɹ���ʼ��Ϊһ����Ч�� Socket
	return fd;
}

typedef struct {
	char date[11];           // ���ڣ��� "2024-12-31"
	char text_day[20];       // ���������������� "��"
	char high[8];              // ����¶ȣ��� 7.0��C
	char low[8];               // ����¶ȣ��� -7.0��C
} DailyForecast;

int main()
{
	loadfileinfor();
	head = dummy->next;
	allmenu(menus);
	int menuchoice = 0, funchoice = 0; //ǰ���ǲ˵�ѡ�񣬺����ǹ���ѡ��
	while (1)
	{
		UINT original_cp = GetConsoleOutputCP(); // �����Ϊ�˱����ʼ�ı��룬�Ա��������������������Խ��
		menushow(menuchoice, funchoice);
		gotoxy(30, 15);
		if (menuchoice  == 0)
		{
			fflush(stdin); // ��ջ�����
			printf("*��������Ҫѡ�����Ŀ>>  ");
			scanf_s("%d", &funchoice);
		}
		if (menuchoice == 0)
		{
			switch (funchoice)
			{
			case 1:
				system("cls");
				inforinput();
				break;
			case 2:
				menuchoice = 1;
				system("cls");
				break;
			case 3:
				menuchoice = 2;
				system("cls");
				break;
			case 4:
			{
				system("cls");
				int n;
				gotoxy(35, 10);
				printf("[0]����  [1]����  [2]�Ϻ�  [3]����\n");
				gotoxy(35, 11);
				printf("��������Ҫ��ѯ�ĵص�ʱ�䣺\n");
				gotoxy(35, 12);
				scanf("%d",&n);
				system("cls");
				print_time(n);
			}
				break;
			case 5:
			{
				system("cls");
				int n;
				printf("[0]����  [1]����  [2]�Ϻ�  [3]����\n");
				printf("��������Ҫ��ѯ�ĵص�--->>>");
				scanf("%d",&n);
				
				initSocket();
				SOCKET fd = connectToHost(host, 80);
				if (fd != INVALID_SOCKET) printf("���ӳɹ���%d\n", WSAGetLastError());
				else
				{
					printf("����ʧ�ܣ��������%d\n", WSAGetLastError());
					system("pause");
					break;
				}

				//��������
				SetConsoleCP(UNICODE);
				char* url = generateUrl(n);
				puts(url);
				if (0 >= send(fd, url, strlen(url), 0)) printf("send error!,%d\n", WSAGetLastError());
				else
				{
					// ���ͳɹ�������������������ݷ���
					char buffer[BUFSIZ * 10] = { 0 };
					int total_received = 0;
					int remaining = sizeof(buffer);

					while (total_received < sizeof(buffer))  //ѭ����������
					{
						int recv_len = recv(fd, buffer + total_received, remaining, 0);
						if (recv_len == -1) {
							perror("recv error");
							break;
						}
						else if (recv_len == 0) {
							break;  // �Զ˹ر�����
						}
						total_received += recv_len;
						remaining -= recv_len;
					}

					if (total_received > 0) {
						//puts(buffer);
						FILE* fp = fopen("weather.json", "w");
						if (!fp) {
							perror("create file error!\n");
						}
						else {
							fwrite(buffer, sizeof(char), total_received, fp);
							fclose(fp);
						}
					}
					else {
						printf("No data received.\n");
					}
				}
			}
			//�����ļ�
			FILE* fp = fopen("weather.json", "r+");
			if (!fp) {
				perror("�޷����ļ�\n");
				return 1;
			}
			fseek(fp, 0, SEEK_END);  // ��λ���ļ�ĩβ
			long length = ftell(fp);  // ��ȡ�ļ�����
			fseek(fp, 0, SEEK_SET);  // ��λ���ļ���ͷ
			char* buff = malloc(length + 1);  // ���仺����
			if (buff) {
				fread(buff, 1, length, fp);  // ��ȡ�ļ����ݵ�������
				buff[length] = '\0';  // ����ַ���������
			}
			//puts(buff);
			fclose(fp);  // �ر��ļ�

			// ����JSON����
			cJSON* json = cJSON_Parse(buff);  // ����JSON�ַ���
			free(buff);  // �ͷŻ�����
			if (!json) {
				const char* error_ptr = cJSON_GetErrorPtr();  // ��ȡ������Ϣ
				if (error_ptr) {
					fprintf(stderr, "JSON��������: %s\n", error_ptr);
				}
				return 1;
			}

			// ��ȡ`daily`����
			cJSON* results = cJSON_GetObjectItemCaseSensitive(json, "results");  // ��ȡ`results`����
			if (cJSON_IsArray(results)) {
				cJSON* first_result = cJSON_GetArrayItem(results, 0);  // ��ȡ`results`����ĵ�һ��Ԫ��
				cJSON* daily = cJSON_GetObjectItemCaseSensitive(first_result, "daily");  // ��ȡ`daily`����
				if (cJSON_IsArray(daily)) {
					int number_of_forecasts = cJSON_GetArraySize(daily);  // ��ȡ`daily`����ĳ���
					DailyForecast* forecasts = malloc(sizeof(DailyForecast) * number_of_forecasts);  // ��������Ԥ������
					if (!forecasts) {
						perror("�ڴ����ʧ��\n");
						cJSON_Delete(json);  // �ͷ�cJSON����
						return 1;
					}

					// ����`daily`���飬��ȡ��������
					for (int i = 0; i < number_of_forecasts; i++) {
						cJSON* forecast_item = cJSON_GetArrayItem(daily, i);  // ��ȡ`daily`����ĵ�i��Ԫ��
						cJSON* date_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "date");  // ��ȡ`date`�ֶ�
						if (cJSON_IsString(date_item)) {
							strcpy(forecasts[i].date, date_item->valuestring);  // ���������ַ���
						}

						cJSON* text_day_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "text_day");  // ��ȡ`text_day`�ֶ�
						if (cJSON_IsString(text_day_item)) {
							strcpy(forecasts[i].text_day, text_day_item->valuestring);  // ���ư�����������
						}

						cJSON* high_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "high");  // ��ȡ`high`�ֶ�
						if (cJSON_IsString(high_item)) {
							strcpy(forecasts[i].high, high_item->valuestring);  // ת������¶�
						}

						cJSON* low_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "low");  // ��ȡ`low`�ֶ�
						if (cJSON_IsString(low_item)) {
							strcpy(forecasts[i].low, low_item->valuestring);  // ת������¶�
						}
					}
					// ��ӡ��ȡ����������
					printf("�����������£�\n");
					for (int i = 0; i < number_of_forecasts; i++) {

						printf("Date: %s, ", forecasts[i].date);
						SetConsoleOutputCP(CP_UTF8);
						printf("Weather: % s ", forecasts[i].text_day);
						SetConsoleOutputCP(original_cp);
						printf("Highest temperature : %s��C, lowest temperature : %s��C\n", forecasts[i].high, forecasts[i].low);
					}
					cJSON_Delete(json);  // �ͷ�cJSON����
					free(forecasts);  // �ͷ�����Ԥ������
				}
				else {
					fprintf(stderr, "δ�ҵ�`daily`����\n");
					cJSON_Delete(json);  // �ͷ�cJSON����
					return 1;
				}
			}
			else {
				fprintf(stderr, "δ�ҵ�`results`����\n");
				cJSON_Delete(json);  // �ͷ�cJSON����
				return 1;
			}
			SetConsoleOutputCP(original_cp);
			system("pause");
				break;
			case 6:
			{
				system("cls");
				int leve;
				char name_log[20] = { 0 };
				while (1)
				{
					system("cls");
					printf("������ȼ���\n");
					printf("[1]��ͭ  [2]����  [3]�ƽ�  [4]��ʯ\n");
					scanf_s("%d", &leve);
					if (leve >= 1 && leve <= 5) break;
					else printf("�������룡\n");
				}
				printf("�������û�������\n");
				scanf("%s", name_log);
				system("cls");
				struct node* temp = head;
				while (temp!=NULL)
				{
					if (travel_time(temp->date.month, temp->date.day, temp->all_day)==1&&temp->level==leve&&strcmp(temp->name,name_log)==0)
					{
						printf("�����ڽ���Ҫ��ʼ�ˣ�\n");
						break;
					}
					if (travel_time(temp->date.month, temp->date.day, temp->all_day) == 2||(travel_time(temp->date.month, temp->date.day, temp->all_day)==3&&temp->level>=2)&& temp->level == leve && strcmp(temp->name, name_log) == 0)
					{
						initSocket();
						SOCKET fd = connectToHost(host, 80);
						if (fd != INVALID_SOCKET) printf("���ӳɹ���%d\n", WSAGetLastError());
						else
						{
							printf("����ʧ�ܣ��������%d\n", WSAGetLastError());
							system("pause");
							break;
						}

						//��������
						SetConsoleCP(UNICODE);
						char* url = generateUrl(temp->destination);
						puts(url);
						if (0 >= send(fd, url, strlen(url), 0)) printf("send error!,%d\n", WSAGetLastError());
						else
						{
							// ���ͳɹ�������������������ݷ���
							char buffer[BUFSIZ * 10] = { 0 };
							int total_received = 0;
							int remaining = sizeof(buffer);

							while (total_received < sizeof(buffer))  //ѭ����������
							{
								int recv_len = recv(fd, buffer + total_received, remaining, 0);
								if (recv_len == -1) {
									perror("recv error");
									break;
								}
								else if (recv_len == 0) {
									break;  // �Զ˹ر�����
								}
								total_received += recv_len;
								remaining -= recv_len;
							}

							if (total_received > 0) {
								//puts(buffer);
								FILE* fp = fopen("weather.json", "w");
								if (!fp) {
									perror("create file error!\n");
								}
								else {
									fwrite(buffer, sizeof(char), total_received, fp);
									fclose(fp);
								}
							}
							else {
								printf("No data received.\n");
							}
						}
					}
					//�����ļ�
					FILE* fp = fopen("weather.json", "r+");
					if (!fp) {
						perror("�޷����ļ�\n");
						return 1;
					}
					fseek(fp, 0, SEEK_END);  // ��λ���ļ�ĩβ
					long length = ftell(fp);  // ��ȡ�ļ�����
					fseek(fp, 0, SEEK_SET);  // ��λ���ļ���ͷ
					char* buff = malloc(length + 1);  // ���仺����
					if (buff) {
						fread(buff, 1, length, fp);  // ��ȡ�ļ����ݵ�������
						buff[length] = '\0';  // ����ַ���������
					}
					//puts(buff);
					fclose(fp);  // �ر��ļ�

					// ����JSON����
					cJSON* json = cJSON_Parse(buff);  // ����JSON�ַ���
					free(buff);  // �ͷŻ�����
					if (!json) {
						const char* error_ptr = cJSON_GetErrorPtr();  // ��ȡ������Ϣ
						if (error_ptr) {
							fprintf(stderr, "JSON��������: %s\n", error_ptr);
						}
						return 1;
					}

					// ��ȡ`daily`����
					cJSON* results = cJSON_GetObjectItemCaseSensitive(json, "results");  // ��ȡ`results`����
					if (cJSON_IsArray(results)) {
						cJSON* first_result = cJSON_GetArrayItem(results, 0);  // ��ȡ`results`����ĵ�һ��Ԫ��
						cJSON* daily = cJSON_GetObjectItemCaseSensitive(first_result, "daily");  // ��ȡ`daily`����
						if (cJSON_IsArray(daily)) {
							int number_of_forecasts = cJSON_GetArraySize(daily);  // ��ȡ`daily`����ĳ���
							DailyForecast* forecasts = malloc(sizeof(DailyForecast) * number_of_forecasts);  // ��������Ԥ������
							if (!forecasts) {
								perror("�ڴ����ʧ��\n");
								cJSON_Delete(json);  // �ͷ�cJSON����
								return 1;
							}

							// ����`daily`���飬��ȡ��������
							for (int i = 0; i < number_of_forecasts; i++) {
								cJSON* forecast_item = cJSON_GetArrayItem(daily, i);  // ��ȡ`daily`����ĵ�i��Ԫ��
								cJSON* date_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "date");  // ��ȡ`date`�ֶ�
								if (cJSON_IsString(date_item)) {
									strcpy(forecasts[i].date, date_item->valuestring);  // ���������ַ���
								}

								cJSON* text_day_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "text_day");  // ��ȡ`text_day`�ֶ�
								if (cJSON_IsString(text_day_item)) {
									strcpy(forecasts[i].text_day, text_day_item->valuestring);  // ���ư�����������
								}

								cJSON* high_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "high");  // ��ȡ`high`�ֶ�
								if (cJSON_IsString(high_item)) {
									strcpy(forecasts[i].high, high_item->valuestring);  // ת������¶�
								}

								cJSON* low_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "low");  // ��ȡ`low`�ֶ�
								if (cJSON_IsString(low_item)) {
									strcpy(forecasts[i].low, low_item->valuestring);  // ת������¶�
								}
							}
							// ��ӡ��ȡ����������
							printf("�����������£�\n");
							for (int i = 0; i < number_of_forecasts; i++) {

								printf("Date: %s, ", forecasts[i].date);
								SetConsoleOutputCP(CP_UTF8);
								printf("Weather: % s ", forecasts[i].text_day);
								SetConsoleOutputCP(original_cp);
								printf("Highest temperature : %s��C, lowest temperature : %s��C\n", forecasts[i].high, forecasts[i].low);
							}
							cJSON_Delete(json);  // �ͷ�cJSON����
							free(forecasts);  // �ͷ�����Ԥ������
							break;
						}
						else {
							fprintf(stderr, "δ�ҵ�`daily`����\n");
							cJSON_Delete(json);  // �ͷ�cJSON����
							return 1;
						}
					}
					else {
						fprintf(stderr, "δ�ҵ�`results`����\n");
						cJSON_Delete(json);  // �ͷ�cJSON����
						return 1;
					}
					temp = temp->next;
				}
				system("pause");




















			}
				break;
			case 7:
				system("pause");
				return 0;

			default:
				break;
			}
		}
		if (menuchoice == 1)
		{
			menushow(menuchoice, funchoice);
			gotoxy(30, 10);
			printf("��������Ҫѡ�����Ŀ>>  ");
			scanf_s("%d", &funchoice);
			switch (funchoice)
			{
			case 1: // �����û��ȼ������������������
			{
				sort_by_level(dummy);
				system("cls");
				head = dummy->next;//����ͷָ��
				struct node* temp = head;
				print(temp);
				system("pause");
				system("cls");
				break;
			}
			case 2 :
				sort_by_bill(dummy);
				system("cls");
				head = dummy->next;//����ͷָ��
				struct node* temp = head;
				print(temp);
				system("pause");
				system("cls");
				break;
			case 0: menuchoice = 0;
				break;
			}
		}
		if (menuchoice == 2)
		{
			menushow(menuchoice, funchoice);
			gotoxy(30, 15);
			printf("��������Ҫѡ�����Ŀ>>  ");
			scanf_s("%d", &funchoice);
			switch (funchoice)
			{
			case 1:  //���û��ȼ���ѯ,���Ӵ�С
			{
				system("cls");
				int lev;
				sort_by_bill(dummy);
				head = dummy->next;//����ͷָ��
				struct node* temp = head;
				while (1)
				{
					system("cls");
					printf("������ȼ���\n");
					printf("[1]��ͭ  [2]����  [3]�ƽ�  [4]��ʯ\n");
					scanf_s("%d", &lev);
					if (lev >= 1 && lev <= 5) break;
					else printf("�������룡\n");
				}
				system("cls");
				printf("�ȼ�\t����\t\tĿ�ĵ�\t\t����\t���(Ԫ)    ����ʱ��(��-��)\n");
				search_by_level(lev, temp);
				system("pause");
			}
				break;
			case 2: // ���û��ȼ������ڲ�ѯ�����Ӵ�С��word�ĵ������ɰ����û��ȼ��Ӹߵ��ͣ������û��ȼ��Ѿ�ȷ���ˣ��޷�����
			{
				system("cls");
				int lev,month, day;
				sort_by_bill(dummy);
				head = dummy->next;//����ͷָ��
				struct node* temp = head;
				while (1)
				{
					system("cls");
					printf("������ȼ���\n");
					printf("[1]��ͭ  [2]����  [3]�ƽ�  [4]��ʯ\n");
					scanf_s("%d", &lev);
					if (lev >= 1 && lev <= 5) break;
					else printf("�������룡\n");
				}
				while (1)
				{
					system("cls");
					printf("�������·ݣ�\n");
					scanf_s("%d",&month);
					if (month >= 1 &&month <= 12) break;
					else printf("�������룡\n");
				}
				while (1)
				{
					system("cls");
					printf("�������գ�\n");
					scanf_s("%d", &day);
					if (day >= 1 && month <= 31) break;
					else printf("�������룡\n");
				}
				system("cls");
				printf("�ȼ�\t����\t\tĿ�ĵ�\t\t����\t���(Ԫ)    ����ʱ��(��-��)\n");
				search_by_level_and_date(lev, temp,month,day);
				system("pause");
			}
			break;
			case 3: // ��������ȥ��ѯ
			{
				system("cls");
				char search_name[100] = "0";
				head = dummy->next;//����ͷָ��
				struct node* temp = head;
				printf("�������û�������\n");
				scanf("%s",search_name);
				system("cls");
				printf("�ȼ�\t����\t\tĿ�ĵ�\t\t����\t���(Ԫ)    ����ʱ��(��-��)\n");
				search_by_name(search_name,temp);
				system("pause");
				system("cls");
				break;
			}
			case 4: //����Ŀ�ĵ�ȥ��ѯ
			{
				system("cls");
				int position;
				sort_by_bill(dummy);
				head = dummy->next;
				struct node* temp = head;
				printf("----- ����Ŀ�ĵأ�-----\n");
				printf("[0]����  [1]����  [2]�Ϻ�  [3]����\n");
				scanf("%d",&position);
				system("cls");
				printf("�ȼ�\t����\t\tĿ�ĵ�\t\t����\t���(Ԫ)    ����ʱ��(��-��)\n");
				search_by_destination(temp, position);
				system("pause");
				system("cls");
			}
				break;
			case 5: //����Ŀ�ĵغ����ڽ��в�ѯ
			{
				system("cls");
				int position,month,day;
				sort_by_level(dummy);
				head = dummy->next;
				struct node* temp = head;
				while (1)
				{
					system("cls");
					printf("----- ����Ŀ�ĵأ�-----\n");
					printf("[0]����  [1]����  [2]�Ϻ�  [3]����\n");
					scanf("%d", &position);
					if (position >= 1 && position <= 5) break;
					else printf("�������룡\n");
				}
				while (1)
				{
					system("cls");
					printf("�������·ݣ�\n");
					scanf_s("%d", &month);
					if (month >= 1 && month <= 12) break;
					else printf("�������룡\n");
				}
				while (1)
				{
					system("cls");
					printf("�������գ�\n");
					scanf_s("%d", &day);
					if (day >= 1 && month <= 31) break;
					else printf("�������룡\n");
				}
				system("cls");
				printf("�ȼ�\t����\t\tĿ�ĵ�\t\t����\t���(Ԫ)    ����ʱ��(��-��)\n");
				search_by_destination_and_date(temp, position,month,day);
				system("pause");
				system("cls");
			}
				break;
			case 0: menuchoice = 0;break;
			}
		}
		system("cls");
	}
	return 0;
}

struct node* loadfileinfor() // ������Ϣ
{
	FILE* file = fopen("G:\\travel_project\\Project2\\data.txt","r+");
	if (file == NULL) {
		perror("�޷����ļ�");
		exit(1);
	}
	struct node* current = (struct node*)malloc(sizeof(struct node)); // �����Ƶ㣬�ձ�
	dummy = current; 
	while (1)
	{
		struct node* temp = (struct node*)malloc(sizeof(struct node));
		current->next = temp;
		int n = fscanf(file, "%d\t%s\t%d\t%d\t%lf\t%d\t%d\n", &temp->level, temp->name, &temp->destination, &temp->all_day, &temp->bill, &temp->date.month, &temp->date.day);
		if (n == EOF) // ��ȡ���ļ�������ʱ����-1�����ʱ�򱣴�βָ�������ڴ�
		{
			tail = current; //����βָ��
			free(temp);
			tail->next = NULL;
			break;
		}
		current = current->next;
		temp->next = NULL;
	}
	fclose(file);
	return dummy;
}
struct node* inforinput() // ¼����Ϣ
{
	FILE* add = fopen("G:\\travel_project\\Project2\\data.txt", "a+");
	struct node* newnode = (struct node*)malloc(sizeof(struct node));
	tail->next = newnode;
	newnode->next = NULL;
	tail = newnode;
	printf("������ȼ���\n");
	printf("[1]��ͭ  [2]����  [3]�ƽ�  [4]��ʯ\n");
	scanf_s("%d",&newnode->level);
	fflush(stdin);
	system("cls");
	printf("������������\n");
	scanf("%s", newnode->name);
	system("cls");
	fflush(stdin);
	printf("----- ����Ŀ�ĵأ�-----\n");
	printf("[0]����  [1]����  [2]�Ϻ�  [3]����\n");
	scanf_s("%d", &newnode->destination);
	system("cls");
	fflush(stdin);
	printf("����������ʱ��(��)��\n");
	scanf_s("%d", &newnode->all_day);
	system("cls");
	fflush(stdin);
	printf("�������\n");
	scanf("%lf", &newnode->bill);
	system("cls");
	fflush(stdin);
	printf("�����붩���£�\n");
	scanf_s("%d", &newnode->date.month);
	fflush(stdin);
	system("cls");
	printf("�����붩���գ�\n");
	scanf_s("%d",  &newnode->date.day);
	system("cls");
	
	int n = fprintf(add, "%d\t%s\t%d\t%d\t%lf\t%d\t%d\n", newnode->level, newnode->name, newnode->destination, newnode->all_day, newnode->bill, newnode->date.month, newnode->date.day);
	if (n != EOF)
	{
		printf("¼��ɹ���");
	}
	else printf("ERROR!"); // ���󾯸�
	fclose(add);
	system("pause");
	return tail;
};
void print(struct node* p)
{
	printf("�ȼ�\t����\t\tĿ�ĵ�\t\t����\t���(Ԫ)    ����ʱ��(��-��)\n");
	while (p != NULL)
	{
		if (strlen(p->name)>=8)
		{
			switch (p->destination)
			{
			case Guangzhou:printf("%d\t%-4s\tGuangzhou\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
			case Litang:printf("%d\t%-4s\tLitang\t\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
			case Shanghai:printf("%d\t%-4s\tShanghai\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
			case Jilin:printf("%d\t%-4s\tJilin\t\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
			default:
				break;
			}
		}
		else
		{
			switch (p->destination)
			{
			case Guangzhou:printf("%d\t%-4s\t\tGuangzhou\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
			case Litang:printf("%d\t%-4s\t\tLitang\t\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
			case Shanghai:printf("%d\t%-4s\t\tShanghai\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
			case Jilin:printf("%d\t%-4s\t\tJilin\t\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
			default:
				break;
			}
		}
		p = p->next;
	}
}
// ͳ�ƹ��ܲ���
void sort_by_level(struct node* front) //ð�����򣬽ڵ㽻��
{
	struct node* prev ; //ǰ��ָ��
	struct node* p;  // ǰһ��ָ��
	struct node* q; //��һ��ָ��
	struct node* end = NULL; // ��ָֹ��
	while (front->next!=end)
	{
		prev = front; 
		p = front->next;
		q = p->next;
		while (p->next!=end) // ��һ����������max/minָ���ֹͣ�������´ӵ�һ����ʼ��
		{
			if (p->level < q->level)
			{
				prev->next = q;
				p->next = q->next;
				q->next = p;
			}
			else if (p->level == q->level && p->bill < q->bill)
			{
				prev->next = q;
				p->next = q->next;
				q->next = p;
			}
			else
			{
				p = p->next;
			}
			q = p->next;
			prev = prev->next;
		}
		end = p;
	}
}
void sort_by_bill(struct node* front)
{
	struct node* prev; //ǰ��ָ��
	struct node* p;  // ǰһ��ָ��
	struct node* q; //��һ��ָ��
	struct node* end = NULL; // ��ָֹ��
	while (front->next != end)
	{
		prev = front;
		p = front->next;
		q = p->next;
		while (p->next != end) // ��һ����������max/minָ���ֹͣ�������´ӵ�һ����ʼ��
		{
			if (p->bill < q->bill)
			{
				prev->next = q;
				p->next = q->next;
				q->next = p;
			}
			else if (p->bill == q->bill && p->level < q->level)
			{
				prev->next = q;
				p->next = q->next;
				q->next = p;
			}
			else
			{
				p = p->next;
			}
			q = p->next;
			prev = prev->next;
		}
		end = p;
	}
}

//��ѯ���ܲ���
void search_by_level(int lev, struct node* check)
{
	int n = 0;
	while (check->next != NULL)
	{
		if (check->level == lev)
		{
			print_no_circle(check);
			n++;
		}
		check = check->next;
	}
	printf("                  --------�ܹ���ѯ��%d���û�---------\n", n);
}
void search_by_level_and_date(int lev, struct node* check, int month, int day)
{
	int n = 0;
	while (check->next!=NULL)
	{
		if (check->level == lev && check->date.month == month && check->date.day == day)
		{
			print_no_circle(check);
			n++;
		}
		check = check->next;
	}
	printf("                  --------�ܹ���ѯ��%d���û�---------\n",n);
}
void search_by_name(char name_s[],struct node* check)
{
	int n = 0;
	while (check->next!=NULL)
	{
		if ((strcmp(name_s,check->name))==0)
		{
			print_no_circle(check);
			n++;
		}
		check = check->next;
	}
	printf("                  --------�ܹ���ѯ��%d�����---------\n", n);
}
void search_by_destination(struct node* check, int p)
{
	int n = 0;
	while (check->next != NULL)
	{
		if (p == check->destination)
		{
			print_no_circle(check);
			n++;
		}
		check = check->next;
	}
	printf("                  --------�ܹ���ѯ��%d�����---------\n", n);
}
void search_by_destination_and_date(struct node* check, int p, int month, int day)
{
	int n = 0;
	while (check->next != NULL)
	{
		if (check->destination == p && check->date.month == month && check->date.day == day)
		{
			print_no_circle(check);
			n++;
		}
		check = check->next;
	}
	printf("                  --------�ܹ���ѯ��%d�����---------\n", n);
}



//�˵�����
void allmenu(struct menu menus[])
{
	//���˵�
	strcpy(menus[0].menu_s[0], "|--------  1.¼���û���Ϣ  --------|");
	strcpy(menus[0].menu_s[1], "|--------  2.ͳ���û���Ϣ  --------|");
	strcpy(menus[0].menu_s[2], "|--------  3.��ѯ�û���Ϣ  --------|");
	strcpy(menus[0].menu_s[3], "|-------- 4.��ѯĿ�ĵ�ʱ�� --------|");
	strcpy(menus[0].menu_s[4], "|-------- 5.��ѯĿ�ĵ����� --------|");
	strcpy(menus[0].menu_s[5], "|-------- 6.�û��˵�����   --------|");
	strcpy(menus[0].menu_s[6], "|--------     7.�˳�      ---------|");

	//ͳ���Ӳ˵�
	strcpy(menus[1].menu_s[0], "|---       1.���û��ȼ�ͳ��     ---|");
	strcpy(menus[1].menu_s[1], "|---       2.�Զ������ͳ��     ---|");
	strcpy(menus[1].menu_s[2], "|---       0.������һ��         ---|");

	//��ѯ�Ӳ˵�
	strcpy(menus[2].menu_s[0], "|---      1.���û��ȼ���ѯ       --|");
	strcpy(menus[2].menu_s[1], "|---      2.�����ں��û��ȼ���ѯ  -|");
	strcpy(menus[2].menu_s[2], "|---      3.��������ѯ           --|");
	strcpy(menus[2].menu_s[3], "|---      4.��Ŀ�ĵز�ѯ         --|");
	strcpy(menus[2].menu_s[4], "|---      5.�����ں�Ŀ�ĵز�ѯ    -|");
	strcpy(menus[2].menu_s[5], "|---      0.������һ��          ---|");
}
void menushow(int menuchoice,int funchocie)
{
	system("cls");
	gotoxy(40, 2);
	printf("*----------------------------------*\n");
	gotoxy(40, 3);
	printf("|-----------    �˵�    -----------|\n");
	gotoxy(40, 4);
	printf("|----------------------------------|\n");
	if (menuchoice==0)
	{
	   for (int i = 0; i < 10; i++)
	   {
			gotoxy(40, 5+i);
			printf("%s\n", menus[0].menu_s[i]);
	   }
	   gotoxy(40, 12);
	   printf("*----------------------------------*\n");
	}
	if (menuchoice==1)
	{
		for (int i = 0; i < 10; i++)
		{
			gotoxy(40, 5+i);
			printf("%s\n", menus[1].menu_s[i]);
		}
		gotoxy(40, 8);
		printf("*----------------------------------*\n");
	}
	if (menuchoice == 2)
	{
		for (int i = 0; i < 10; i++)
		{
			gotoxy(40, 5+i);
			printf("%s\n", menus[2].menu_s[i]);
		}
		gotoxy(40, 11);
		printf("*----------------------------------*\n");
	}
	
}
void gotoxy(int x, int y) 
{
	COORD pos = { x,y };
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);// ��ȡ��׼����豸���
	SetConsoleCursorPosition(hOut, pos);//���������ֱ���ָ���ĸ����壬����λ��
}
void print_no_circle(struct node* p)
{
	if (strlen(p->name) >= 8)
	{
		switch (p->destination)
		{
		case Guangzhou:printf("%d\t%-4s\tGuangzhou\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
		case Litang:printf("%d\t%-4s\tLitang\t\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
		case Shanghai:printf("%d\t%-4s\tShanghai\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
		case Jilin:printf("%d\t%-4s\tJilin\t\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
		default:
			break;
		}
	}
	else
	{
		switch (p->destination)
		{
		case Guangzhou:printf("%d\t%-4s\t\tGuangzhou\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
		case Litang:printf("%d\t%-4s\t\tLitang\t\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
		case Shanghai:printf("%d\t%-4s\t\tShanghai\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
		case Jilin:printf("%d\t%-4s\t\tJilin\t\t%-2d\t%-4.2lf\t\t%02d-%02d\n", p->level, p->name, p->all_day, p->bill, p->date.month, p->date.day); break;
		default:
			break;
		}
	}
}

//Ŀ�ĵ�ʱ�����ɹ���  �Ա���ʱ��Ϊ����ʱ�� ����ǲ�ȡutf+8��ʽ�Ļ�������ͨ������ʱ���ǿ��Ժ��Ե�
void print_time(int p)
{
	int arr[4] = { 112,100,120,126 }; // ���ݣ��������Ϻ������� �ľ���
	char des[4][12] = { "����","����","�Ϻ�","����" };
	while (1)
	{
		system("cls");
			time_t raw_time; //ԭʼ����ʱ��
			struct tm* local_tm, * utc_tm;
			time(&raw_time);
			local_tm = localtime(&raw_time);

			// ����Ŀ��ʱ��ΪUTC+n Сʱ �þ�γ��ȥ��
			double utc_offset = (arr[p]-116)/15 * 3600;  // 8Сʱת��Ϊ��

			// ����ʱ��
			time_t target_time = raw_time + utc_offset;
			utc_tm = localtime(&target_time); //����ʱ��

			// ��ʽ�����
			char buffer[80];
			strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", utc_tm); 
			gotoxy(40, 6);
			printf("*-----      ʱ���ѯ     -----*\n");
			gotoxy(40, 7);
			printf("|---     ������%sʱ��    ---|\n", des[p]);
			gotoxy(40, 8);
			printf("|---  %s  ---|\n", buffer);
			gotoxy(40, 9);
			printf("*----- �������������һ��-----*\n");
		if (_kbhit()) {
			char key = getch(); //���������Ļ
			break;
		}
		Sleep(1);
	}
}

//����ǰ��ʱ�����
int travel_time(int month,int day,int travel_day)
{
	time_t raw_time; //ԭʼ����ʱ��
	struct tm* local_tm,*utc_tm,*travel_tm;
	time(&raw_time);
	local_tm = localtime(&raw_time);
	double utc_offset = 24 * 3600;
	time_t target_time = raw_time - utc_offset;
	utc_tm = localtime(&target_time);
	


	if (month == local_tm->tm_mon && day==local_tm->tm_mday) // ����
	{
		return 1;
	}
	if (month == utc_tm->tm_mon && day== utc_tm->tm_mday) // ǰһ��
	{
		return 2;
	}
	if (month+1.0*travel_day/30<=local_tm->tm_mon)
	{
		return 3;
	}
	return 0;
	
}

