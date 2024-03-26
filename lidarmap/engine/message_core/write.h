#ifndef _YuanshengSun_LM_Engine_MessageCose_Write_H_
#define _YuanshengSun_LM_Engine_MessageCose_Write_H_
#include"../message_core/message_core.h"
#include"../thread/ThreadPool.h"
#include"../message_core/read.h"
#include <mutex>
#include <atomic>

namespace YSSLM {
	namespace engine {
		namespace msg_c {
			using MesaageCore = YSSLM::engine::msg_c::MesaageCore;
			using threadpool = YSSLM::engine::threadpool::threadpool;
			using Read = YSSLM::engine::msg_c::Read;
			using Sendto = YSSLM::engine::udp::Sendto;

class Write {
public:

	template<class T>
	void write(std::shared_ptr<T> msg_c,bool ptr_write=true) {
		if (!*init_flash) {
			return;
		}
		if (ptr_write) {
			for (uint64_t i = 0; i < call_back_length_; ++i) {

				threadpool_[i]->commit([this, i](std::shared_ptr<T>msg_i) {
					if (read_call_back_[i]->ptr_read) {
						std::shared_ptr<YSSLM::engine::msg_c::Reades<T>>reades =
							std::static_pointer_cast<YSSLM::engine::msg_c::Reades<T>>(read_call_back_[i]);
						reades->call_back(msg_i);
					}
					else {
						std::shared_ptr<T> data2 = std::make_shared<T>();
						*data2=*msg_i;
						std::shared_ptr<YSSLM::engine::msg_c::Reades<T>>reades =
							std::static_pointer_cast<YSSLM::engine::msg_c::Reades<T>>(read_call_back_[i]);
						reades->call_back(data2);
					}
				}, msg_c);
			}
		}
		else {
			std::shared_ptr<T> data = std::make_shared<T>();
			*data=*msg_c;
			for (uint64_t i = 0; i < call_back_length_; ++i) {

				threadpool_[i]->commit([this, i](std::shared_ptr<T>msg_i) {
					if (read_call_back_[i]->ptr_read) {
						std::shared_ptr<YSSLM::engine::msg_c::Reades<T>>reades =
							std::static_pointer_cast<YSSLM::engine::msg_c::Reades<T>>(read_call_back_[i]);
						reades->call_back(msg_i);
					}
					else {
						std::shared_ptr<T> data2 = std::make_shared<T>();
						*data2 = *msg_i;
						std::shared_ptr<YSSLM::engine::msg_c::Reades<T>>reades =
							std::static_pointer_cast<YSSLM::engine::msg_c::Reades<T>>(read_call_back_[i]);
						reades->call_back(data2);
					}
				}, data);
			}
		}
	}
	void write(std::shared_ptr<std::string>& msg_c) {
		if (!*init_flash || !channel_data_.udp_open) {
			return;
		}

		*msg_c +=  channel_data_.Channel;
		short num = channel_data_.Channel.size();
		msg_c->push_back(((char*)&num)[0]);
		msg_c->push_back(((char*)&num)[1]);
		*msg_c += "YSS";
		
		udp_sendto.Send(msg_c,channel_data_.udp_port,channel_data_.udp_addr);
	}

	bool Write_init(std::vector<std::shared_ptr<Read >> call_back) {
		call_back_length_ = call_back.size();

		threadpool_.resize((int)(call_back_length_));

		read_call_back_ = new std::shared_ptr<Read >[call_back_length_];
		for (int i = 0; i < (int)call_back.size(); ++i) {
			read_call_back_[i] = call_back[i];
			threadpool_[i] = std::make_shared<threadpool>(
				read_call_back_[i]->num_thr , 
				0 , 
				read_call_back_[i]->policy ,
				read_call_back_[i]->sched_priority ,
				read_call_back_[i]->cpu_id);
		}

		if(channel_data_.udp_open){
			if(udp_sendto.Init() != 0 ){
				ULOG_ERROR("write::Write_init() udp_sendto Init fell !");
				return false;
			}
		}

		*init_flash = true;

		return true;
	}

	~Write() {
		threadpool_.clear();

		if(channel_data_.udp_open){
			udp_sendto.Close();
		}

		delete[] read_call_back_;
		read_call_back_ = nullptr;
	}

	void close_flash() {
		*init_flash = false;
	}

	bool get_udp_sendto_is_open(){return channel_data_.udp_open;}

	Write(ChannelData &channel_data):channel_data_(channel_data),init_flash(std::make_shared<std::atomic<bool>>(false)){ }

private:

	std::shared_ptr<Read >* read_call_back_;

	std::vector<std::shared_ptr<threadpool>> threadpool_;
	std::mutex mtx;
	std::condition_variable codv;
	std::shared_ptr<std::atomic<bool>> init_flash;

	uint64_t call_back_length_;
	ChannelData channel_data_;
	Sendto udp_sendto;
};

class CWrite {

public:
	template<class T>
	void Send(std::shared_ptr<T> msg_c,bool ptr_write=true){
		if(msg_c == nullptr){
			return;
		}
		for(auto &data : *write){
			data->write(msg_c,ptr_write);
		}

		if(threadpool_ == nullptr){
			return;
		}
		std::shared_ptr<std::string> msg_s = msg_c->Get();
		int sz = msg_s->size();
		for(auto &data : *write){
			threadpool_->commit([this, &data, sz](std::shared_ptr<std::string> msg_c) {
				data->write(msg_c);
				msg_c->resize(sz);
			}, msg_s);
		}
	}

	CWrite(std::shared_ptr<std::vector<std::shared_ptr<Write>>> w , int policy , int sched_priority , int cpu_id):write(w){
		threadpool_ = nullptr;
		for(auto &data : *write){
			if(data->get_udp_sendto_is_open()){
				threadpool_ = std::make_shared<threadpool>(1 , 0 , policy , sched_priority , cpu_id);
				return;
			}
		}
	}
private:
	std::shared_ptr<std::vector<std::shared_ptr<Write>>> write;
	std::shared_ptr<threadpool> threadpool_;
};

		}
	}
}

#endif