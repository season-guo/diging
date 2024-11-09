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
	char queryarticle[512];
	short *header;
	short *ask;
	query(char *domain)
	{
		for (int i = 0; i < 12; i++)
			queryarticle[i] = 0x00;
		queryarticle[0] = 0x01;
		queryarticle[2] = 0x01;
		queryarticle[5] = 0x01;
		int tmp = 12;
		int ac = 0;
		for (int i = 13; domain[i - 13] != '\0'; i++)
		{
			if (domain[i - 13] == '.')
			{
				queryarticle[tmp] = ac;
				tmp = i;
				ac = 0;
			}
			else
				ac++;
		}
	}
}

creatquery(char *domain, uint16_t qtype, uint8_t *query, size_t &querylen)
{
	// header first
	query[0] = 0;
	query[1] = 0;
	query[2] = 0x01;
	query[3] = 0x00;
	query[4] = 0x00;
	query[5] = 0x00;
	query[6] = 0x00;
	query[7] = 0x00;
	query[8] = 0x00;
	query[9] = 0x00;
	query[10] = 0x00;
	query[11] = 0x01;
	size_t len = 12;
	size_t lastlen = 12;
	size_t splitlen = 0;
	for (char *i = domain;; i++)
	{
		len++;
		if (*i == '.')
			query[lastlen] = splitlen, splitlen = 0, lastlen = len;
		else if (*i == '\0')
		{
			query[lastlen] = splitlen;
			query[len] = 0;
			break;
		}
		else
			splitlen += 1, query[len] = *i;
	};
	cout << len << endl;
	query[++len] = 0;
	query[++len] = 1;
	query[++len] = 0;
	query[++len] = 1;
	querylen = len;
}
int main(int argc, char *argv[])
{
	uint8_t qtype;
	char *domain;
	if (argv[0][0] = '@')
	{
		char *dnsse = argv[0] + 1;
		domain = argv[1];
		if (argc == 3)
		{
			if (argv[2][0] == 'A')
				qtype = A;
			else
				qtype = NS;
		}
	}
	else
	{
		domain = argv[0];
		if (argc == 2)
		{
			if (argv[1][0] == 'A')
				qtype = A;
			else
				qtype = NS;
		}
	}
	size_t querylen;
	uint8_t query[512];
	creatquery(domain, qtype, query, querylen);
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		cerr << "sock fail" << endl;
		return 1;
	};
	struct sockaddr_in seaddr;
	memset(&seaddr, 0, sizeof seaddr);
	seaddr.sin_family = AF_INET;
	seaddr.sin_port = htons(53);
	inet_pton(AF_INET, "8.8.8.8", &seaddr.sin_addr);
	ssize_t sent = sendto(sockfd, query, querylen, 0, (struct sockaddr *)&seaddr, sizeof(seaddr));
	if (sent < 0)
	{
		cerr << "send fail" << endl;
		return 1;
	}
	cout << "DNS query!" << endl;
	uint8_t response[512];
	socklen_t addrlen = sizeof(seaddr);
	ssize_t receive = recvfrom(sockfd, response, sizeof(response), 0, (struct sockaddr *)&seaddr, &addrlen);
	if (receive < 0)
	{
		cerr << "receive fail" << endl;
		return 1;
	}
	cout << "DNS receive!" << endl;
	size_t start = 12;
	while (response[start] != 0)
		start++;
	start += 5;
	for (int i = 0; i < 1; i++)
	{
		while (response[start] != 0)
			start++;
		start += 11;
		for (int j = 0; j < 4; ++j)
		{
			cout << dec << (int)response[start + j] << ".";
		}
	}
	close(sockfd);
	return 0;
}
