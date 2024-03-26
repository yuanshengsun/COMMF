#ifndef _YuanshengSun_LM_Engine_MessageCose_MessageCose_H_
#define _YuanshengSun_LM_Engine_MessageCose_MessageCose_H_

#include<deque>
#include<vector>
#include<stdint.h>
#include<map>
#include <functional> 
#include"../message_core/read.h"
#include"../udp/recvfrom.h"
#include"../udp/sendto.h"


namespace YSSLM {
	namespace engine {
		namespace msg_c {
			
			using Read = YSSLM::engine::msg_c::Read;
			using Recvfrom = YSSLM::engine::udp::Recvfrom;

struct ChannelData
{
	std::string Channel;
	bool udp_open;
	uint32_t udp_addr;
	int udp_port;
	std::string udp_addr_s;
	ChannelData(std::string Channel0,bool udp_open0,uint32_t udp_addr0,int udp_port0 , std::string udp_addr_s0):
		Channel(Channel0),udp_open(udp_open0),udp_addr(udp_addr0),udp_port(udp_port0),udp_addr_s(udp_addr_s0){}
	ChannelData(){};
};

struct MesaageCore {

public:

	static uint64_t write_add(const std::string chenlne) ;

	//

	static void read_add(const std::string chenlne, const std::string msg_c, std::shared_ptr<Read> read_call_back) ;

	static void udp_read_add(const std::string chenlne, ChannelData &channel_data , std::function<void(std::shared_ptr<std::string>&)> read_call_back,
				int policy0,
				int sched_priority0,
				int cpu_id0,
				int num_thr0) ;
	
	//

	static std::vector<std::shared_ptr<Read>> load_init(std::string name, uint64_t id) ;

	static bool Udp_init() ;

	static void Udp_close();
private:
	static bool Udp_r_init(ChannelData &channel_data);

private:

	static std::vector<std::vector<std::shared_ptr<Read>>> read_data_call_back;

	static std::vector<std::string> chennle_name;

	static std::vector<std::vector<std::string>> msg_name;

	struct UdpReadChannelData
	{
		ChannelData channel_data;
		std::function<void(std::shared_ptr<std::string>&)> read;
		int policy;
		int sched_priority;
		int cpu_id;
		int num_thr;
		UdpReadChannelData(ChannelData channel_data0,std::function<void(std::shared_ptr<std::string>&)> read0,
				int policy0,
				int sched_priority0,
				int cpu_id0,
				int num_thr0):
			channel_data(channel_data0),read(read0),policy(policy0),sched_priority(sched_priority0),cpu_id(cpu_id0),num_thr(num_thr0){}
	};
	static std::map<uint32_t , std::map<int ,std::map<std::string , std::vector<UdpReadChannelData>>>> udp_read_data_call_back;

	static std::map<uint32_t , std::map<int ,Recvfrom>> udp_recvfrom;
};

		}
	}
}

#endif