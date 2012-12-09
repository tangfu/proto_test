[模型简化]
1.我们只关心json（mongodb也是json），协议扩展者定义协议与json的对应关系
2.我们只关心udp
3.我们提供基本断言库
4.新增协议时由我们自己来实现,映射关系表由协议制定者提供

[设计]
1.用例的来源，设计为3个渠道：终端，文件，网络
2.基本配置文件在可执行文件当前目录下，proto_extend.conf，用以指定协议实现so的路径

[使用]
* proto_extend			    从终端获取INPUT
* proto_extend -f testcase1	    从文件testcase1中获取INPUT
* proto_extend -server		    从网络获取INPUT

从配置中指定 ”协议名 = 协议的.so文件“ libconfig
以群空间协议为例来操作

[问题]

[输入输出定义]
* INPUT为用例格式:
必须包含proto，head，body，ctrl，assert。
proto用来指明协议。不能为空。
head用来承载协议头部。head为空，表示协议头使用默认（这个看协议制定时的默认值）。
body用来承载协议内容。body为空，表示发空包。
ctrl用来存放控制信息。ctrl不能为空，必须包含mode字段用以指明用例如何发送到后端。
assert用来存放断言条件。assert为空，表明不需要进行断言。

* OUTPUT为用例执行后的结果：
必须包含head，body，result。
这3个字段都不会为空，


INPUT {
    "proto": "gspace",
    "head": {"uin":343385528,"cmd":1,"seqno":2,"reserve":231313},
    "body": {"auth_type":1,"cgi_type":"dfdfdf","guin":757878},
    "ctrl": {"mode":"udp://127.0.0.1:52222","proto":"gspace"},
    "assert" { "EQ": {"seqno":2, "errcode":0} }		    #行为，字段和期待值
}

OUTPUT {
    "head":
    "body":
    "result" { "proto":"gspace","total":"PASSED", "case_num":"2", "case":{"seqno EQ 2":"passed","errcode EQ 0":"passed"} }  
    #与assert对应，如果没有assert条件，那么result字段仅仅包含"total":"Passed","case_num":"0"两个字段
}

