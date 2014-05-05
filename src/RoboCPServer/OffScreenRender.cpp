#include "OffScreenRender.h";

/*class exception
{

public:
    exception() :
         m_pMessage("") {}
    virtual ~exception() {}
    exception(const char *pMessage) :
         m_pMessage(pMessage) {}
    const char * what() { return m_pMessage; }
private:
    const char *m_pMessage;
};
*/
OffScreenRender::OffScreenRender(XMLConfig * x)
{}
OffScreenRender::~OffScreenRender()
{}
void OffScreenRender:: Start()
{
	WSADATA wsaData;
	//printf( "%s\n",argv[0]);
	//printf("Initializing winsock... ");

	if (WSAStartup(MAKEWORD(REQ_WINSOCK_VER,0), &wsaData)==0)
	{

		if (LOBYTE(wsaData.wVersion) >= REQ_WINSOCK_VER)
		{
			printf("initialized.\n");
			int port = DEFAULT_PORT;
			RunServer(port);
		}
		else
		{
			cerr << "required version not supported!";
		}

		printf("Cleaning up winsock... ");


		if (WSACleanup()!=0)
		{
			cerr << "cleanup failed!\n";
		}   
		printf("done.\n");
	}
	else
	{
		cerr << "startup failed!\n";
	}
	
}

static char const month[12][4] = {
  "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

static char const day[7][4] = {
  "Sun","Mon","Tue","Wed","Thu","Fri","Sat"
};
string OffScreenRender::GetHostDescription(const sockaddr_in &sockAddr)
{
	ostringstream stream;
	stream << inet_ntoa(sockAddr.sin_addr) << ":" << ntohs(sockAddr.sin_port);
	return stream.str();
}

void OffScreenRender::SetServerSockAddr(sockaddr_in *pSockAddr, int portNumber)
{
	pSockAddr->sin_family = AF_INET;
	pSockAddr->sin_port = htons(portNumber);
	pSockAddr->sin_addr.S_un.S_addr = INADDR_ANY;
}

int OffScreenRender::FindRange(char *buffer,const char* pattern)
{
	char * ptrStartRange = nullptr;
	char  strtempOfStartRange[20];
	ptrStartRange = strstr(buffer,pattern);
	if(ptrStartRange == nullptr){ return -1;}
	ptrStartRange += strlen(pattern);
	int strcnt = 0;
		while(*ptrStartRange!=RANGE_DELIMITER){
			strtempOfStartRange[strcnt] = *ptrStartRange; 
			strcnt++;
			ptrStartRange++;
			if(*ptrStartRange=='\0') break;
		} 
		strtempOfStartRange[strcnt] = '\0';
		return atoi(strtempOfStartRange);
}
int  OffScreenRender::GET(SOCKET s, char* buf, int length)
{
	char tempbuf[1024];
	int count=0;
	int recieved=0;
    count=recv(s, tempbuf, length, 0);
    memcpy(buf,tempbuf,sizeof(tempbuf));
    buf[count]='\0';
	return count;
}

string  OffScreenRender::int_to_hex(int i)
{
  std::stringstream stream;
  stream << std::hex << i;
  std::string result( stream.str() );
  return stream.str();
}
 int OffScreenRender::gmtime_r(const time_t *timep, struct tm *result) {
  return gmtime_s(result, timep) == 0;
}

string OffScreenRender:: GetDate() {
  time_t rawtime;
  time(&rawtime);
  struct tm t;
  if (!gmtime_r(&rawtime, &t)) {
    return "Date: Thu Jan 01 1970 00:00:00 GMT";
  }

  char buf[128];
  unsigned len = sprintf_s(buf, 
    "Date: %s, %d %s %d %.2d:%.2d:%.2d GMT",
    day[t.tm_wday], t.tm_mday, month[t.tm_mon],
    1900 + t.tm_year, t.tm_hour, t.tm_min, t.tm_sec);

  return string(buf, len);
}

int OffScreenRender::SEND(SOCKET s, int startRange)
{
    ifstream file("C:\\Users\\Darenqa\\Documents\\Visual Studio 2010\\Projects\\Server\\small.ogv", ios::in | ios::binary);

	string response; 
	response.append("HTTP/1.1 200 OK\r\n");
	response.append("Content-Type: application/octet-stream\r\n");
	response.append(GetDate());
	response.append("\r\n");
	 response.append("\r\n");
	int	fileCnt = send(s,response.c_str(),response.size(),0);
	int seekAmount = 0;
	while(file.good()){
        string ss;
	    ss.resize(200);
	    file.seekg(seekAmount);
	    seekAmount += 200;
	    file.read(&ss[0],200);
	    send(s,ss.c_str(),ss.size(),0);

	    response.append("\r\n");
	    response.append(ss);
	    response.append("\r\n");
	    Sleep(1);
	}
	file.close();

	return fileCnt;
}

void OffScreenRender::HandleConnection(SOCKET hClientSocket, const sockaddr_in &sockAddr)
{

	printf("Connected with %s .\n", GetHostDescription(sockAddr));

	char StreamBuffer[BUFF_SIZE];
	int sizeStrBuf;
	int fileCnt;
    int startRange;
	int endRange;


	while(true)
	{
		fileCnt=GET(hClientSocket, StreamBuffer,BUFF_SIZE);

		if (fileCnt ==0)
		{ 
			break; 
		}
		else if (fileCnt ==SOCKET_ERROR)
		{
			//throw exception("socket error while receiving.");
		}
		else
		{

		printf("Request recieved as \n%s\n",StreamBuffer); 
		int startRange = FindRange(StreamBuffer,"bytes=");
		printf("startRange = %i\n",startRange);
		int sizeOfSendData = SEND(hClientSocket, startRange);
		printf("Data sent to the client %d bytes\n",sizeOfSendData);
		}
	}
	printf("Connection closed.\n");
}

bool OffScreenRender::RunServer(int portNumber)
{
	SOCKET 		hSocket = INVALID_SOCKET,
				hClientSocket = INVALID_SOCKET;
	bool		bSuccess = true;
	sockaddr_in	sockAddr = {0};

	try
	{

		printf("Creating socket... ");
		if ((hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
			throw exception("could not create socket.");
		printf("created.\n");


		printf("Binding socket... ");
		SetServerSockAddr(&sockAddr, portNumber);
		if (::bind(hSocket, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr))!=0)
			throw exception("could not bind socket.");
		printf("bound.\n");


		printf("Putting socket in listening mode... ");
		if (listen(hSocket, SOMAXCONN)!=0)
			throw exception("could not put socket in listening mode.");
		printf("done.\n");

		printf("Waiting for incoming connection... ");

		sockaddr_in clientSockAddr;
		int			clientSockSize = sizeof(clientSockAddr);


		hClientSocket = accept(hSocket,
						 reinterpret_cast<sockaddr*>(&clientSockAddr),
						 &clientSockSize);


		if (hClientSocket==INVALID_SOCKET)
			throw exception("accept function failed.");
		printf("accepted.\n");

		HandleConnection(hClientSocket, clientSockAddr);
	}
	catch(exception e)
	{
		cerr << "\nError: " << e.what() << endl;
		bSuccess = false; 
	}

	if (hSocket!=INVALID_SOCKET)
		closesocket(hSocket);

	if (hClientSocket!=INVALID_SOCKET)
		closesocket(hClientSocket);

	return bSuccess;
}	
