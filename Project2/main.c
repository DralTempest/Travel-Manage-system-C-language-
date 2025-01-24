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
//cJSON外部解析库
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
struct node* dummy = NULL; //哑点
struct node* head = NULL;
struct node* tail = NULL;
struct node* loadfileinfor();

void print(struct node* head);
void print_no_circle(struct node* p);

struct node* inforinput();

void allmenu(struct menu menus[]);
void menushow(int menuchoice, int funchocie);
void gotoxy(int x, int y);
// 统计
void sort_by_level(struct node* front);
void sort_by_bill(struct node* front);
//查询
void search_by_level(int lev, struct node* check);
void search_by_level_and_date(int lev, struct node* check, int month, int day);
void search_by_name(char name_s[], struct node* check);
void search_by_destination(struct node* check, int p);
void search_by_destination_and_date(struct node* check, int p, int month, int day);
//生成时间
void print_time(int p);
int travel_time(int month, int day);

// api 格式
// api.seniverse.com/v3/weather/daily.json?key=SPvsbW1tlsFrmR-p5&location=beijing&language=zh-Hans&unit=c&start=0&days=5
const char* host = "api.seniverse.com";
const char* key = "SPvsbW1tlsFrmR-p5";
const char location[5][12] = {"guangzhou", "lasa", "shanghai", "changchun"};
const char* language = "zh-Hans";
char gurl[100] = { 0 }; // 最终获取的url地址
char* generateUrl(int n)
{
	// 请求GET 数据
	sprintf(gurl,"GET /v3/weather/daily.json?key=%s&location=%s&language=%s&unit=c&start=0&days=5\r\n\r\n",key,location[n], language);
	return gurl;
}


//初始化网络库
void initSocket()
{
	WSADATA data;
	if (0 != WSAStartup(MAKEWORD(2, 2), &data))
	{
		printf("SWAStartup error!n,%d", WSAGetLastError());
	}
}

//连接服务器
SOCKET connectToHost(const char* host,unsigned short port)
{
	// 初始化 addrinfo 结构体
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    // 允许 IPv4 或 IPv6
	hints.ai_socktype = SOCK_STREAM; // 使用 TCP
	hints.ai_protocol = IPPROTO_TCP; // 指定 TCP 协议

	// 调用 getaddrinfo 函数获取地址信息
	struct addrinfo* result;
	int status = getaddrinfo(host, "80", &hints, &result);
	if (status) {
		printf("getaddrinfo error: %s\n", gai_strerror(status));
		return INVALID_SOCKET;
	}

	// 遍历 addrinfo 链表，尝试连接
	struct addrinfo* rp;
	SOCKET fd = INVALID_SOCKET;  // 初始化 fd 为 INVALID_SOCKET
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		// 创建 Socket
		fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (fd == INVALID_SOCKET) {
			printf("socket creation error: %d\n", WSAGetLastError());
			continue;
		}

		// 尝试连接到服务器
		if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
			break;  // 连接成功，退出循环
		}
		else {
			printf("connect error: %d\n", WSAGetLastError());
			closesocket(fd);  // 关闭 Socket，尝试下一个地址
			fd = INVALID_SOCKET;  // 重置 fd 为 INVALID_SOCKET
		}
	}

	// 检查是否找到有效的地址
	if (rp == NULL) {
		printf("Could not connect to %s\n", host);
		freeaddrinfo(result);
		return INVALID_SOCKET;
	}

	// 释放 addrinfo 链表
	freeaddrinfo(result);

	// 此时，fd 已经被成功初始化为一个有效的 Socket
	return fd;
}

typedef struct {
	char date[11];           // 日期，如 "2024-12-31"
	char text_day[20];       // 白天天气描述，如 "晴"
	char high[8];              // 最高温度，如 7.0°C
	char low[8];               // 最低温度，如 -7.0°C
} DailyForecast;

int main()
{
	loadfileinfor();
	head = dummy->next;
	allmenu(menus);
	int menuchoice = 0, funchoice = 0; //前者是菜单选择，后者是功能选择
	while (1)
	{
		UINT original_cp = GetConsoleOutputCP(); // 这个是为了保存初始的编码，以便后面天气的中文乱码可以解决
		menushow(menuchoice, funchoice);
		gotoxy(30, 15);
		if (menuchoice  == 0)
		{
			fflush(stdin); // 清空缓冲区
			printf("*请输入你要选择的项目>>  ");
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
				printf("[0]广州  [1]理塘  [2]上海  [3]吉林\n");
				gotoxy(35, 11);
				printf("请输入你要查询的地点时间：\n");
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
				printf("[0]广州  [1]理塘  [2]上海  [3]吉林\n");
				printf("请输入你要查询的地点--->>>");
				scanf("%d",&n);
				
				initSocket();
				SOCKET fd = connectToHost(host, 80);
				if (fd != INVALID_SOCKET) printf("连接成功！%d\n", WSAGetLastError());
				else
				{
					printf("连接失败！错误代码%d\n", WSAGetLastError());
					system("pause");
					break;
				}

				//发送请求
				SetConsoleCP(UNICODE);
				char* url = generateUrl(n);
				puts(url);
				if (0 >= send(fd, url, strlen(url), 0)) printf("send error!,%d\n", WSAGetLastError());
				else
				{
					// 发送成功，服务器把请求的数据发送
					char buffer[BUFSIZ * 10] = { 0 };
					int total_received = 0;
					int remaining = sizeof(buffer);

					while (total_received < sizeof(buffer))  //循环接收数据
					{
						int recv_len = recv(fd, buffer + total_received, remaining, 0);
						if (recv_len == -1) {
							perror("recv error");
							break;
						}
						else if (recv_len == 0) {
							break;  // 对端关闭连接
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
			//解析文件
			FILE* fp = fopen("weather.json", "r+");
			if (!fp) {
				perror("无法打开文件\n");
				return 1;
			}
			fseek(fp, 0, SEEK_END);  // 定位到文件末尾
			long length = ftell(fp);  // 获取文件长度
			fseek(fp, 0, SEEK_SET);  // 定位到文件开头
			char* buff = malloc(length + 1);  // 分配缓冲区
			if (buff) {
				fread(buff, 1, length, fp);  // 读取文件内容到缓冲区
				buff[length] = '\0';  // 添加字符串结束符
			}
			//puts(buff);
			fclose(fp);  // 关闭文件

			// 解析JSON数据
			cJSON* json = cJSON_Parse(buff);  // 解析JSON字符串
			free(buff);  // 释放缓冲区
			if (!json) {
				const char* error_ptr = cJSON_GetErrorPtr();  // 获取错误信息
				if (error_ptr) {
					fprintf(stderr, "JSON解析错误: %s\n", error_ptr);
				}
				return 1;
			}

			// 提取`daily`数组
			cJSON* results = cJSON_GetObjectItemCaseSensitive(json, "results");  // 提取`results`数组
			if (cJSON_IsArray(results)) {
				cJSON* first_result = cJSON_GetArrayItem(results, 0);  // 提取`results`数组的第一个元素
				cJSON* daily = cJSON_GetObjectItemCaseSensitive(first_result, "daily");  // 提取`daily`数组
				if (cJSON_IsArray(daily)) {
					int number_of_forecasts = cJSON_GetArraySize(daily);  // 获取`daily`数组的长度
					DailyForecast* forecasts = malloc(sizeof(DailyForecast) * number_of_forecasts);  // 分配天气预报数组
					if (!forecasts) {
						perror("内存分配失败\n");
						cJSON_Delete(json);  // 释放cJSON对象
						return 1;
					}

					// 遍历`daily`数组，提取天气数据
					for (int i = 0; i < number_of_forecasts; i++) {
						cJSON* forecast_item = cJSON_GetArrayItem(daily, i);  // 提取`daily`数组的第i个元素
						cJSON* date_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "date");  // 提取`date`字段
						if (cJSON_IsString(date_item)) {
							strcpy(forecasts[i].date, date_item->valuestring);  // 复制日期字符串
						}

						cJSON* text_day_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "text_day");  // 提取`text_day`字段
						if (cJSON_IsString(text_day_item)) {
							strcpy(forecasts[i].text_day, text_day_item->valuestring);  // 复制白天天气描述
						}

						cJSON* high_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "high");  // 提取`high`字段
						if (cJSON_IsString(high_item)) {
							strcpy(forecasts[i].high, high_item->valuestring);  // 转换最高温度
						}

						cJSON* low_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "low");  // 提取`low`字段
						if (cJSON_IsString(low_item)) {
							strcpy(forecasts[i].low, low_item->valuestring);  // 转换最低温度
						}
					}
					// 打印提取的天气数据
					printf("当地天气如下：\n");
					for (int i = 0; i < number_of_forecasts; i++) {

						printf("Date: %s, ", forecasts[i].date);
						SetConsoleOutputCP(CP_UTF8);
						printf("Weather: % s ", forecasts[i].text_day);
						SetConsoleOutputCP(original_cp);
						printf("Highest temperature : %s°C, lowest temperature : %s°C\n", forecasts[i].high, forecasts[i].low);
					}
					cJSON_Delete(json);  // 释放cJSON对象
					free(forecasts);  // 释放天气预报数组
				}
				else {
					fprintf(stderr, "未找到`daily`数组\n");
					cJSON_Delete(json);  // 释放cJSON对象
					return 1;
				}
			}
			else {
				fprintf(stderr, "未找到`results`数组\n");
				cJSON_Delete(json);  // 释放cJSON对象
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
					printf("请输入等级：\n");
					printf("[1]青铜  [2]白银  [3]黄金  [4]钻石\n");
					scanf_s("%d", &leve);
					if (leve >= 1 && leve <= 5) break;
					else printf("重新输入！\n");
				}
				printf("请输入用户姓名：\n");
				scanf("%s", name_log);
				system("cls");
				struct node* temp = head;
				while (temp!=NULL)
				{
					if (travel_time(temp->date.month, temp->date.day, temp->all_day)==1&&temp->level==leve&&strcmp(temp->name,name_log)==0)
					{
						printf("旅游在今天要开始了！\n");
						break;
					}
					if (travel_time(temp->date.month, temp->date.day, temp->all_day) == 2||(travel_time(temp->date.month, temp->date.day, temp->all_day)==3&&temp->level>=2)&& temp->level == leve && strcmp(temp->name, name_log) == 0)
					{
						initSocket();
						SOCKET fd = connectToHost(host, 80);
						if (fd != INVALID_SOCKET) printf("连接成功！%d\n", WSAGetLastError());
						else
						{
							printf("连接失败！错误代码%d\n", WSAGetLastError());
							system("pause");
							break;
						}

						//发送请求
						SetConsoleCP(UNICODE);
						char* url = generateUrl(temp->destination);
						puts(url);
						if (0 >= send(fd, url, strlen(url), 0)) printf("send error!,%d\n", WSAGetLastError());
						else
						{
							// 发送成功，服务器把请求的数据发送
							char buffer[BUFSIZ * 10] = { 0 };
							int total_received = 0;
							int remaining = sizeof(buffer);

							while (total_received < sizeof(buffer))  //循环接收数据
							{
								int recv_len = recv(fd, buffer + total_received, remaining, 0);
								if (recv_len == -1) {
									perror("recv error");
									break;
								}
								else if (recv_len == 0) {
									break;  // 对端关闭连接
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
					//解析文件
					FILE* fp = fopen("weather.json", "r+");
					if (!fp) {
						perror("无法打开文件\n");
						return 1;
					}
					fseek(fp, 0, SEEK_END);  // 定位到文件末尾
					long length = ftell(fp);  // 获取文件长度
					fseek(fp, 0, SEEK_SET);  // 定位到文件开头
					char* buff = malloc(length + 1);  // 分配缓冲区
					if (buff) {
						fread(buff, 1, length, fp);  // 读取文件内容到缓冲区
						buff[length] = '\0';  // 添加字符串结束符
					}
					//puts(buff);
					fclose(fp);  // 关闭文件

					// 解析JSON数据
					cJSON* json = cJSON_Parse(buff);  // 解析JSON字符串
					free(buff);  // 释放缓冲区
					if (!json) {
						const char* error_ptr = cJSON_GetErrorPtr();  // 获取错误信息
						if (error_ptr) {
							fprintf(stderr, "JSON解析错误: %s\n", error_ptr);
						}
						return 1;
					}

					// 提取`daily`数组
					cJSON* results = cJSON_GetObjectItemCaseSensitive(json, "results");  // 提取`results`数组
					if (cJSON_IsArray(results)) {
						cJSON* first_result = cJSON_GetArrayItem(results, 0);  // 提取`results`数组的第一个元素
						cJSON* daily = cJSON_GetObjectItemCaseSensitive(first_result, "daily");  // 提取`daily`数组
						if (cJSON_IsArray(daily)) {
							int number_of_forecasts = cJSON_GetArraySize(daily);  // 获取`daily`数组的长度
							DailyForecast* forecasts = malloc(sizeof(DailyForecast) * number_of_forecasts);  // 分配天气预报数组
							if (!forecasts) {
								perror("内存分配失败\n");
								cJSON_Delete(json);  // 释放cJSON对象
								return 1;
							}

							// 遍历`daily`数组，提取天气数据
							for (int i = 0; i < number_of_forecasts; i++) {
								cJSON* forecast_item = cJSON_GetArrayItem(daily, i);  // 提取`daily`数组的第i个元素
								cJSON* date_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "date");  // 提取`date`字段
								if (cJSON_IsString(date_item)) {
									strcpy(forecasts[i].date, date_item->valuestring);  // 复制日期字符串
								}

								cJSON* text_day_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "text_day");  // 提取`text_day`字段
								if (cJSON_IsString(text_day_item)) {
									strcpy(forecasts[i].text_day, text_day_item->valuestring);  // 复制白天天气描述
								}

								cJSON* high_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "high");  // 提取`high`字段
								if (cJSON_IsString(high_item)) {
									strcpy(forecasts[i].high, high_item->valuestring);  // 转换最高温度
								}

								cJSON* low_item = cJSON_GetObjectItemCaseSensitive(forecast_item, "low");  // 提取`low`字段
								if (cJSON_IsString(low_item)) {
									strcpy(forecasts[i].low, low_item->valuestring);  // 转换最低温度
								}
							}
							// 打印提取的天气数据
							printf("当地天气如下：\n");
							for (int i = 0; i < number_of_forecasts; i++) {

								printf("Date: %s, ", forecasts[i].date);
								SetConsoleOutputCP(CP_UTF8);
								printf("Weather: % s ", forecasts[i].text_day);
								SetConsoleOutputCP(original_cp);
								printf("Highest temperature : %s°C, lowest temperature : %s°C\n", forecasts[i].high, forecasts[i].low);
							}
							cJSON_Delete(json);  // 释放cJSON对象
							free(forecasts);  // 释放天气预报数组
							break;
						}
						else {
							fprintf(stderr, "未找到`daily`数组\n");
							cJSON_Delete(json);  // 释放cJSON对象
							return 1;
						}
					}
					else {
						fprintf(stderr, "未找到`results`数组\n");
						cJSON_Delete(json);  // 释放cJSON对象
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
			printf("请输入你要选择的项目>>  ");
			scanf_s("%d", &funchoice);
			switch (funchoice)
			{
			case 1: // 按照用户等级进行日排序和月排序
			{
				sort_by_level(dummy);
				system("cls");
				head = dummy->next;//更新头指针
				struct node* temp = head;
				print(temp);
				system("pause");
				system("cls");
				break;
			}
			case 2 :
				sort_by_bill(dummy);
				system("cls");
				head = dummy->next;//更新头指针
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
			printf("请输入你要选择的项目>>  ");
			scanf_s("%d", &funchoice);
			switch (funchoice)
			{
			case 1:  //以用户等级查询,金额从大到小
			{
				system("cls");
				int lev;
				sort_by_bill(dummy);
				head = dummy->next;//更新头指针
				struct node* temp = head;
				while (1)
				{
					system("cls");
					printf("请输入等级：\n");
					printf("[1]青铜  [2]白银  [3]黄金  [4]钻石\n");
					scanf_s("%d", &lev);
					if (lev >= 1 && lev <= 5) break;
					else printf("重新输入！\n");
				}
				system("cls");
				printf("等级\t姓名\t\t目的地\t\t天数\t金额(元)    订单时间(月-日)\n");
				search_by_level(lev, temp);
				system("pause");
			}
				break;
			case 2: // 以用户等级和日期查询，金额从大到小，word文档中依旧按照用户等级从高到低，这里用户等级已经确定了，无法排序
			{
				system("cls");
				int lev,month, day;
				sort_by_bill(dummy);
				head = dummy->next;//更新头指针
				struct node* temp = head;
				while (1)
				{
					system("cls");
					printf("请输入等级：\n");
					printf("[1]青铜  [2]白银  [3]黄金  [4]钻石\n");
					scanf_s("%d", &lev);
					if (lev >= 1 && lev <= 5) break;
					else printf("重新输入！\n");
				}
				while (1)
				{
					system("cls");
					printf("请输入月份：\n");
					scanf_s("%d",&month);
					if (month >= 1 &&month <= 12) break;
					else printf("重新输入！\n");
				}
				while (1)
				{
					system("cls");
					printf("请输入日：\n");
					scanf_s("%d", &day);
					if (day >= 1 && month <= 31) break;
					else printf("重新输入！\n");
				}
				system("cls");
				printf("等级\t姓名\t\t目的地\t\t天数\t金额(元)    订单时间(月-日)\n");
				search_by_level_and_date(lev, temp,month,day);
				system("pause");
			}
			break;
			case 3: // 按照姓名去查询
			{
				system("cls");
				char search_name[100] = "0";
				head = dummy->next;//更新头指针
				struct node* temp = head;
				printf("请输入用户姓名：\n");
				scanf("%s",search_name);
				system("cls");
				printf("等级\t姓名\t\t目的地\t\t天数\t金额(元)    订单时间(月-日)\n");
				search_by_name(search_name,temp);
				system("pause");
				system("cls");
				break;
			}
			case 4: //按照目的地去查询
			{
				system("cls");
				int position;
				sort_by_bill(dummy);
				head = dummy->next;
				struct node* temp = head;
				printf("----- 输入目的地：-----\n");
				printf("[0]广州  [1]理塘  [2]上海  [3]吉林\n");
				scanf("%d",&position);
				system("cls");
				printf("等级\t姓名\t\t目的地\t\t天数\t金额(元)    订单时间(月-日)\n");
				search_by_destination(temp, position);
				system("pause");
				system("cls");
			}
				break;
			case 5: //按照目的地和日期进行查询
			{
				system("cls");
				int position,month,day;
				sort_by_level(dummy);
				head = dummy->next;
				struct node* temp = head;
				while (1)
				{
					system("cls");
					printf("----- 输入目的地：-----\n");
					printf("[0]广州  [1]理塘  [2]上海  [3]吉林\n");
					scanf("%d", &position);
					if (position >= 1 && position <= 5) break;
					else printf("重新输入！\n");
				}
				while (1)
				{
					system("cls");
					printf("请输入月份：\n");
					scanf_s("%d", &month);
					if (month >= 1 && month <= 12) break;
					else printf("重新输入！\n");
				}
				while (1)
				{
					system("cls");
					printf("请输入日：\n");
					scanf_s("%d", &day);
					if (day >= 1 && month <= 31) break;
					else printf("重新输入！\n");
				}
				system("cls");
				printf("等级\t姓名\t\t目的地\t\t天数\t金额(元)    订单时间(月-日)\n");
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

struct node* loadfileinfor() // 加载信息
{
	FILE* file = fopen("G:\\travel_project\\Project2\\data.txt","r+");
	if (file == NULL) {
		perror("无法打开文件");
		exit(1);
	}
	struct node* current = (struct node*)malloc(sizeof(struct node)); // 建立哑点，空表
	dummy = current; 
	while (1)
	{
		struct node* temp = (struct node*)malloc(sizeof(struct node));
		current->next = temp;
		int n = fscanf(file, "%d\t%s\t%d\t%d\t%lf\t%d\t%d\n", &temp->level, temp->name, &temp->destination, &temp->all_day, &temp->bill, &temp->date.month, &temp->date.day);
		if (n == EOF) // 读取到文件结束符时返回-1，这个时候保存尾指针和清除内存
		{
			tail = current; //保留尾指针
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
struct node* inforinput() // 录入信息
{
	FILE* add = fopen("G:\\travel_project\\Project2\\data.txt", "a+");
	struct node* newnode = (struct node*)malloc(sizeof(struct node));
	tail->next = newnode;
	newnode->next = NULL;
	tail = newnode;
	printf("请输入等级：\n");
	printf("[1]青铜  [2]白银  [3]黄金  [4]钻石\n");
	scanf_s("%d",&newnode->level);
	fflush(stdin);
	system("cls");
	printf("请输入姓名：\n");
	scanf("%s", newnode->name);
	system("cls");
	fflush(stdin);
	printf("----- 输入目的地：-----\n");
	printf("[0]广州  [1]理塘  [2]上海  [3]吉林\n");
	scanf_s("%d", &newnode->destination);
	system("cls");
	fflush(stdin);
	printf("请输入旅游时间(天)：\n");
	scanf_s("%d", &newnode->all_day);
	system("cls");
	fflush(stdin);
	printf("请输入金额：\n");
	scanf("%lf", &newnode->bill);
	system("cls");
	fflush(stdin);
	printf("请输入订单月：\n");
	scanf_s("%d", &newnode->date.month);
	fflush(stdin);
	system("cls");
	printf("请输入订单日：\n");
	scanf_s("%d",  &newnode->date.day);
	system("cls");
	
	int n = fprintf(add, "%d\t%s\t%d\t%d\t%lf\t%d\t%d\n", newnode->level, newnode->name, newnode->destination, newnode->all_day, newnode->bill, newnode->date.month, newnode->date.day);
	if (n != EOF)
	{
		printf("录入成功！");
	}
	else printf("ERROR!"); // 错误警告
	fclose(add);
	system("pause");
	return tail;
};
void print(struct node* p)
{
	printf("等级\t姓名\t\t目的地\t\t天数\t金额(元)    订单时间(月-日)\n");
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
// 统计功能部分
void sort_by_level(struct node* front) //冒泡排序，节点交换
{
	struct node* prev ; //前驱指针
	struct node* p;  // 前一个指针
	struct node* q; //后一个指针
	struct node* end = NULL; // 终止指针
	while (front->next!=end)
	{
		prev = front; 
		p = front->next;
		q = p->next;
		while (p->next!=end) // 下一个是排完后的max/min指针就停止排序，重新从第一个开始排
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
	struct node* prev; //前驱指针
	struct node* p;  // 前一个指针
	struct node* q; //后一个指针
	struct node* end = NULL; // 终止指针
	while (front->next != end)
	{
		prev = front;
		p = front->next;
		q = p->next;
		while (p->next != end) // 下一个是排完后的max/min指针就停止排序，重新从第一个开始排
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

//查询功能部分
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
	printf("                  --------总共查询到%d个用户---------\n", n);
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
	printf("                  --------总共查询到%d个用户---------\n",n);
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
	printf("                  --------总共查询到%d个结果---------\n", n);
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
	printf("                  --------总共查询到%d个结果---------\n", n);
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
	printf("                  --------总共查询到%d个结果---------\n", n);
}



//菜单部分
void allmenu(struct menu menus[])
{
	//主菜单
	strcpy(menus[0].menu_s[0], "|--------  1.录入用户信息  --------|");
	strcpy(menus[0].menu_s[1], "|--------  2.统计用户信息  --------|");
	strcpy(menus[0].menu_s[2], "|--------  3.查询用户信息  --------|");
	strcpy(menus[0].menu_s[3], "|-------- 4.查询目的地时间 --------|");
	strcpy(menus[0].menu_s[4], "|-------- 5.查询目的地天气 --------|");
	strcpy(menus[0].menu_s[5], "|-------- 6.用户账单提醒   --------|");
	strcpy(menus[0].menu_s[6], "|--------     7.退出      ---------|");

	//统计子菜单
	strcpy(menus[1].menu_s[0], "|---       1.以用户等级统计     ---|");
	strcpy(menus[1].menu_s[1], "|---       2.以订单金额统计     ---|");
	strcpy(menus[1].menu_s[2], "|---       0.返回上一级         ---|");

	//查询子菜单
	strcpy(menus[2].menu_s[0], "|---      1.以用户等级查询       --|");
	strcpy(menus[2].menu_s[1], "|---      2.以日期和用户等级查询  -|");
	strcpy(menus[2].menu_s[2], "|---      3.以姓名查询           --|");
	strcpy(menus[2].menu_s[3], "|---      4.以目的地查询         --|");
	strcpy(menus[2].menu_s[4], "|---      5.以日期和目的地查询    -|");
	strcpy(menus[2].menu_s[5], "|---      0.返回上一级          ---|");
}
void menushow(int menuchoice,int funchocie)
{
	system("cls");
	gotoxy(40, 2);
	printf("*----------------------------------*\n");
	gotoxy(40, 3);
	printf("|-----------    菜单    -----------|\n");
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
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);// 获取标准输出设备句柄
	SetConsoleCursorPosition(hOut, pos);//两个参数分别是指定哪个窗体，具体位置
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

//目的地时间生成功能  以北京时间为本地时间 如果是采取utf+8形式的话，按照通俗来讲时差是可以忽略的
void print_time(int p)
{
	int arr[4] = { 112,100,120,126 }; // 广州，理塘，上海，吉林 的经度
	char des[4][12] = { "广州","理塘","上海","吉林" };
	while (1)
	{
		system("cls");
			time_t raw_time; //原始本地时间
			struct tm* local_tm, * utc_tm;
			time(&raw_time);
			local_tm = localtime(&raw_time);

			// 假设目标时区为UTC+n 小时 用经纬度去求
			double utc_offset = (arr[p]-116)/15 * 3600;  // 8小时转换为秒

			// 调整时区
			time_t target_time = raw_time + utc_offset;
			utc_tm = localtime(&target_time); //当地时间

			// 格式化输出
			char buffer[80];
			strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", utc_tm); 
			gotoxy(40, 6);
			printf("*-----      时间查询     -----*\n");
			gotoxy(40, 7);
			printf("|---     现在是%s时间    ---|\n", des[p]);
			gotoxy(40, 8);
			printf("|---  %s  ---|\n", buffer);
			gotoxy(40, 9);
			printf("*----- 按任意键返回上一级-----*\n");
		if (_kbhit()) {
			char key = getch(); //不输出到屏幕
			break;
		}
		Sleep(1);
	}
}

//旅游前后时间计算
int travel_time(int month,int day,int travel_day)
{
	time_t raw_time; //原始本地时间
	struct tm* local_tm,*utc_tm,*travel_tm;
	time(&raw_time);
	local_tm = localtime(&raw_time);
	double utc_offset = 24 * 3600;
	time_t target_time = raw_time - utc_offset;
	utc_tm = localtime(&target_time);
	


	if (month == local_tm->tm_mon && day==local_tm->tm_mday) // 当天
	{
		return 1;
	}
	if (month == utc_tm->tm_mon && day== utc_tm->tm_mday) // 前一天
	{
		return 2;
	}
	if (month+1.0*travel_day/30<=local_tm->tm_mon)
	{
		return 3;
	}
	return 0;
	
}

