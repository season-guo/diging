#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
using namespace std;
enum
{
	A = 1,
	NS = 2,
	PTR = 12,
};
enum
{
	IN = 1,
};
class query
{
public:
	unsigned char request[4096];
	unsigned char receive[4096];
	int domainlen = 0;
	size_t len;
	char root[11] = "198.41.0.4";
	query(unsigned char *domain)
	{
		for (int i = 0; i < 12; i++)
			request[i] = 0x00;
		request[0] = 0x01;
		request[5] = 0x01;
		int tmp = 12;
		int ac = 0;
		for (int i = 13;; i++)
		{
			domainlen++;
			if (domain[i - 13] == '\0')
			{
				request[tmp] = ac;
				request[i] = 0x00;
				len = i;
				break;
			}
			if (domain[i - 13] == '.')
			{
				request[tmp] = ac;
				tmp = i;
				ac = 0;
			}
			else
			{
				ac++;
				request[i] = domain[i - 13];
			}
		}
		request[++len] = 0x00;
		request[++len] = 0x01;
		request[++len] = 0x00;
		request[++len] = 0x01;
		++len;
		++domainlen;
	};
	int defaultdig(int istld, int showall, int isA)
	{
		if (showall)
			cout << "the used root server ip is:198.41.0.4" << endl;
		int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		struct sockaddr_in server_addr, client_addr;
		socklen_t clen = sizeof(client_addr);
		memset(&client_addr, 0, sizeof(client_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(53);
		server_addr.sin_addr.s_addr = inet_addr(root);
		inet_pton(AF_INET, root, &server_addr.sin_addr);
		sendto(sockfd, request, len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
		recvfrom(sockfd, receive, 4096, 0, (struct sockaddr *)&client_addr, &clen);
		int answer = receive[7] + receive[6] * 256;
		int auth = receive[8] * 256 + receive[9];
		int recordlen, start = 15 + domainlen;
		for (int j = 0; j < auth; j++)
		{
			recordlen = receive[start + 12];
			start += 12 + recordlen;
		}
		start += 13;
		char tldip[16];
		int now = 0;
		for (int i = 0; i < 4; i++)
		{
			int sup = receive[start + i];
			int place = 1;
			while (sup > 10 * place)
			{
				place *= 10;
			}
			while (place >= 1)
			{
				tldip[now] = '0' + sup / place;
				now++;
				sup = sup - (sup / place) * place;
				place /= 10;
			}
			tldip[now] = '.';
			now++;
		}
		tldip[now - 1] = '\0';
		if (istld)
		{
			if (isA)
				cout << "the ip you want is:" << tldip << endl;
			else
			{
				printf("the ns of the tld you want is:");
				for (int s = domainlen + 29; s < receive[domainlen + 27] + domainlen + 28; s++)
					if ((receive[s] >= 'a' && receive[s] <= 'z') || (receive[s] == '-'))
						printf("%c", receive[s]);
					else
						printf(".");
			}
			return 1;
		}
		if (showall)
			cout << "the used tld server ip is:" << tldip << endl;
		inet_pton(AF_INET, tldip, &server_addr.sin_addr);
		sendto(sockfd, request, len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
		recvfrom(sockfd, receive, 4096, 0, (struct sockaddr *)&client_addr, &clen);
		recordlen = 0;
		answer = receive[7] + receive[6] * 256;
		auth = receive[8] * 256 + receive[9];
		if (answer != 0)
		{
			start = 25 + 2 * domainlen;
			printf("%d.%d.%d.%d", receive[start + 1], receive[start + 2], receive[start + 3], receive[start + 4]);
		}
		else
		{
			start = 15 + domainlen;
			for (int j = 0; j < auth; j++)
			{
				recordlen = receive[start + 12];
				start += 12 + recordlen;
			}
			start += 13;
			printf("the ip you want is:");
			printf("%d.%d.%d.%d", receive[start], receive[start + 1], receive[start + 2], receive[start + 3]);
		}
		return 1;
	};
	int directdig(char *directdns)
	{
		int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		struct sockaddr_in server_addr, client_addr;
		socklen_t clen = sizeof(client_addr);
		memset(&client_addr, 0, sizeof(client_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(53);
		server_addr.sin_addr.s_addr = inet_addr(directdns);
		inet_pton(AF_INET, directdns, &server_addr.sin_addr);
		sendto(sockfd, request, len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
		recvfrom(sockfd, receive, 4096, 0, (struct sockaddr *)&client_addr, &clen);
		int answer = receive[7] + receive[6] * 256;
		int auth = receive[8] * 256 + receive[9];
		int recordlen, start = 15 + domainlen;
		if (answer != 0)
		{
			if (answer == 1)
				start = 25 + 2 * domainlen;
			else
				start = 27 + domainlen;
			printf("%d.%d.%d.%d", receive[start + 1], receive[start + 2], receive[start + 3], receive[start + 4]);
		}
		else
		{
			start = 15 + domainlen;
			for (int j = 0; j < auth; j++)
			{
				recordlen = receive[start + 12];
				start += 12 + recordlen;
			}
			start += 13;
			printf("%d.%d.%d.%d", receive[start], receive[start + 1], receive[start + 2], receive[start + 3]);
		}
		return 1;
	}
};
int main(int argc, char *argv[])
{
	int istld = 1;
	int direct = 0;
	int showall = 0;
	int isA = 1;
	char *tmpdomain;
	char *directdns;
	for (int i = 1; i < argc; i++)
	{
		if ((argv[i][0]) == '@')
		{
			direct = 1;
			directdns = argv[i] + 1;
		}
		if (argv[i][0] == '+')
			showall = 1;
		if (argv[i][0] == 'N')
			isA = 0;
	}
	if (direct)
		tmpdomain = argv[2];
	else
		tmpdomain = argv[1];
	int domainlen = 2;
	int i;
	unsigned char domain[100];
	for (i = 0; tmpdomain[i] != '\0'; i++)
	{
		domain[i] = (unsigned char)tmpdomain[i];
		if (domain[i] == '.')
		{
			istld -= 1;
		}
	}
	domain[i] = '\0';
	query a = query(domain);
	if (showall || istld || (!direct))
		a.defaultdig(istld, showall, isA);
	else
		a.directdig(directdns);
	return 0;
}