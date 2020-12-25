#ifndef _thread_h
//Created by lalalilia
#define _thread_h
/*
	��������дһ���߳�����Ҫ��ʲô�ðɣ�
	���ȵ����̵߳Ĵ����ѣ�����Ǳ���ģ�
	Ȼ�����ǵ�������̵߳ĵȴ��ѣ�������������Ǹ�join��
	�� ��һ������߳���ĳ�Ա����Ҫ��ɶ�أ�
	�߳�id ���������аɣ�
	�̵߳�״̬���аɣ�
	�߳����ˣ��������ִ�й����ѣ����Ե���һ�����ܻص������ı������Ͱѣ�
	��ʱ������Щ��
	�����Ǹ�ԭ�������о�ûɶ��Ҫ��֮���ٿ���
	�� ��Ա����Ҫ��ɶ�أ�
	����һ�������ĵ� ���캯����������캯������ó�ʼ�����еı�����Ȼ�󴴽�һ���̣߳��ԣ�
	Ȼ��û��ô��Ҫ�������������������������ȷ������̺߳������ʧ�������ڴ�й©�԰ѣ�
	Ȼ���дһ���������״̬�ĺ������Ա��û�֪������̵߳�һЩ״̬��
	�� ���ڵ�ǰ�̣߳�
	��muduo���أ�������һ�������ռ������������ǰ�̣߳��������ͦ�б�Ҫ�ģ�

*/

#include <pthread.h>
#include <memory>
#include <functional>

class Thread {
public:
    typedef std::function<void()> Threadfunc;
	explicit Thread(const Threadfunc&,const std::string& name = std::string()) ;
    ~Thread() ;
	 bool started() {};
	 void start();
	 void join();
     void runInthread();
private:
	bool started_{}; // ��ʼ״̬��
	Threadfunc func_; // ��Ҫһ���Ǹ��ص������������߳�ִ�е�����
	pthread_t pid_{}; // �߳� id
	pid_t tid_{}; // ��ǰ true id
	std::string name_; //��ǰ����߳�����;
	//Ϊʲôһ����Ҫ����̬�����أ���Ϊ��ͨ�����Ա�����Ļ��ͻ��Դ�һ�� this ָ�����������̬����û��
	//���Ե����Ա������Ϊ pthread_created �����Ĳ�����ʱ��ͻ���Ϊ����ƥ�䲹�ϣ�������
	//��Ϊʲô��̬������û��thisָ���أ�
	static void* ThreadRunfunc(void* obj); 
};



#endif
