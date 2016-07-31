#include <uv.h>
#include "uv_trader.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcMdApi.h"
#include <cstring>
#include <sstream>
#include "wraper_struct.h"
#include <string>

std::string to_string(int val){
  std::stringstream ss;
  ss << val;
  return ss.str();
}

std::string charto_string(char val){
  std::stringstream ss; 
  ss << val;
  return ss.str();
}

std::map<int, CbWrap*> uv_trader::cb_map;

uv_trader::uv_trader(void) {
  iRequestID = 0;
  uv_async_init(uv_default_loop(),&async_t,NULL);
  logger_cout("uv_trader init");
}

uv_trader::~uv_trader(void) {
  uv_close((uv_handle_t*)&async_t,NULL);
}

const char* uv_trader::GetTradingDay(){
  return this->m_pApi->GetTradingDay();
}

int uv_trader::On(const char* eName,int cb_type, void(*callback)(CbRtnField* cbResult)) {
	std::string log = "uv_trader On------>";
	std::map<int, CbWrap*>::iterator it = cb_map.find(cb_type);
	if (it != cb_map.end()) {
		logger_cout(log.append(" event id").append(to_string(cb_type)).append(" register repeat").c_str());
		return 1;
	}

	CbWrap* cb_wrap = new CbWrap();
	cb_wrap->callback = callback;
	cb_map[cb_type] = cb_wrap;
	logger_cout(log.append(" Event:").append(eName).append(" ID:").append(to_string(cb_type)).append(" register").c_str());
	return 0;
}

void uv_trader::Connect(UVConnectField* pConnectField, void(*callback)(int, void*), int uuid) {
	UVConnectField* _pConnectField = new UVConnectField();
	memcpy(_pConnectField, pConnectField, sizeof(UVConnectField));
  logger_cout("trader Connect this -> invoke");
	this->invoke(_pConnectField, T_CONNECT_RE, callback, uuid);
}

void uv_trader::ReqUserLogin(CThostFtdcReqUserLoginField *pReqUserLoginField, void(*callback)(int, void*), int uuid) {
	CThostFtdcReqUserLoginField *_pReqUserLoginField = new CThostFtdcReqUserLoginField();
	memcpy(_pReqUserLoginField, pReqUserLoginField, sizeof(CThostFtdcReqUserLoginField));
	this->invoke(_pReqUserLoginField, T_LOGIN_RE, callback, uuid);
}

void uv_trader::Disconnect() {
	m_pApi->RegisterSpi(NULL);
	m_pApi->Release();
	m_pApi = NULL;

	std::map<int, CbWrap*>::iterator callback_it = cb_map.begin();
	while (callback_it != cb_map.end()) {
    delete callback_it->second;
		callback_it++;
	}
	logger_cout("uv_trader Disconnect------>object destroyed");
}


void uv_trader::ReqUserLogout(CThostFtdcUserLogoutField *pUserLogout, void(*callback)(int, void*), int uuid) {
  CThostFtdcUserLogoutField* _pUserLogout = new CThostFtdcUserLogoutField();
	memcpy(_pUserLogout, pUserLogout, sizeof(CThostFtdcUserLogoutField));
	this->invoke(_pUserLogout, T_LOGOUT_RE, callback, uuid);
}

void uv_trader::ReqQryTradingAccount(CThostFtdcQryTradingAccountField *pQryTradingAccount, void(*callback)(int, void*), int uuid) {
	CThostFtdcQryTradingAccountField *_pQryTradingAccount = new CThostFtdcQryTradingAccountField();
	memcpy(_pQryTradingAccount, pQryTradingAccount, sizeof(CThostFtdcQryTradingAccountField));
	this->invoke(_pQryTradingAccount, T_TRADINGACCOUNT_RE, callback, uuid);
}

void uv_trader::ReqOrderInsert(CThostFtdcInputOrderField *pInputOrder, void(*callback)(int, void*), int uuid) {
	CThostFtdcInputOrderField *_pInputOrder = new CThostFtdcInputOrderField();
	memcpy(_pInputOrder, pInputOrder, sizeof(CThostFtdcInputOrderField));
	this->invoke(_pInputOrder, T_INSERT_RE, callback, uuid);
}

void uv_trader::ReqOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, void(*callback)(int, void*), int uuid) {
	CThostFtdcInputOrderActionField *_pInputOrderAction = new CThostFtdcInputOrderActionField();
	memcpy(_pInputOrderAction, pInputOrderAction, sizeof(CThostFtdcInputOrderActionField));
	this->invoke(_pInputOrderAction, T_ACTION_RE, callback, uuid);
}

void uv_trader::ReqQryDepthMarketData(CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, void(*callback)(int, void*), int uuid) {
	CThostFtdcQryDepthMarketDataField *_pQryDepthMarketData = new CThostFtdcQryDepthMarketDataField();
	memcpy(_pQryDepthMarketData, pQryDepthMarketData, sizeof(CThostFtdcQryDepthMarketDataField));
	this->invoke(_pQryDepthMarketData, T_DEPTHMARKETDATA_RE, callback, uuid);
}




void uv_trader::OnFrontConnected() {
	std::string log = "uv_trader OnFrontConnected";
  logger_cout(log.c_str());
	CbRtnField* field = new CbRtnField();
	field->eFlag = T_ON_CONNECT;
  field->work.data = field;
	uv_queue_work(uv_default_loop(), &field->work, _on_async, _on_completed);
}

void uv_trader::OnFrontDisconnected(int nReason) {
	std::string log = "uv_trader OnFrontDisconnected------>";
	logger_cout(log.append("nReason:").append(to_string(nReason)).c_str());
	CbRtnField* field = new CbRtnField();
	field->eFlag = T_ON_DISCONNECTED;
	field->nReason = nReason;
	field->work.data = field;
	uv_queue_work(uv_default_loop(), &field->work, _on_async, _on_completed);
}

void uv_trader::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CThostFtdcRspUserLoginField* _pRspUserLogin = NULL;
	if (pRspUserLogin) {
		_pRspUserLogin = new CThostFtdcRspUserLoginField();
		memcpy(_pRspUserLogin, pRspUserLogin, sizeof(CThostFtdcRspUserLoginField));
	}
	std::string log = "uv_trader OnRspUserLogin------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RSPUSERLOGIN, _pRspUserLogin, pRspInfo, nRequestID, bIsLast);
}




void uv_trader::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CThostFtdcUserLogoutField* _pUserLogout = NULL;
	if (pUserLogout) {
		_pUserLogout = new CThostFtdcUserLogoutField();
		memcpy(_pUserLogout, pUserLogout, sizeof(CThostFtdcUserLogoutField));
	}
	std::string log = "uv_trader OnRspUserLogout------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RSPUSERLOGOUT, _pUserLogout, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CThostFtdcInputOrderField* _pInputOrder = NULL;
	if (pInputOrder) {
		_pInputOrder = new CThostFtdcInputOrderField();
		memcpy(_pInputOrder, pInputOrder, sizeof(CThostFtdcInputOrderField));
	}
	std::string log = "uv_trader OnRspOrderInsert------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RSPINSERT, _pInputOrder, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo) {
	CThostFtdcInputOrderField* _pInputOrder = NULL;
	if (pInputOrder) {
		_pInputOrder = new CThostFtdcInputOrderField();
		memcpy(_pInputOrder, pInputOrder, sizeof(CThostFtdcInputOrderField));
	}
	std::string log = "uv_trader OnErrRtnOrderInsert";
	logger_cout(log.c_str());
	on_invoke(T_ON_ERRINSERT, _pInputOrder, pRspInfo, 0, 0);
}

void uv_trader::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CThostFtdcInputOrderActionField* _pInputOrderAction = NULL;
	if (pInputOrderAction) {
		_pInputOrderAction = new CThostFtdcInputOrderActionField();
		memcpy(_pInputOrderAction, pInputOrderAction, sizeof(CThostFtdcInputOrderActionField));
	}
	std::string log = "uv_trader OnRspOrderAction------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RSPACTION, _pInputOrderAction, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo) {
	CThostFtdcOrderActionField* _pOrderAction = NULL;
	if (pOrderAction) {
		_pOrderAction = new CThostFtdcOrderActionField();
		memcpy(_pOrderAction, pOrderAction, sizeof(CThostFtdcOrderActionField));
	}
	std::string log = "uv_trader OnErrRtnOrderAction";
	logger_cout(log.c_str());
	on_invoke(T_ON_ERRACTION, _pOrderAction, pRspInfo, 0, 0);
}

void uv_trader::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CThostFtdcOrderField* _pOrder = NULL;
	if (pOrder) {
		_pOrder = new CThostFtdcOrderField();
		memcpy(_pOrder, pOrder, sizeof(CThostFtdcOrderField));
	}
	std::string log = "uv_trader OnRspQryOrder------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RQORDER, _pOrder, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnRtnOrder(CThostFtdcOrderField *pOrder) {
	CThostFtdcOrderField* _pOrder = NULL;
	if (pOrder) {
		_pOrder = new CThostFtdcOrderField();
		memcpy(_pOrder, pOrder, sizeof(CThostFtdcOrderField));
	}
	std::string log = "uv_trader OnRtnOrder";
	logger_cout(log.c_str());
	on_invoke(T_ON_RTNORDER, _pOrder, new CThostFtdcRspInfoField(), 0, 0);
}

void uv_trader::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CThostFtdcTradeField* _pTrade = NULL;
	if (pTrade) {
		_pTrade = new CThostFtdcTradeField();
		memcpy(_pTrade, pTrade, sizeof(CThostFtdcTradeField));
	}
	std::string log = "uv_trader OnRspQryTrade------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RQTRADE, _pTrade, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnRtnTrade(CThostFtdcTradeField *pTrade) {
	CThostFtdcTradeField* _pTrade = NULL;
	if (pTrade) {
		_pTrade = new CThostFtdcTradeField();
		memcpy(_pTrade, pTrade, sizeof(CThostFtdcTradeField));
	}
	std::string log = "uv_trader OnRtnTrade";
	logger_cout(log.c_str());
	on_invoke(T_ON_RTNTRADE, _pTrade, new CThostFtdcRspInfoField(), 0, 0);
}

void uv_trader::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CThostFtdcTradingAccountField *_pTradingAccount = NULL;
	if (pTradingAccount) {
		_pTradingAccount = new CThostFtdcTradingAccountField();
		memcpy(_pTradingAccount, pTradingAccount, sizeof(CThostFtdcTradingAccountField));
	}
	std::string log = "uv_trader OnRspQryTradingAccount------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RQTRADINGACCOUNT, _pTradingAccount, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CThostFtdcDepthMarketDataField* _pDepthMarketData = NULL;
	if (pDepthMarketData) {
		_pDepthMarketData = new CThostFtdcDepthMarketDataField();
		memcpy(pDepthMarketData, _pDepthMarketData, sizeof(CThostFtdcDepthMarketDataField));
	}
	std::string log = "uv_trader OnRspQryDepthMarketData------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RQDEPTHMARKETDATA, _pDepthMarketData, pRspInfo, nRequestID, bIsLast);
}




void uv_trader::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CThostFtdcRspInfoField* _pRspInfo = NULL;
	if (pRspInfo) {
		_pRspInfo = new CThostFtdcRspInfoField();
		memcpy(_pRspInfo, pRspInfo, sizeof(CThostFtdcRspInfoField));
	}
	std::string log = "uv_trader OnRspError------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RSPERROR, _pRspInfo, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::_async(uv_work_t * work) {
	LookupCtpApiBaton* baton = static_cast<LookupCtpApiBaton*>(work->data);
	uv_trader* uv_trader_obj = static_cast<uv_trader*>(baton->uv_trader_obj);
	std::string log = "uv_trader _async------>";
  logger_cout(log.append(to_string(baton->fun)).c_str());
	switch (baton->fun) {
	case T_CONNECT_RE:
    {
      UVConnectField* _pConnectF = static_cast<UVConnectField*>(baton->args);
      uv_trader_obj->m_pApi = CThostFtdcTraderApi::CreateFtdcTraderApi(_pConnectF->szPath);
      uv_trader_obj->m_pApi->RegisterSpi(uv_trader_obj);
      uv_trader_obj->m_pApi->SubscribePublicTopic(static_cast<THOST_TE_RESUME_TYPE>(_pConnectF->public_topic_type));
      uv_trader_obj->m_pApi->SubscribePrivateTopic(static_cast<THOST_TE_RESUME_TYPE>(_pConnectF->private_topic_type));
      uv_trader_obj->m_pApi->RegisterFront(_pConnectF->front_addr);
      uv_trader_obj->m_pApi->Init(); // CThostFtdcMdApi
      logger_cout(log.append("invoke connect,the result is 0 | szPath is ").append(_pConnectF->szPath).append(CThostFtdcTraderApi::GetApiVersion()).c_str());
      break;
    }
	case T_LOGIN_RE:
    {
      CThostFtdcReqUserLoginField *_pReqUserLoginField = static_cast<CThostFtdcReqUserLoginField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqUserLogin(_pReqUserLoginField, baton->iRequestID);
      logger_cout(log.append("invoke ReqUserLogin,the result:").append(to_string(baton->nResult)).c_str());
      break;
    }
  case T_LOGOUT_RE:
    {
      CThostFtdcUserLogoutField* _pUserLogout = static_cast<CThostFtdcUserLogoutField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqUserLogout(_pUserLogout, baton->iRequestID);
      logger_cout(log.append("invoke ReqUserLogout,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  case T_CONFIRM_RE:
    {
      CThostFtdcSettlementInfoConfirmField* _pSettlementInfoConfirm = static_cast<CThostFtdcSettlementInfoConfirmField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqSettlementInfoConfirm(_pSettlementInfoConfirm, baton->iRequestID);
      logger_cout(log.append("invoke ReqSettlementInfoConfirm,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  case T_INSTRUMENT_RE:
    {
      CThostFtdcQryInstrumentField *_pQryInstrument = static_cast<CThostFtdcQryInstrumentField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqQryInstrument(_pQryInstrument, baton->iRequestID);
      logger_cout(log.append("invoke ReqQryInstrument,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  case T_TRADINGACCOUNT_RE:
    {
      CThostFtdcQryTradingAccountField *_pQryTradingAccount = static_cast<CThostFtdcQryTradingAccountField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqQryTradingAccount(_pQryTradingAccount, baton->iRequestID);
      logger_cout(log.append("invoke ReqQryTradingAccount,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  case T_INVESTORPOSITION_RE:
    {
      CThostFtdcQryInvestorPositionField *_pQryInvestorPosition = static_cast<CThostFtdcQryInvestorPositionField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqQryInvestorPosition(_pQryInvestorPosition, baton->iRequestID);
      logger_cout(log.append("invoke ReqQryInvestorPosition,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  case T_INVESTORPOSITIONDETAIL_RE:
    {
      CThostFtdcQryInvestorPositionDetailField *_pQryInvestorPositionDetail = static_cast<CThostFtdcQryInvestorPositionDetailField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqQryInvestorPositionDetail(_pQryInvestorPositionDetail, baton->iRequestID);
      logger_cout(log.append("invoke ReqQryInvestorPositionDetail,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  case T_INSERT_RE:
    {
      CThostFtdcInputOrderField *_pInputOrder = static_cast<CThostFtdcInputOrderField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqOrderInsert(_pInputOrder, baton->iRequestID);
      logger_cout(log.append("invoke ReqOrderInsert,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  case T_ACTION_RE:
    {
      CThostFtdcInputOrderActionField *_pInputOrderAction = static_cast<CThostFtdcInputOrderActionField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqOrderAction(_pInputOrderAction, baton->iRequestID);
      logger_cout(log.append("invoke ReqOrderAction,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  case T_MARGINRATE_RE:
    {
      CThostFtdcQryInstrumentMarginRateField *_pQryInstrumentMarginRate = static_cast<CThostFtdcQryInstrumentMarginRateField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqQryInstrumentMarginRate(_pQryInstrumentMarginRate, baton->iRequestID);
      logger_cout(log.append("invoke ReqQryInstrumentMarginRate,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  case T_DEPTHMARKETDATA_RE:
    {
      CThostFtdcQryDepthMarketDataField *_pQryDepthMarketData = static_cast<CThostFtdcQryDepthMarketDataField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqQryDepthMarketData(_pQryDepthMarketData, baton->iRequestID);
      logger_cout(log.append("invoke ReqQryDepthMarketData,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  case T_SETTLEMENTINFO_RE:
    {
      CThostFtdcQrySettlementInfoField *_pQrySettlementInfo = static_cast<CThostFtdcQrySettlementInfoField*>(baton->args);
      baton->nResult = uv_trader_obj->m_pApi->ReqQrySettlementInfo(_pQrySettlementInfo, baton->iRequestID);
      logger_cout(log.append("invoke ReqQrySettlementInfo,the result:").append(to_string(baton->nResult)).c_str());

      break;
    }
  default:
    {
      logger_cout(log.append("No case event:").append(to_string(baton->fun)).c_str());
      break;
    }
	}
}
///uv_queue_work
void uv_trader::_completed(uv_work_t * work, int) {
	LookupCtpApiBaton* baton = static_cast<LookupCtpApiBaton*>(work->data);
	baton->callback(baton->nResult, baton);
	delete baton->args;
	delete baton;
}

void uv_trader::_on_async(uv_work_t * work){
    //do nothing
}

void uv_trader::_on_completed(uv_work_t * work,int){
	std::string head = "uv_trader _on_completed  ==== ";
  logger_cout(head.c_str());

	CbRtnField* cbTrnField = static_cast<CbRtnField*>(work->data);
	std::map<int, CbWrap*>::iterator it = cb_map.find(cbTrnField->eFlag);
	if (it != cb_map.end()) {
		cb_map[cbTrnField->eFlag]->callback(cbTrnField);
	}
	if (cbTrnField->rtnField)
		delete cbTrnField->rtnField;
	if (cbTrnField->rspInfo)
		delete cbTrnField->rspInfo;
	delete cbTrnField;
}

void uv_trader::invoke(void* field, int ret, void(*callback)(int, void*), int uuid) {
	LookupCtpApiBaton* baton = new LookupCtpApiBaton();
	baton->work.data = baton;
	baton->uv_trader_obj = this;
	baton->callback = callback;
	baton->args = field;
	baton->fun = ret;
	baton->uuid = uuid;

  iRequestID = iRequestID+1;
  baton->iRequestID = iRequestID;
	std::string head = "uv_trader invoke------>uuid:";
	logger_cout(head.append(to_string(uuid)).append(",requestid:").append(to_string(baton->iRequestID)).c_str());
	uv_queue_work(uv_default_loop(), &baton->work, _async, _completed);
}

void uv_trader::on_invoke(int event_type, void* _stru, CThostFtdcRspInfoField *pRspInfo_org, int nRequestID, bool bIsLast){
  CThostFtdcRspInfoField* _pRspInfo = NULL;
	if (pRspInfo_org) {
		_pRspInfo = new CThostFtdcRspInfoField();
		memcpy(_pRspInfo, pRspInfo_org, sizeof(CThostFtdcRspInfoField));
	}
	CbRtnField* field = new CbRtnField();
  field->work.data = field;
	field->eFlag = event_type;
	field->rtnField = _stru;
	field->rspInfo = (void*)_pRspInfo;
	field->nRequestID = nRequestID;
	field->bIsLast = bIsLast;
	uv_queue_work(uv_default_loop(), &field->work, _on_async, _on_completed);
}
