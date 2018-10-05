//˵������

//ϵͳ

#include <string>
#include <queue>

//Boost
#define BOOST_PYTHON_STATIC_LIB
#include <boost/python/module.hpp>	//python��װ
#include <boost/python/def.hpp>		//python��װ
#include <boost/python/dict.hpp>	//python��װ
#include <boost/python/object.hpp>	//python��װ
#include <boost/python.hpp>			//python��װ
#include <boost/thread.hpp>			//������е��̹߳���
#include <boost/bind.hpp>			//������е��̹߳���
#include <boost/any.hpp>			//������е�����ʵ��
#include <boost/locale.hpp>			//�ַ���ת��

//API
#include "ThostFtdcMdApi.h"

//�����ռ�
using namespace std;
using namespace boost::python;
using namespace boost;

//����
// render_data.ctp_md_header_define start
#define ONFRONTCONNECTED 1
#define ONFRONTDISCONNECTED 2
#define ONHEARTBEATWARNING 3
#define ONRSPUSERLOGIN 4
#define ONRSPUSERLOGOUT 5
#define ONRSPERROR 6
#define ONRSPSUBMARKETDATA 7
#define ONRSPUNSUBMARKETDATA 8
#define ONRSPSUBFORQUOTERSP 9
#define ONRSPUNSUBFORQUOTERSP 10
#define ONRTNDEPTHMARKETDATA 11
#define ONRTNFORQUOTERSP 12

// render_data.ctp_md_header_define end



///-------------------------------------------------------------------------------------
///API�еĲ������
///-------------------------------------------------------------------------------------

//GILȫ�����򻯻�ȡ�ã�
//���ڰ���C++�̻߳��GIL�����Ӷ���ֹpython����
class PyLock
{
private:
	PyGILState_STATE gil_state;

public:
	//��ĳ�����������д����ö���ʱ�����GIL��
	PyLock()
	{
		gil_state = PyGILState_Ensure();
	}

	//��ĳ��������ɺ����ٸö���ʱ�����GIL��
	~PyLock()
	{
		PyGILState_Release(gil_state);
	}
};


//����ṹ��
struct Task
{
	int task_name;		//�ص��������ƶ�Ӧ�ĳ���
	any task_data;		//���ݽṹ��
	any task_error;		//����ṹ��
	int task_id;		//����id
	bool task_last;		//�Ƿ�Ϊ��󷵻�
};


///�̰߳�ȫ�Ķ���
template<typename Data>

class ConcurrentQueue
{
private:
	queue<Data> the_queue;								//��׼�����
	mutable mutex the_mutex;							//boost������
	condition_variable the_condition_variable;			//boost��������

public:

	//�����µ�����
	void push(Data const& data)
	{
		mutex::scoped_lock lock(the_mutex);				//��ȡ������
		the_queue.push(data);							//������д�������
		lock.unlock();									//�ͷ���
		the_condition_variable.notify_one();			//֪ͨ���������ȴ����߳�
	}

	//�������Ƿ�Ϊ��
	bool empty() const
	{
		mutex::scoped_lock lock(the_mutex);
		return the_queue.empty();
	}

	//ȡ��
	Data wait_and_pop()
	{
		mutex::scoped_lock lock(the_mutex);

		while (the_queue.empty())						//������Ϊ��ʱ
		{
			the_condition_variable.wait(lock);			//�ȴ���������֪ͨ
		}

		Data popped_value = the_queue.front();			//��ȡ�����е����һ������
		the_queue.pop();								//ɾ��������
		return popped_value;							//���ظ�����
	}

};


//���ֵ��л�ȡĳ����ֵ��Ӧ������������ֵ������ṹ������ֵ��
void getInt(dict d, string key, int* value);


//���ֵ��л�ȡĳ����ֵ��Ӧ�ĸ�����������ֵ������ṹ������ֵ��
void getDouble(dict d, string key, double* value);


//���ֵ��л�ȡĳ����ֵ��Ӧ���ַ�������ֵ������ṹ������ֵ��
void getChar(dict d, string key, char* value);


//���ֵ��л�ȡĳ����ֵ��Ӧ���ַ���������ֵ������ṹ������ֵ��
void getStr(dict d, string key, char* value);


///-------------------------------------------------------------------------------------
///C++ SPI�Ļص���������ʵ��
///-------------------------------------------------------------------------------------

//API�ļ̳�ʵ��
class MdApi : public CThostFtdcMdSpi
{
private:
	CThostFtdcMdApi* api;				//API����
	thread *task_thread;				//�����߳�ָ�루��python���������ݣ�
	ConcurrentQueue<Task> task_queue;	//�������

public:
	MdApi()
	{
		function0<void> f = boost::bind(&MdApi::processTask, this);
		thread t(f);
		this->task_thread = &t;
	};

	~MdApi()
	{
	};

	//-------------------------------------------------------------------------------------
	//API�ص�����
	//-------------------------------------------------------------------------------------

	// render_data.ctp_md_header_on_orig start
	virtual void OnFrontConnected();
    virtual void OnFrontDisconnected(int nReason);
    virtual void OnHeartBeatWarning(int nTimeLapse);
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) ;
    virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) ;

	// render_data.ctp_md_header_on_orig end

	//-------------------------------------------------------------------------------------
	//task������
	//-------------------------------------------------------------------------------------

	void processTask();

	// render_data.ctp_md_header_process start
	void processFrontConnected(Task task);

    void processFrontDisconnected(Task task);

    void processHeartBeatWarning(Task task);

    void processRspUserLogin(Task task);

    void processRspUserLogout(Task task);

    void processRspError(Task task);

    void processRspSubMarketData(Task task);

    void processRspUnSubMarketData(Task task);

    void processRspSubForQuoteRsp(Task task);

    void processRspUnSubForQuoteRsp(Task task);

    void processRtnDepthMarketData(Task task);

    void processRtnForQuoteRsp(Task task);


	// render_data.ctp_md_header_process end

	//-------------------------------------------------------------------------------------
	//data���ص������������ֵ�
	//error���ص������Ĵ����ֵ�
	//id������id
	//last���Ƿ�Ϊ��󷵻�
	//i������
	//-------------------------------------------------------------------------------------

	virtual void onFrontConnected(){};

	virtual void onFrontDisconnected(int i){};

	virtual void onHeartBeatWarning(int i){};

	// render_data.ctp_md_header_on start
	virtual void onRspUserLogin(dict data, dict error, int id, bool last) {};
    virtual void onRspUserLogout(dict data, dict error, int id, bool last) {};
    virtual void onRspError(dict error, int id, bool last) {};
    virtual void onRspSubMarketData(dict data, dict error, int id, bool last) {};
    virtual void onRspUnSubMarketData(dict data, dict error, int id, bool last) {};
    virtual void onRspSubForQuoteRsp(dict data, dict error, int id, bool last) {};
    virtual void onRspUnSubForQuoteRsp(dict data, dict error, int id, bool last) {};
    virtual void onRtnDepthMarketData(dict data) {};
    virtual void onRtnForQuoteRsp(dict data) {};

	// render_data.ctp_md_header_on end

	//-------------------------------------------------------------------------------------
	//req:���������������ֵ�
	//-------------------------------------------------------------------------------------

	void createFtdcMdApi(string pszFlowPath = "");

	void release();

	void init();

	int join();

	int exit();

	string getTradingDay();

	void registerFront(string pszFrontAddress);

	int subscribeMarketData(string instrumentID);

	int unSubscribeMarketData(string instrumentID);

	int subscribeForQuoteRsp(string instrumentID);

	int unSubscribeForQuoteRsp(string instrumentID);

	// render_data.ctp_md_header_function start
	int reqUserLogin(dict req, int nRequestID);

    int reqUserLogout(dict req, int nRequestID);


	// render_data.ctp_md_header_function end
};
