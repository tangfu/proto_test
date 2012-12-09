
#include<std>
#include<map>
#include<json/json.h>

#define localbindport 52222
using namespace std;


char *state[] = {"INPUT", "PROCESS", "FINISH"};
enum STAT {INPUT=0,PROCESS,FINISH};



namespace proto_extend {
    //每一步以返回0表示成功，-1表示失败，失败将直接返回错误包
    typedef (*callback)(void);			//必须,验证是否是json

    void run(class proto_base& tmp) {
	tmp->run();
    }

    class proto_base {
	public:
	    proto_base() : stat_index(0),cur_buflen(0) {}
	    virtual ~proto_base() {};
	    
	    virtual bool input() {};
	    virtual bool process() {};
	    virtual bool finish() {};

	    void run() {
		check_err(input(buf, 8196));
		++stat_index;
		check_err(process(buf, 8196));
		++stat_index;
		check_err(finish(buf, 8196));
	    }
	private:
	    string name;
	    string errmsg;
	    int state_index;
	    Json::Value input; //JsonString
	    Json::Value output;//JsonString
	    char buf[8196];
	    int cur_buflen;
	    void check_err() {
		if(isErr) 
		    LOG(ERROR) << "state=" << state[state_index] << "|" << errmsg;
		return;
	    }
    }
}
