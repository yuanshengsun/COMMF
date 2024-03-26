#include"angle_message.h"
namespace YSSLM {
	namespace node {
		namespace message {
			namespace angle {
				//typedef struct AngleDataJYME {
				//	double data;8
				//};


				//struct AngleDataJYMET {
				//	AngleDataJYME data;
				//	timeType_timeOfClock time;20
				//};

				void  AngleMessage::operator=(const AngleMessage &data) {
					name = data.name;


				}

				void AngleMessage::Set(const std::shared_ptr<const std::string>& data){
					name = *data;
				}
				std::shared_ptr<std::string> AngleMessage::Get(){
					return std::make_shared<std::string>(name);
				}

			}
		}
	}
}