//
//Created by lalalilia
//

#include "Timer.h"

// Timer.cc

void Timer::restart(Timestamp now){
    if(repeat_){
        expiration_ = addTime(now,interval_);
    }
    else{
        expiration_ = Timestamp::invalic();
    }

}