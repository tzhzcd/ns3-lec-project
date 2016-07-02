
#include "ns3/core-module.h"

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("project1");

static void printHello(std::string word,std::string num,float t) {
	std::cout<<Simulator::Now()<<" Hello "<<word<<" "<<num<<std::endl;
	Simulator::Schedule(Seconds(t),&printHello,word,num,t);
}
// ./test arg0 arg1 arg2
// argc=4
// argv[0] -> "test"
// argv[1] -> "arg0"
// argv[2] -> "arg1"
// argv[3] -> "arg2"
// argv[4] -> NULL
int
main (int argc, char *argv[])
{
	CommandLine cmd;
	std::string name,num;
	float t=0;
	if(argc != 4)
                std::cout<<"parameters error!!"<<std::endl;
        else{cmd.AddValue ("name", "my name ", name);
	cmd.AddValue ("num", "my ID ", num);
	cmd.AddValue ("t", "period ", t);
	cmd.Parse(argc,argv);

	printHello(name,num,t);
	Simulator::Stop(Seconds(5));
	Simulator::Run ();
	Simulator::Destroy ();
	}
}
