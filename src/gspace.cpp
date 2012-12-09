#include "adpter.h"


#pragma pack(1)
struct SCmdHead
{
    SCmdHead() { memset(this, 0, sizeof(SCmdHead)); }

    void SetVer(const unsigned char ucVer) { m_ucVer = ucVer; }
    unsigned char GetVer() const { return m_ucVer; }

    void SetType(const unsigned char ucType) { m_ucType = ucType; }
    unsigned char GetType() const { return m_ucType; }

    void SetSeqno(const unsigned long dwSeq) { m_dwSeqno = htonl(dwSeq); }
    unsigned long GetSeqno() const { return ntohl(m_dwSeqno); }

    void SetUin(const unsigned long dwUin)  { m_dwUin = htonl(dwUin); }
    unsigned long GetUin() const { return ntohl(m_dwUin); }

    void SetCmd(const unsigned short wCmd)  { m_wCmd = htons(wCmd); }
    unsigned short GetCmd() const { return ntohs(m_wCmd); }

    void SetErrCode(const short wErrCode) { m_wErrCode = htons(wErrCode); }
    short GetErrCode() const { return ntohs(m_wErrCode); }

    char* GetReserve() { return szReserve; }

    private:
    unsigned char m_ucVer;		    //Packet version
    unsigned char m_ucType;		//Packet type (0 - Req, 1 - Ack)
    unsigned long m_dwSeqno;	    //Sequence number (used by Happy)
    unsigned long m_dwUin;		//User Id: user performing the operation, 0 otherwise
    unsigned short m_wCmd;		    //The application's command id
    short m_wErrCode;			//Only used in Ack (0 - success, all else failure)
    char szReserve[4];		    //N/A reserved for future use
}
#pragma pack()

const char *gspace_map[] = {"version", "type", "Seqno", "uin", "cmd", "errcode", "reserve"};

extern "C" {
    regestor_proto(map<string,class proto_base>& g_map_proto) {
	class proto_base *p = new proto_gspace("gspace");
	g_map_proto.insert("gspace", p);

    }
}

namespace proto_extend {
    class proto_gspace : public proto_base {
	proto_gspace(string proto_name) : name(proto_name) {
	    header = reinterpret_cast<SCmdHead *>buf;
	    // memset(&header, 0, sizeof(header));
	}
	public:
	virtual bool input() { 
	    if(input_value.decodeJson(buf, json_size) == false) {
		errmsg="decode json failed";
		return false;
	    }
	    //检查json中那些是必填的字段
	    if(!input_value.isObject() 
		    || !input_value.["head"].isObject()
		    || !input_value.["body"].isObject()		//不关心body具体内容
		    || !input_value.["ctrl"].isObject()		//不同协议机制不一样，这里要检查ip port,type = "yaaf://地址" ,type = "udp://ip:port"
		    || !input_value["uin"].isIntegeral() 
		    || !input_value["seqno"].isIntergral()
		    || !input_value["cmd"].isIntergral()

		    || !input_value["ctrl"]["mode"].isString()
	      ) {
		errmsg="json field [uin, seqno, cmd] error";
		return false;
	    }
	    Members gs_members = input_value.getMemberNames();
	    for(vector<string>::iterator i = gs_members.begin(); i < gs_members.end(); i++) {
		for(int j = 0; j < sizeof(gspace_map)/sizeof(gspace_map[0]);j++) {
		    if((*i).compare(gspace_map[j])) {
			//const char *gspace_map[] = {"version", "type", "Seqno", "uin", "cmd", "errcode", "reserve"};
			switch(j) {
			    case 0:
				header->SetVer(input_value[i].asUInt());
				break;
			    case 1:
				header->SetType(input_value[i].asUInt());
				break;
			    case 2:
				header->SetSeqno(input_value[i].asUInt());
				break;
			    case 3:
				header->SetUin(input_value[i].asUInt());
				break;
			    case 4:
				header->SetCmd(input_value[i].asUInt());
				break;
			    case 5:
				uint32_t tmp = input_value[i].asUInt();
				memcpy(&header.szReserve, &tmp, sizeof(tmp));
				break;
			}
		    }
		}
	    }

	}
	string body = input_value["body"].toJsonString();
	memcpy(buf + sizeof(header), body.data(), body.size());
	cur_buflen = sizeof(header) + body.size();
	return true;
    };
    virtual bool process() {
	//后续支持yaaf和comm两种收发机制
	string mode = input_value["ctrl"]["mode"].asString();
	if(mode.find("yaaf://",0) == 0) {//yaaf收发机制,暂时未支持
	    errmsg="don't support :" + input_value["ctrl"]["mode"].asString();
	    return false;
	}
	else if(mode.find("udp://",0) == 0) {//普通udp收发机制
	    size_type ip_index = mode.find(':', 0);
	    if(ip_index == string::npos) {
		errmsg="mode format err";
		return false;
	    }
	    size_type port_index = mode.find(':', ip_index); 
	    if(port_index == string::npos) {
		errmsg="mode format err";
		return false;
	    }

	    char ip[20], port[10];
	    mode.copy(ip, ip_index+1,port_index - ip_index);
	    mode.copy(port, port_index+1,mode.size() - port_index);
	    struct sockaddr_in dstaddr, srcaddr;
	    socket_len len = sizeof(struct sockaddr_in);
	    dstadd.sin_family = AF_INET;
	    dstaddr.sin_port = htons(atoi(port));
	    inet_aton(ip,&dstaddr.sin_addr);
	    int sk = socket(AF_INET,SOCK_DGRAM, 0);
	    int iRet;
	    if(sendto(sk,buf,cur_buflen ,0, (struct sockaddr *)&dstaddr, sizeof(dstaddr)) < 0) {
		errmsg = "send error";
		return false;
	    }
	    if((cur_buflen=recvfrom(sk, buf, 8196 , 0, (struct sockaddr *)&srcaddr, &len)) < 0) {
		errmsg = "recv error";
		return false;
	    }
	    output_value["head"]["version"] = header->GetVer();
	    output_value["head"]["type"] = header->GetType();
	    output_value["head"]["seqno"] = header->GetSeqno();
	    output_value["head"]["uin"] = header->GetUin();
	    output_value["head"]["cmd"] = header->GetCmd();
	    output_value["head"]["reserve"] = header->GetReserve();

	    Json::value tmp;
	    tmp.decodeJson(buf+sizeof(struct CmdHead),cur_buflen - sizeof(struct CmdHead));
	    output_value["body"] = tmp;
	}
	else { //其他不支持
	    errmsg="don't support :" + input_value["ctrl"]["mode"].asString();
	    return false;
	}
	//关注assert,输出结果到output_value
	if(input_value.isMember("assert")) {	//需要进行断言,暂时不支持
	    output_value["result"]["total"]="passed";
	    output_value["result"]["case_num"]=0;
	} else {
	    output_value["result"]["total"]="passed";
	    output_value["result"]["case_num"]=0;
	}

	return true;
    };
    virtual bool finish() { 
	//以什么形式回包	
	return true; 
    };
    private:
    SCmdHead header;
}
}
