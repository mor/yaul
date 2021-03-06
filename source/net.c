#include "net.h"
#include "wpad.h"

const struct block emptyblock = {0, NULL};

//The maximum amount of bytes to send per net_write() call
#define NET_BUFFER_SIZE 1024
#define HTTP_BUFFER_SIZE 1024 * 5
#define MAX_IP_CACHE_ENTRIES 5

struct IPCache {
    char * host;
    u32 ip;
    struct IPCache *nextnode;
} ;

static struct IPCache *firstIPCache = NULL;
static int IPCachecount = 0;

u32 Net_GetIP(char * host) {
    //Search if this hostname is already cached
    struct IPCache *node = firstIPCache;
    struct IPCache *previousnode = NULL;

    while (node != NULL) {
        if (strcmp(node->host, host) == 0) {
            //DNS node found in the cache, move it to the front of the list
            if (previousnode != NULL)
                previousnode->nextnode = node->nextnode;

            if (node != firstIPCache)
                node->nextnode = firstIPCache;
            firstIPCache = node;

            return node->ip;
        }
        //Go to the next element in the list
        previousnode = node;
        node = node->nextnode;
    }

    u32 ip = Net_Getipbyname(host);
    
    //No cache of this domain could be found, create a cache node and add it to the front of the cache
    struct IPCache *newnode = malloc(sizeof(struct IPCache));
    if (newnode == NULL) {
        return ip;
    }

    newnode->ip = ip;
    newnode->host = malloc(strlen(host)+1);
    if (newnode->host == NULL) {
        free(newnode);
        return ip;
    }
    strcpy(newnode->host, host);

    newnode->nextnode = firstIPCache;
    firstIPCache = newnode;
    IPCachecount++;

    //If the cache grows too big delete the last (and probably least important) node of the list
    if (IPCachecount > MAX_IP_CACHE_ENTRIES) {
        struct IPCache *node = firstIPCache;
        struct IPCache *previousnode = NULL;

        //Fetch the last two elements of the list
        while (node->nextnode != NULL) {
            previousnode = node;
            node = node->nextnode;
        }

        if (node == NULL) {
            printf("Configuration error, MAX_DNS_ENTRIES reached while the list is empty\n");
            exit(1);
        } else if (previousnode == NULL) {
            firstIPCache = NULL;
        } else {
            previousnode->nextnode = NULL;
        }

        free(node->host);
        free(node);
        IPCachecount--;
    }

    return newnode->ip;
}

static s32 Net_Send(s32 server, char *msg) {

	s32 bytes_transferred = 0;
	s32 remaining = strlen(msg);

	while (remaining) {
		if ((bytes_transferred = net_write(server, msg, remaining > NET_BUFFER_SIZE ? NET_BUFFER_SIZE : remaining)) > 0) {
			//printf("bytes_transferred: %d\n", bytes_transferred);
			remaining -= bytes_transferred;
			usleep (20 * 1000);
		} else if (bytes_transferred < 0) {
			//printf("bytes_transferred: %d\n", bytes_transferred);
			return bytes_transferred;
		} else {
			//printf("ERROR NO DATA\n");
			return -ENODATA;
		}
	}
	
	return 0;
}

void Net_Init(void) {
	s32 result = -1;
	while (result < 0) {
		while ((result = net_init()) == -EAGAIN);
		if (result < 0)
			printf("net_init() failed: [%i] %s, retrying...\n", result, strerror(-result));
	}
	if (result >= 0) {
		u32 ip = 0;
		do {
    			ip = net_gethostip();
    			if (!ip)
    				printf("    net_gethostip() failed, retrying...\n");
		} while (!ip);
		if (ip) {
			//printf("    Network initialized.\n    IP address: %s\n", inet_ntoa(*(struct in_addr *)&ip));
			//Network_Running = true;
		}
	}
}

static s32 Net_Connect(u32 ip, u32 port) {

	/* Create a socket */
	s32 connection = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	
	if (connection < 0) {
		//printf("net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP) FAIL\n");
		return connection;
	}
	
	/* Config socket */
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = ip;

	/* Make connection, check for failure */
	if (net_connect(connection, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		//printf("net_connect(connection, (struct sockaddr*)&addr, sizeof(addr) FAILED\n");
		net_close(connection);
		return -1;
	}

	/* Connected! */
	return connection;
}


struct block Net_Read(s32 connection)
{
	/* Simple struct has size and data elements */
	struct block read_buf;

	read_buf.size = HTTP_BUFFER_SIZE;
	read_buf.data = malloc(HTTP_BUFFER_SIZE);

	if(read_buf.data == NULL) {
		//printf("read_buf.data FAILED malloc(HTTP_BUFFER_SIZE)\n");
		return emptyblock;
	}

	/* Keep track of buffer location */
	u32 offset = 0;

	while(1) {
		//printf("            connection: %d\n", connection);
		//printf("read_buf.data + offset: %d\n", offset);
		//printf("read_buf.size - offset: %d\n", read_buf.size - offset);
		//printf("Press any button...\n");
		//Wpad_WaitButtons();
		
		/* read_buf.size - offset -> keeps from overflowing buffer */
		s32 bytes_read = net_read(connection, read_buf.data + offset, read_buf.size - offset);

		if(bytes_read < 0) {
			/* Error */
			//printf("net_read()  Errorcode: %i\n", bytes_read);
			return emptyblock;
		}

		/* Finished */
		if(bytes_read == 0) {
			break;
		}

		/* Update buffer loaction */
		offset += bytes_read;

		/* Buffer full? */
		if(offset >= read_buf.size) {
			read_buf.size += HTTP_BUFFER_SIZE;
			read_buf.data = realloc(read_buf.data, read_buf.size);

			/* Malloc error, out of memory */
			if(read_buf.data == NULL)
			{
				//printf("read_buf.data FAILED realloc(HTTP_BUFFER_SIZE)\n");
				return emptyblock;
			}
		}
	}

	/* Exact byte count */
	read_buf.size = offset;

	/* Re-size malloc */
	realloc(read_buf.data, read_buf.size);

	/* Finished! */
	return read_buf;
}

u32 Net_Getipbyname(char * host)
{
	//u32 my_ip = 0;
	//my_ip = net_gethostip();
	//printf("my ip: %s", inet_ntoa(*(struct in_addr *)&my_ip));
	
	struct hostent * hostdata = net_gethostbyname(host);

	if(hostdata == NULL) {
		//printf("*hostdata = net_gethostbyname(host) FAIL\n");
		return 0;
	}

	u32 *ip = (u32*)hostdata->h_addr_list[0];
	return *ip;
}

/* Download a file via HTTP */
struct block Net_GetFile(char * host, char * path)
{
	Net_Init();
	//printf("host: %s", host);
	u32 ip = Net_Getipbyname(host);

	//printf("ip: %s", inet_ntoa(*(struct in_addr *)&ip));
	s32 connection = Net_Connect(ip, 80);

	if(connection < 0) {
		//printf("NetConnect() Error, returned %d\n", connection);
		return emptyblock;
	}
	
	/* Create HTTP request */
	char* headerformat = "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n";
	char header[strlen(headerformat) + strlen(path) + strlen(host)];
	sprintf(header, headerformat, path, host);
	
	//printf("HTTP REQUEST:\n%s\n", header);
	//printf("Press any button...\n");
	//Wpad_WaitButtons();
	
	/* Ask the server for our file */
	Net_Send(connection, header);

	//printf("Net_Send(): %d\n", sent);
	//printf("Press any button...\n");
	//Wpad_WaitButtons();
	
	/* Get the response */
	struct block response = Net_Read(connection);
	net_close(connection);

	//printf("RECV: %d bytes\n\n", response.size);
	
	if (response.size < 15) {
		return emptyblock;
	}

	/* Quick check status line to ensure it was a good download */			
	bool status_200 = true;
	char status_line[] = "HTTP/1.1 200 OK";
	int i;	
	for (i = 8 ; i < 15 ; i++) {
		if (response.data[i] != status_line[i]) {
			status_200 = false;
			break;
		}
	}
	
	/* It was bad, bomb out */
	if (!status_200)
		return emptyblock;
		
	/* Skip past the HTTP header stuff to the actual file content */
	unsigned char *filestart = NULL;
	u32 filesize = 0;
	for(i = 3; i < response.size; i++)
	{
		if(response.data[i] == '\n' &&
			response.data[i-1] == '\r' &&
			response.data[i-2] == '\n' &&
			response.data[i-3] == '\r')
		{
			filestart = response.data + i + 1;
			filesize = response.size - i - 1;
			break;
		}
	}

	/* No file content? */
	if(filestart == NULL)
	{
		free(response.data);
		return emptyblock;
	}

	/* Create simple struct to hold file content */
	struct block file;
	file.data = malloc(filesize);
	file.size = filesize;

	if(file.data == NULL)
	{
		/* Malloc error, out of memory */
		free(response.data);
		return emptyblock;
	}

	/* Move the data */
	memcpy(file.data, filestart, filesize);

	/* Dispose of download buffer */
	free(response.data);
	
	return file;
}
