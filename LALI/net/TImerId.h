//
//Created by lalalilia
//

#ifndef UNTITLED1_TIMERID_H
#define UNTITLED1_TIMERID_H


class Timer;

class TimerId{

public:
    explicit TimerId(Timer* timer)
            :value_(timer){

    }

private:
    Timer* value_;


};

#endif //UNTITLED1_TIMERID_H
