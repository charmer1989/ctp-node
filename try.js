var ctp = require('./build/Release/ctp.node');
ctp.settings({log: true});

var trader = ctp.createTrader();
trader.on("connect", function (result) {
  console.log("js文件中: ----> on connected , result=", result);
  trader.reqUserLogin("----", "------", "--------", function (result) {
    console.log("js文件中: reqUserlogin result=", result);
  });
});

trader.on("rspUserLogin", function (requestId, isLast, field, info) {
  console.log("js文件中: requestId", requestId);
  console.log("js文件中: isLast", isLast);
  console.log("js文件中: field", JSON.stringify(field));
  console.log("js文件中: info", JSON.stringify(info));

  trader.reqQryTradingAccount('----','------',function(result){
    console.log("js文件中:", 'reqQryTradingAccount return val is '+result);

  });
});

trader.connect("tcp://------", undefined, 0, 1, function (result) {
  console.log("js文件中:", 'connect return val is '+result);
});

console.log("js文件中: getTradingDay", trader.getTradingDay())

trader.on('rqTradingAccount',function(requestId, isLast, field, info){
  console.log("js文件中:", 'rqTradingAccount callback');
  console.log("js文件中:", JSON.stringify(field));
  console.log("js文件中:", JSON.stringify(info));

});

console.log("js文件中:", 'continute');
