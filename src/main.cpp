#include<gflags/gflags.h>
#include<glog/logging.h>
#include<libconfig++.h>
#include<event.h>
#include "adpter.h"


using namespace std;

extern map<string,class proto_extend *> map_proto;

struct config {
    map<string,string> proto_plugin;
    string server_ip;
    uint16_t port;
} ;

struct config g_conf;

DEFINE_bool(server,false,"running mode");                                                     
DEFINE_string(conf,"proto_extend.conf","tool conf file");
DEFINE_string(casefile,"","tool conf file");

void init_proto_plugin(map<string,string> &proto_map_list) 
{
//从map中挨个初始化协议插件,在这里new也行
}

void parse_config(struct config *conf)
{
//libconfig解析FLAGS_conf
}

void resolve_proto() 
{

}

void server_callback(int fd, short event,void *arg)
{

}

int main(int argc , char *argv[])
{
    google:ParseCommandLineFlags(&argc, argv, true);
    google:InitGoogleLogging(argv[0]);
    parse_config(&g_conf);
    init_proto_plugin(g_conf->proto_plugin);

    //从网络获取用例
    if(FLAGS_server) {
	daemon(1,1);
	int fd = CreateUdpServer(g_conf->server_ip,g_conf->port);
	struct event event;
	event_init();
	event_set(&event, fd, EV_READ|EV_PERSIST, server_callback,NULL);
	event_add(&event, NULL); 
	event_dispatch();
	return 0;
    }
    //从文件获取用例
    if(!FLAGS_casefile.empty()) { 
	//用string的东西来处理
    }
    //从终端获取用例

    return 0;
}
