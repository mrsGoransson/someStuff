pragma solidity >=0.8.0 <0.9.0;
pragma experimental ABIEncoderV2;

import "./Wallet.sol";
import "./Utils.sol";

contract emmaDex is Wallet {

    using SafeMath for uint256;

    enum ESide {
        BUY,
        SELL
    }

    enum EOrderType {
        LIMIT,
        MARKET
    }

    struct SOrder {
        uint _id;
        address _trader;
        ESide _side;
        bytes32 _ticker;
        uint _amount;
        uint _price;
        uint _filled;
        uint _lockedBalance;
    }

    uint public _nextOrderId;
    mapping(bytes32=>mapping(ESide=>SOrder[])) public _orderBook;

    event OrderExecuted(address buyer, address seller, bytes32 ticker, uint amount, uint price);

    function cancelLimitOrder(ESide side, bytes32 ticker) public tokenExists(ticker) {
        require(_hasOrder(msg.sender, ticker, side), "Must have order to be able to cancel it");

        _removeOrderByTrader(_orderBook[ticker][side], msg.sender);
    }

    function changeExistingLimitOrder(ESide side, bytes32 ticker, uint newAmount, uint newPrice) public tokenExists(ticker) {
        require(_hasOrder(msg.sender, ticker, side), "Must have an existing limit order to modify");

        _removeOrderByTrader(_orderBook[ticker][side], msg.sender);
        createLimitOrder(side, ticker, newAmount, newPrice);
    }

    function createLimitOrder(ESide side, bytes32 ticker, uint amount, uint price) public tokenExists(ticker) {
        _verifyCanMakeOrder(EOrderType.LIMIT, side, ticker, amount, price);

        uint totalFilled = _handleOrder(EOrderType.LIMIT, side, ticker, amount, price);
        _removeFilledOrders(_orderBook[ticker][side == ESide.BUY ? ESide.SELL: ESide.BUY]);

        if(totalFilled < amount) {
            SOrder[] storage orders = _orderBook[ticker][side];
            uint lockedBalance = side == ESide.BUY ? amount.mul(price) : amount;

            orders.push(SOrder(_nextOrderId, msg.sender, side, ticker, amount, price, totalFilled, lockedBalance));
            _nextOrderId++;
            _sortOrders(side, orders);
        }
    }

    function createMarketOrder(ESide side, bytes32 ticker, uint amount) public tokenExists(ticker) {
        _verifyCanMakeOrder(EOrderType.MARKET, side, ticker, amount, 0);

        _handleOrder(EOrderType.MARKET, side, ticker, amount, 0);
        _removeFilledOrders(_orderBook[ticker][side == ESide.BUY ? ESide.SELL: ESide.BUY]);
    }

    function getMyOrder(bytes32 ticker) public view tokenExists(ticker) returns (SOrder memory) {
        SOrder memory foundOrder = _getOrder(msg.sender, ticker);
        require(foundOrder._trader != address(0), "Can't find any order that is yours");
        return foundOrder;
    }

    function getOrderBook(bytes32 ticker, ESide side) public view tokenExists(ticker) returns(SOrder[] memory){
        return _orderBook[ticker][side];
    }

    function getSpendingBalance(bytes32 ticker) public view returns(uint){
        return _getSpendingBalance(msg.sender, ticker);
    }


    function _executeTrade(address buyer, address seller, bytes32 ticker, uint amount, uint cost) private {
        require(_balances[buyer]["ETH"] >= cost, Utils.concatStrings("buyer of token does not have enough ETH, cost: ", Utils.uint2str(cost)));
        require(_balances[seller][ticker] >= amount, "seller of token does not have enough tokens");

        _balances[buyer]["ETH"] = _balances[buyer]["ETH"].sub(cost);
        _balances[buyer][ticker] = _balances[buyer][ticker].add(amount);

        _balances[seller]["ETH"] = _balances[seller]["ETH"].add(cost);
        _balances[seller][ticker] = _balances[seller][ticker].sub(amount);

        emit OrderExecuted(buyer, seller, ticker, amount, cost);
    }

    function _getHowMuchCanBeFilled(SOrder storage order, uint maxAmount) private view returns(uint) {
        uint leftInLimitOrder = order._amount.sub(order._filled);
        uint leftToFill = maxAmount;
        uint canBeFilled = leftToFill < leftInLimitOrder ? leftToFill : leftInLimitOrder;
        return canBeFilled;
    }

    function _getSpendingBalance(address trader, bytes32 ticker) private view returns(uint) {
        if (ticker == "ETH"){
            uint lockedETHBalance = 0;
            for(uint i = 0; i < _tokenList.length; i++) {
                SOrder memory order = _getOrder(trader, _tokenList[i]);
                if(order._trader != address(0)) {
                    if(order._side == ESide.BUY) {
                        lockedETHBalance += order._lockedBalance;
                    }
                }

            }

            return _balances[trader]["ETH"].sub(lockedETHBalance);
        }
        else {
            SOrder memory order = _getOrder(trader, ticker);
            if(order._trader != address(0) && order._ticker == ticker) {
                return _getTokenSpendingBalance(ticker, order._lockedBalance);
            } 
            else {
                return _getTokenSpendingBalance(ticker, 0);
            }
        }
    }

    function _getTokenSpendingBalance(bytes32 ticker, uint lockedBalance) private view tokenExists(ticker) returns(uint) {
        return _balances[msg.sender][ticker].sub(lockedBalance);
    }

    function _getOrder(address trader, bytes32 ticker) private view returns (SOrder memory){
        SOrder[] storage buyOrders = _orderBook[ticker][ESide.BUY];
        SOrder memory foundOrder;
        for(uint i = 0; i < buyOrders.length; i++) {
            if (buyOrders[i]._trader == trader){
                foundOrder = buyOrders[i];
                break;
            }
        }
        if(foundOrder._trader != trader){
            SOrder[] storage sellOrders = _orderBook[ticker][ESide.SELL];
            for(uint i = 0; i < sellOrders.length; i++) {
                if (sellOrders[i]._trader == trader){
                    foundOrder = sellOrders[i];
                    break;
                }
            }
        }
        return foundOrder;
    }

    function _handleOrder(EOrderType orderType, ESide side, bytes32 ticker, uint amount, uint price) private returns(uint wasFilled) {
        SOrder[] storage otherSideOrders = _orderBook[ticker][side == ESide.BUY ? ESide.SELL: ESide.BUY];
        uint totalFilled;
        
        for(uint i = 0; i < otherSideOrders.length && totalFilled < amount; i++)  {
            if(orderType == EOrderType.MARKET || (orderType == EOrderType.LIMIT && otherSideOrders[i]._price == price)) {
                uint toFill = _getHowMuchCanBeFilled(otherSideOrders[i], amount.sub(totalFilled));
                totalFilled = totalFilled.add(toFill);
                uint cost = toFill.mul(otherSideOrders[i]._price);

                if(side == ESide.BUY) {
                    if(orderType == EOrderType.MARKET){
                        require(_getSpendingBalance(msg.sender, "ETH") >= cost, "Buyer doesn't have enough ETH for purchase");
                    }
                    _executeTrade(msg.sender, otherSideOrders[i]._trader, ticker, toFill, cost);
                    otherSideOrders[i]._filled = otherSideOrders[i]._filled.add(toFill);
                    otherSideOrders[i]._lockedBalance = otherSideOrders[i]._lockedBalance.sub(toFill);
                } 
                else if(side == ESide.SELL) {
                    _executeTrade(otherSideOrders[i]._trader, msg.sender, ticker, toFill, cost);
                    otherSideOrders[i]._filled = otherSideOrders[i]._filled.add(toFill);
                    otherSideOrders[i]._lockedBalance = otherSideOrders[i]._lockedBalance.sub(cost);
                }
            }
            else if(orderType == EOrderType.LIMIT && 
                    (side == ESide.BUY && otherSideOrders[i]._price > price ||
                    side == ESide.SELL && otherSideOrders[i]._price < price)) {
                break;
            }
        }

        return totalFilled;
    }

    function _hasOrder(address trader, bytes32 ticker, ESide side) private view returns(bool) {
        SOrder[] storage orders = _orderBook[ticker][side];
        bool hasOrder;
        for(uint i = 0; i < orders.length && !hasOrder; i++) {
            hasOrder = orders[i]._trader == trader;
        }
        return hasOrder;
    }

    function _removeFilledOrders(SOrder[] storage orders) private {
        while(orders.length > 0 && orders[0]._filled == orders[0]._amount){
            for(uint i = 0; i < orders.length - 1; i++) {
                orders[i] = orders[i + 1];
            }
            orders.pop();
        }
    }

    function _removeOrderByTrader(SOrder[] storage orders, address trader) private {
        if(orders.length == 1 && orders[0]._trader == trader){
            orders.pop();
            return;
        }

        bool found = false;
        for(uint i = 0; i < orders.length; i++) {
            if(found) {
                orders[i-1] = orders[i];
            }
            else if(orders[i]._trader == trader) {
                if(i == (orders.length - 1)) {
                    orders.pop();
                    return;
                }
                found = true;
            }
        }
    }

    function _sortOrders(ESide side, SOrder[] storage orders) private {
        for(uint i = orders.length > 0 ? orders.length -1 : 0; i > 0; i --) {
            if ((side == ESide.BUY && orders[i - 1]._price >= orders[i]._price)
                || (side == ESide.SELL && orders[i - 1]._price <= orders[i]._price)) {
                break;
            }    
            SOrder memory swap = orders[i - 1];
            orders[i - 1] = orders[i];
            orders[i] = swap;
        }
    }

    function _verifyCanMakeOrder(EOrderType orderType, ESide side, bytes32 ticker, uint amount, uint price) private view {
        if(side == ESide.BUY){
            if(orderType == EOrderType.LIMIT) {
                require(_getSpendingBalance(msg.sender, "ETH") >= amount.mul(price), 
                Utils.concatStrings("Not enough ETH for purchase, cost: ", Utils.uint2str(amount.mul(price))));
            }
            require(!_hasOrder(msg.sender, ticker, ESide.BUY), "Can't create buy order when a buy order already exists");
            require(!_hasOrder(msg.sender, ticker, ESide.SELL), "Can't create buy order when a sell order exists");
        } 
        else if(side == ESide.SELL){
            require(_getSpendingBalance(msg.sender, ticker) >= amount, "Insuffient balance, Amount too high");
            require(!_hasOrder(msg.sender, ticker, ESide.SELL), "Can't create sell order when a sell order already exists");
            require(!_hasOrder(msg.sender, ticker, ESide.BUY), "Can't create sell order when a buy order exists");
        }
    }
}
