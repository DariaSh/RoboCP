#pragma once
#include "RenderBase.h"
#include "KinectData.h"
#include "KinectBuffer.h"
#include "XMLConfig.h"
#include <boost/asio.hpp>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
//#include "pcl/compression/octree_pointcloud_compression.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <time.h>
#include <stdio.h>
#pragma comment (lib, "ws2_32.lib")
//WSADATA ws;
using namespace std;
//
//#define GLOG_NO_ABBREVIATED_SEVERITIES
//#include <glog/logging.h>
//#include <glog/raw_logging.h>

using boost::asio::ip::tcp;
using namespace pcl;
using namespace pcl::octree;
using namespace std;

//Class purpose: taking KinectData object from buffer and sending it via TCP-IP
class OffScreenRender :
  public RenderBase
{
public:
  void Start();
  OffScreenRender(XMLConfig * x);
  ~OffScreenRender(void);
private:
	 
//TO DO: 1)add constants to config!!!2) create thread in main 3)

static const int  REQ_WINSOCK_VER   = 2;	
static const int  DEFAULT_PORT      = 8067;	
static const int  BUFF_SIZE=383631;
static const int  CHUNK_SIZE=383700;
static const char RANGE_DELIMITER='-';

//

static int gmtime_r(const time_t *timep, struct tm *result);
string GetDate();
string int_to_hex(int i);
int FindRange (char *buffer,const char* pattern);

string GetHostDescription(const sockaddr_in &sockAddr);
void SetServerSockAddr(sockaddr_in *pSockAddr, int portNumber);

int GET(SOCKET s, char* buf, int length);
int SEND(SOCKET s, int startRange);

void HandleConnection(SOCKET hClientSocket, const sockaddr_in &sockAddr);
bool RunServer(int portNumber);
};