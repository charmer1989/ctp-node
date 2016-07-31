#ifndef UV_TRADER_H_
#define UV_TRADER_H_

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include <uv.h>
#include <node.h>
#include "wraper_struct.h"

extern bool islog;
void logger_cout(const char* content);
std::string to_string(int val);
std::string charto_string(char val);

class uv_trader : public CThostFtdcTraderSpi {
public:
  uv_trader(void);
	virtual ~uv_trader(void);

  const char* GetTradingDay();
  /// 注册
  int On(const char* eName,int cb_type, void(*callback)(CbRtnField* cbResult));
  /// 连接
  void Connect(UVConnectField* pConnectField, void(*callback)(int, void*),int uuid);
  /// 登录
  void ReqUserLogin(CThostFtdcReqUserLoginField *pReqUserLoginField, void(*callback)(int, void*),int uuid);
  /// 断开
  void Disconnect();

  // 登出
  void  ReqUserLogout(CThostFtdcUserLogoutField *pUserLogout, void(*callback)(int, void*),int uuid);
	// 投资者结算结果确认
  // void  ReqSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, void(*callback)(int, void*),int uuid);
	// 请求查询合约
  //void ReqQryInstrument(CThostFtdcQryInstrumentField *pQryInstrument, void(*callback)(int, void*),int uuid);
	// 请求查询资金账户
  void ReqQryTradingAccount(CThostFtdcQryTradingAccountField *pQryTradingAccount, void(*callback)(int, void*),int uuid);
  // 请求查询投资者持仓
  // void ReqQryInvestorPosition(CThostFtdcQryInvestorPositionField *pQryInvestorPosition, void(*callback)(int, void*),int uuid);
  // 持仓明细
  // void ReqQryInvestorPositionDetail(CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, void(*callback)(int, void*),int uuid);
	// 报单录入请求
  void ReqOrderInsert(CThostFtdcInputOrderField *pInputOrder, void(*callback)(int, void*),int uuid);
	// 报单操作请求
  void ReqOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, void(*callback)(int, void*),int uuid);
	// 请求查询合约保证率
  // void ReqQryInstrumentMarginRate(CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, void(*callback)(int, void*),int uuid);
	// 请求查询行情
   void ReqQryDepthMarketData(CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, void(*callback)(int, void*),int uuid);
	// 请求查询投资者结算结果
  // void ReqQrySettlementInfo(CThostFtdcQrySettlementInfoField *pQrySettlementInfo, void(*callback)(int, void*),int uuid);

private:
  static void _async(uv_work_t * work);
  static void _completed(uv_work_t * work, int);
  static void _on_async(uv_work_t * work);
  static void _on_completed(uv_work_t * work,int);
  void invoke(void* field, int ret, void(*callback)(int, void*), int uuid);
  void on_invoke(int event_type, void* _stru, CThostFtdcRspInfoField *pRspInfo_org, int nRequestID, bool bIsLast);

  virtual void OnFrontConnected();
  virtual void OnFrontDisconnected(int nReason);
  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  virtual void  OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 投资者结算结果确认响应
	// virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 报单录入请求响应
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 报单录入错误回报
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
  // 报单操作请求响应
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 报单操作错误回报
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
  // 请求查询报单响应
  virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 报单通知
  virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
  // 请求查询成交响应
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 成交通知
  virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
  // 请求查询投资者持仓响应
	// virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 请求查询投资者持仓明细响应
	// virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 请求查询账户资金响应
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 请求查询合约响应
	// virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 请求查询行情响应
	 virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 请求查询投资者结算结果响应
	// virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  CThostFtdcTraderApi* m_pApi;
  int iRequestID;
  uv_async_t async_t;
  static std::map<int, CbWrap*> cb_map;
};

#endif
