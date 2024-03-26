#include"message_core.h"
#include"../base/component.h"
#include"../log.h"

namespace YSSLM {
	namespace engine {
		namespace msg_c {
std::vector<std::vector<std::shared_ptr<Read>>> MesaageCore::read_data_call_back;
std::vector<std::vector<std::string>>  MesaageCore::msg_name;
std::vector<std::string> MesaageCore::chennle_name;
std::map<uint32_t , std::map<int , std::map<std::string , std::vector<MesaageCore::UdpReadChannelData>>>> MesaageCore::udp_read_data_call_back;
std::map<uint32_t , std::map<int ,YSSLM::engine::udp::Recvfrom>> MesaageCore::udp_recvfrom;


uint64_t MesaageCore::write_add(const std::string chenlne) {
	for (uint64_t i = 0; i < chennle_name.size(); ++i) {
		if (chenlne == chennle_name[i]) {
			return i;
		}
	}
	chennle_name.push_back(chenlne);

	read_data_call_back.push_back(std::vector<std::shared_ptr<Read>>());

	msg_name.push_back(std::vector<std::string>());

	return (uint64_t)chennle_name.size() - 1;
}

void MesaageCore::read_add(const std::string chenlne, const std::string msg_c, std::shared_ptr<Read> read_call_back) {
	for (uint64_t i = 0; i < chennle_name.size(); ++i) {
		if (chenlne == chennle_name[i]) {
			read_data_call_back[i].push_back(read_call_back);
			msg_name[i].push_back(msg_c);
			return;
		}
	}
	chennle_name.push_back(chenlne);

	read_data_call_back.push_back(std::vector<std::shared_ptr<Read>>());
	read_data_call_back.back().push_back(read_call_back);

	msg_name.push_back(std::vector<std::string>());
	msg_name.back().push_back(msg_c);
}

void MesaageCore::udp_read_add(const std::string chenlne, ChannelData &channel_data , 
								std::function<void(std::shared_ptr<std::string>&)> read_call_back,
								int policy0,
								int sched_priority0,
								int cpu_id0,
								int num_thr0) {
	if(channel_data.udp_open == false){
		return;
	}
	if(udp_read_data_call_back.find(channel_data.udp_addr) == udp_read_data_call_back.end()){
		udp_read_data_call_back.emplace(channel_data.udp_addr,std::map<int , std::map<std::string , std::vector<UdpReadChannelData>>>());
	}
	if(udp_read_data_call_back[channel_data.udp_addr].find(channel_data.udp_port) == udp_read_data_call_back[channel_data.udp_addr].end()){
		udp_read_data_call_back[channel_data.udp_addr].emplace(channel_data.udp_port,std::map<std::string , std::vector<UdpReadChannelData>>());
	}
	if(udp_read_data_call_back[channel_data.udp_addr][channel_data.udp_port].find(chenlne) == udp_read_data_call_back[channel_data.udp_addr][channel_data.udp_port].end()){
		udp_read_data_call_back[channel_data.udp_addr][channel_data.udp_port].emplace(chenlne,std::vector<UdpReadChannelData>());
	}
	udp_read_data_call_back[channel_data.udp_addr][channel_data.udp_port][chenlne].emplace_back(channel_data,read_call_back,policy0,sched_priority0,cpu_id0,num_thr0);
}

std::vector<std::shared_ptr<Read>> MesaageCore::load_init(std::string name, uint64_t id) {
	std::vector<std::shared_ptr<Read>> data;
	for (int i = 0; i < (int)msg_name[id].size(); ++i) {
		if (msg_name[id][i] == name) {
			data.push_back(read_data_call_back[id][i]);
		}
	}
	return data;
}

bool MesaageCore::Udp_init(){
	for(auto &data_d :udp_read_data_call_back){
		for(auto &data_p :data_d.second){
			for(auto &data_m :data_p.second){
				for(auto &data_c :data_m.second){
					if(!Udp_r_init(data_c.channel_data)){
						ULOG_ERROR("MesaageCore::Udp_init() Udp_r_init Init fell !");
						return false;
					}
					udp_recvfrom[data_d.first][data_p.first].RecvfromAdd(data_m.first,
							data_c.read,
							data_c.policy,
							data_c.sched_priority,
							data_c.cpu_id,
							data_c.num_thr);
				}
			}
		}
	}
	for(auto &data_d :udp_recvfrom){
		for(auto &data_p :data_d.second){
			data_p.second.Start();
		}
	}
	return true;
}

void MesaageCore::Udp_close(){
	for(auto &data_d :udp_recvfrom){
		for(auto &data_p :data_d.second){
			data_p.second.Close();
		}
	}
}

bool MesaageCore::Udp_r_init(ChannelData &channel_data){
	if(base::NodeComponent::write_udp_is_exist(channel_data.udp_addr,channel_data.udp_port)){
		ULOG_ERROR("MesaageCore::Udp_r_init() Init(udp addr %s port %d) WUDP == RUDP error !" , channel_data.udp_addr_s.c_str(), (int)channel_data.udp_port);
		return false;
	}
	if(udp_recvfrom.find(channel_data.udp_addr) == udp_recvfrom.end()){
		udp_recvfrom.emplace(channel_data.udp_addr,std::map<int ,Recvfrom>());
	}
	if(udp_recvfrom[channel_data.udp_addr].find(channel_data.udp_port) == udp_recvfrom[channel_data.udp_addr].end()){
		udp_recvfrom[channel_data.udp_addr].emplace(channel_data.udp_port,Recvfrom());
		if(udp_recvfrom[channel_data.udp_addr][channel_data.udp_port].Init(channel_data.udp_port , channel_data.udp_addr) != 0 ){
			ULOG_ERROR("MesaageCore::Udp_r_init() Init(udp addr %s port %d) fell !" , channel_data.udp_addr_s.c_str(), (int)channel_data.udp_port);
			return false;
		}
	}

	return true;
}


		}
	}
}
