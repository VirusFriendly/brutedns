/* BruteDNS by Chris Gragsone <maetrics@realwarp.net>
 *
 * compile: gcc -O2 -o brutedns brutedns.c
 * execute: brutedns [domain [wordfile]]
 */

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define DICT "/usr/share/dict/words"
#define NUM_BETWEEN_DISPLAYS 5000
#define printusage(x) fprintf(stderr, Usage, x)

int brutedns(FILE *, char *);
int checkhost(char *);
char *mkhostname(char *, char *);
int printhost(char *, struct in_addr **);

char *UnknownHost="The specified host is unknown.";
char *NoIP="The requested name is valid but does not have an IP address.";
char *UnRecovery="A non-recoverable name server error occurred.";
char *TryAgain="A temporary error occurred on an authoritative name server."
	"  Try again later.";
char *NoDomain="Using default domain from /etc/resolv.conf";
char *CopyRight="BruteDNS -- Brute forces domains when you can't axfr\n"
	"\tChris Gragsone <maetrics@users.sf.net>\n"
	"\tJeffery Strubenator <strube@users.sf.net>\n"
	"\t2003\n\n"; /* TODO: Need License */
char *Usage="Usage %s [domain] [file]\n"
        "\tdomain\t\tThe target domain\n"
        "\tfile\t\tThe word list [/usr/share/dict/words]\n\n";

int main(int argc, char **argv) {
	FILE *stream=NULL;
	char *domain=NULL;
	char *path=NULL;
	
	fprintf(stderr, "%s", CopyRight);
	/* TODO: Will need getopt() to handle options */

	if(argc == 3) {
		path=*(argv+2);
	} else {
		path=DICT;
	}

	if(argc >= 2) {
		domain=*(argv+1);
	} else {
		printusage(*argv);
		fprintf(stderr, "%s\n", NoDomain);
	}
	
	if(strcmp(path, "-") == 0) {
		stream=stdin;
	} else {
		stream=fopen(path, "r");
	}

	if(stream == NULL) {
		perror(path);
		return 0;
	}
	
	brutedns(stream, domain);
	return 1;
}

/* brutedns()
 * arguments:
 * 	stream: file descriptor for the file containing test hostnames
 * 	domain: string containing the target domain name
 * calls:
 * 	<stdio.h> fgets()
 * 	mkhostname()
 * returns:
 *	always returns 1 for now
 *	
 * notes:
 * 	grab a word from the word file
 * 	create a hostname using the word and the domain name
 * 	resolve the hostname
 * 	print the results
 * 	continue until there are no more words
 */
int brutedns(FILE *stream, char *domain) {
	int size=254-strlen(domain);
	char host[size];
	char *hostname=NULL;
	char *test=NULL;
	int wordnum=0;

	//First thing we do is resolve just the domain
	checkhost(domain);

	test = fgets(host, size, stream);

	while(test != NULL) {
		//Small hack to show some progress
		if(wordnum == NUM_BETWEEN_DISPLAYS) {
			printf("Current word :\t%s\n",test);
			wordnum=0;
		} else {
			wordnum++;
		}

		hostname=mkhostname(host, domain);
		checkhost(hostname);

		test = fgets(host, size, stream);
	}
	//TODO: Return number of hosts found
	return 1;
}

/* checkhost()
 * arguments:
 * 	hostname: string containing current host name to try
 * calls:
 *	<netdb.h> gethostbyname()
 *	printhost()
 * returns:
 *	always returns 1 for now
 *
 * notes:
 *	trys to resolve hostname
 *	if hostnotfound then it exits
 *	if the host resolves then calls printhost()
 */
int checkhost(char *hostname) {
	struct hostent *ent = NULL;

	ent = gethostbyname(hostname);
	if(ent != NULL) {
		printhost(hostname,(struct in_addr **)ent->h_addr_list);
		//TODO: add functionality for recursive queries
	} else if(h_errno != HOST_NOT_FOUND) {
		fprintf(stderr,"(%s) %s\n",hostname,UnknownHost);
	}
	return 1;
}

/* mkhostname()
 * arguments:
 * 	host: string containing current host name to try
 * 	domain: string containing target's domain name
 * calls:
 * 	<stdio.h> snprintf()
 * returns:
 * 	hostname: string containing a formated query name
 *
 * notes:
 * 	remove the return charector from host
 * 	check if domain is not null and concatonate host with domain with
 * 		seperating and ending periods
 * 	if domain doesnt exist copy host into hostname
 */
char *mkhostname(char *host, char *domain) {
	static char hostname[256];
	char *catfmt="%s.%s.";
	
	//TODO: should check if host has a return charector before removing it
	host[strlen(host)-1]='\0';
	
	if(domain != NULL) {
		(void) snprintf(hostname, 255, catfmt, host, domain);
	} else {
		snprintf(hostname, 255, "%s", host);
	}
	
	return hostname;
}

/* printhost()
 * arguments:
 * 	hostname: string containing the name which was queried
 * 	in: an array of pointers to resolved addresses
 * calls:
 * 	<stdio.h> printf()
 * 	<string.h> strlen()
 * 	<arpa/inet.h> inet_ntoa()
 * returns:
 * 	-1: if there was an error in writing to stdout
 * 	the number of resolved hosts (currently just returns 1
 *
 * notes:
 * 	Print the hostnamed queried and the first resolved address
 * 	if there are more addresses loop until there are no more
 * 	during each iteration, print the next resolved address
 * 	return the number of addresses resolved
 */
int printhost(char *hostname, struct in_addr **in) {
	char *foundfmt="%s resolves to %s\n";
	char *foundfmt2="\t- %s\n";
	char *addr=NULL;
	int x=0;
	int y=0;
	
	addr=inet_ntoa(**in);
	x=printf(foundfmt, hostname, addr);
	y=strlen(foundfmt)+strlen(hostname)+strlen(addr)-4;

	if(x > y) {
		printf("printed %d, strlen %d\n", x, y);
		return -1;
	}
	
	in++;
		
	while(*in != NULL) {
		addr=inet_ntoa(**in);
		x=printf(foundfmt2, addr);
		y=strlen(foundfmt2)+strlen(addr)-2;
		
		if(x > y) {
			printf("printed %d, strlen %d\n", x, y);
			return -1;
		}
		
		in++;
	}
	
	printf("\n");
	return 1;
}
