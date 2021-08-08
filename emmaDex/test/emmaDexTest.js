const emmaDex = artifacts.require("emmaDex");
const Link = artifacts.require("Link");
const truffleAssertions = require('truffle-assertions');
const truffleAssert = require('truffle-assertions');

contract("emmaDex", accounts => {
    it("Should throw an error if ETH balance is too low when creating BUY limit order", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        await dex.addToken(web3.utils.fromUtf8("LINK"), link.address)

        await truffleAssert.reverts(
            dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 10, 1)
        )
        await dex.depositEth({value: 10})
        await truffleAssert.passes(
            dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 10, 1)
        )
        
    })
    it("Should throw an error if token balance is too low when creating SELL limit order", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        await truffleAssert.reverts(
            dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 10, 1, {from:accounts[1]})
            );

        await link.transfer(accounts[1], 10);
        await link.approve(dex.address, 10, {from:accounts[1]});
        await dex.deposit(10, web3.utils.fromUtf8("LINK"), {from:accounts[1]});
        await truffleAssert.passes(
            dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 10, 1, {from:accounts[1]})
            );

        let sellOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(sellOrderBook.length == 0, "Filled order should have been removed");
        let buyOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 0);
        assert(buyOrderBook.length == 0, "Filled order should have been removed");
        
    });
    it("Should execute agreeing limit orders", async() =>{
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        await dex.depositEth({value:1500, from:accounts[0]});
        await dex.depositEth({value:1000, from:accounts[1]});

        let balancesBefore = []
        balancesBefore.push(await dex._balances(accounts[0], web3.utils.fromUtf8("LINK")));
        balancesBefore.push(await dex._balances(accounts[1], web3.utils.fromUtf8("LINK")));

        balancesBefore.push(await dex._balances(accounts[2], web3.utils.fromUtf8("ETH")));
        balancesBefore.push(await dex._balances(accounts[3], web3.utils.fromUtf8("ETH")));

        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 5, 300);
        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 10, 100, {from:accounts[1]});

        await link.transfer(accounts[2], 5);
        await link.transfer(accounts[3], 10);
        await link.approve(dex.address, 5, {from:accounts[2]});
        await link.approve(dex.address, 10, {from:accounts[3]});
        await dex.deposit(5, web3.utils.fromUtf8("LINK"), {from:accounts[2]})
        await dex.deposit(10, web3.utils.fromUtf8("LINK"), {from:accounts[3]})

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 5, 300, {from:accounts[2]});
        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 10, 100, {from:accounts[3]});

        let balancesAfter = []
        balancesAfter.push(await dex._balances(accounts[0], web3.utils.fromUtf8("LINK")));
        balancesAfter.push(await dex._balances(accounts[1], web3.utils.fromUtf8("LINK")));
        balancesAfter.push(await dex._balances(accounts[2], web3.utils.fromUtf8("ETH")));

        balancesAfter.push(await dex._balances(accounts[3], web3.utils.fromUtf8("ETH")));

        assert.equal(balancesBefore[0].toNumber() + 5, balancesAfter[0].toNumber(), "Wrong LINK balance for account[0]");
        assert.equal(balancesBefore[1].toNumber() + 10, balancesAfter[1].toNumber(), "Wrong LINK balance for account[1]");
        assert.equal(balancesBefore[2].toNumber() + 5 * 300, balancesAfter[2].toNumber(), "Wrong ETH balance for account[2]");
        assert.equal(balancesBefore[3].toNumber() + 10 * 100, balancesAfter[3].toNumber(), "Wrong ETH balance for account[3]");
    });
    it("Should check balance that can be spent correctly", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        await dex.depositEth({value: 500});
        await dex.depositEth({value: 600, from:accounts[1]});
        await dex.depositEth({value: 700, from:accounts[2]});

        let spendingBalancesBefore = [];
        spendingBalancesBefore.push(await dex.getSpendingBalance(web3.utils.fromUtf8("ETH")));
        spendingBalancesBefore.push(await dex.getSpendingBalance(web3.utils.fromUtf8("ETH"), {from:accounts[1]}));
        spendingBalancesBefore.push(await dex.getSpendingBalance(web3.utils.fromUtf8("ETH"), {from:accounts[2]}));

        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 10, 50);
        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 20, 30, {from:accounts[1]});
        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 5, 140, {from:accounts[2]});

        let spendingBalancesAfter = [];
        spendingBalancesAfter.push(await dex.getSpendingBalance(web3.utils.fromUtf8("ETH")));
        spendingBalancesAfter.push(await dex.getSpendingBalance(web3.utils.fromUtf8("ETH"), {from:accounts[1]}));
        spendingBalancesAfter.push(await dex.getSpendingBalance(web3.utils.fromUtf8("ETH"), {from:accounts[2]}));

        assert.equal(spendingBalancesBefore[0].toNumber() - 500, spendingBalancesAfter[0].toNumber());
        assert.equal(spendingBalancesBefore[1].toNumber() - 600, spendingBalancesAfter[1].toNumber());
        assert.equal(spendingBalancesBefore[2].toNumber() - 700, spendingBalancesAfter[2].toNumber());

        await link.transfer(accounts[3], 10);
        await link.transfer(accounts[4], 20);
        await link.transfer(accounts[5], 30);
        await link.approve(dex.address, 10, {from:accounts[3]});
        await link.approve(dex.address, 20, {from:accounts[4]});
        await link.approve(dex.address, 30, {from:accounts[5]});
        await dex.deposit(10, web3.utils.fromUtf8("LINK"), {from:accounts[3]});
        await dex.deposit(20, web3.utils.fromUtf8("LINK"), {from:accounts[4]});
        await dex.deposit(30, web3.utils.fromUtf8("LINK"), {from:accounts[5]});

        spendingBalancesBefore.push(await dex.getSpendingBalance(web3.utils.fromUtf8("LINK"), {from:accounts[3]}));
        spendingBalancesBefore.push(await dex.getSpendingBalance(web3.utils.fromUtf8("LINK"), {from:accounts[4]}));
        spendingBalancesBefore.push(await dex.getSpendingBalance(web3.utils.fromUtf8("LINK"), {from:accounts[5]}));

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 10, 25, {from:accounts[3]});
        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 20, 35, {from:accounts[4]});
        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 30, 65, {from:accounts[5]});

        spendingBalancesAfter.push(await dex.getSpendingBalance(web3.utils.fromUtf8("LINK"), {from:accounts[3]}));
        spendingBalancesAfter.push(await dex.getSpendingBalance(web3.utils.fromUtf8("LINK"), {from:accounts[4]}));
        spendingBalancesAfter.push(await dex.getSpendingBalance(web3.utils.fromUtf8("LINK"), {from:accounts[5]}));

        assert.equal(spendingBalancesBefore[3].toNumber() - 10, spendingBalancesAfter[3].toNumber());
        assert.equal(spendingBalancesBefore[4].toNumber() - 20, spendingBalancesAfter[4].toNumber());
        assert.equal(spendingBalancesBefore[5].toNumber() - 30, spendingBalancesAfter[5].toNumber());
    });
    it("Should sort the BUY orderbook from highest to lowest", async() =>{
        let dex = await emmaDex.deployed();

        await dex.depositEth({value: 1000, from:accounts[6]});
        await dex.depositEth({value: 1000, from:accounts[7]});

        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 2, 150, {from:accounts[6]});
        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 3, 250, {from:accounts[7]});

        let orderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 0);
        assert(orderBook.length > 0, "No orders in orderbook");

        for(let i = 0; i < orderBook.length -1; i++) {
            assert(parseInt(orderBook[i]._price) >= parseInt(orderBook[i+1]._price), "BUY orderbook out of order");
        }
    });
    it("Should order the SELL orderbook from lowest to highest", async() =>{
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        await link.transfer(accounts[8], 5);
        await link.transfer(accounts[9], 5);

        await link.approve(dex.address, 5, {from:accounts[8]});
        await link.approve(dex.address, 5, {from:accounts[9]});

        await dex.deposit(5, web3.utils.fromUtf8("LINK"), {from:accounts[8]});
        await dex.deposit(5, web3.utils.fromUtf8("LINK"), {from:accounts[9]});

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 2, 100, {from:accounts[8]});       
        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 3, 300, {from:accounts[9]});

        let orderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(orderBook.length > 0, "No orders in orderbook");

        for(let i = 0; i < orderBook.length -1; i++) {
            assert(parseInt(orderBook[i]._price) <= (orderBook[i+1]._price), "SELL orderbook out of order");
        }
    });
});

contract("emmaDex", accounts => {
    it("Should throw an error when creating a sell market order without adequate token balance", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        await dex.addToken(web3.utils.fromUtf8("LINK"), link.address);
        let balance = await dex._balances(accounts[0], web3.utils.fromUtf8("LINK"));
        assert.equal(balance.toNumber(), 0, "Initial LINK balance is not 0");
        
        await truffleAssert.reverts(
            dex.createMarketOrder(1, web3.utils.fromUtf8("LINK"), 10)
        );
    });
    it("Market orders can be submitted even if the order book is empty", async() => {
        let dex = await emmaDex.deployed();
        
        let orderbook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 0);
        assert(orderbook.length == 0, "Buy side Orderbook length is not 0");
        
        await dex.depositEth({value: 50000});
        await truffleAssert.passes(
            dex.createMarketOrder(0, web3.utils.fromUtf8("LINK"), 10)
        );
    });
    it("Market orders should not fill more limit orders than the market order amount", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        let orderbook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(orderbook.length == 0, "Sell side Orderbook should be empty at start of test");

        await link.transfer(accounts[1], 5);
        await link.transfer(accounts[2], 5);
        await link.transfer(accounts[3], 5);
        await link.approve(dex.address, 5, {from: accounts[1]});
        await link.approve(dex.address, 5, {from: accounts[2]});
        await link.approve(dex.address, 5, {from: accounts[3]});

        await dex.deposit(5, web3.utils.fromUtf8("LINK"), {from: accounts[1]});
        await dex.deposit(5, web3.utils.fromUtf8("LINK"), {from: accounts[2]});
        await dex.deposit(5, web3.utils.fromUtf8("LINK"), {from: accounts[3]});

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 5, 300, {from: accounts[1]});
        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 5, 400, {from: accounts[2]});
        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 5, 500, {from: accounts[3]});

        await dex.createMarketOrder(0, web3.utils.fromUtf8("LINK"), 10);

        orderbook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(orderbook.length == 1, "Sell side Orderbook should only have 1 order left");
        assert(orderbook[0]._filled == 0, "Sell side order should have 0 filled");

    });
    it("Market orders should be filled until the order book is empty", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        let orderbook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(orderbook.length == 1, "Sell side Orderbook should have 1 order left");

        await link.transfer(accounts[1], 5);
        await link.transfer(accounts[2], 5);
        await link.approve(dex.address, 5, {from: accounts[1]});
        await link.approve(dex.address, 5, {from: accounts[2]});     
        await dex.deposit(5, web3.utils.fromUtf8("LINK"), {from: accounts[1]});
        await dex.deposit(5, web3.utils.fromUtf8("LINK"), {from: accounts[2]});

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 5, 400, {from: accounts[1]});
        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 5, 500, {from: accounts[2]});

        let balanceBefore = await dex._balances(accounts[0], web3.utils.fromUtf8("LINK"));

        await dex.createMarketOrder(0, web3.utils.fromUtf8("LINK"), 50);

        let balanceAfter = await dex._balances(accounts[0], web3.utils.fromUtf8("LINK"));

        assert.equal(balanceBefore.toNumber() + 15, balanceAfter.toNumber());
    });
    it("The ETH balance of the buyer should decrease with the filled amount", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        await link.transfer(accounts[1], 1);
        await link.approve(dex.address, 1, {from: accounts[1]});
        await dex.deposit(1, web3.utils.fromUtf8("LINK"), {from:accounts[1]});

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 1, 300, {from: accounts[1]});

        let balanceBefore = await dex._balances(accounts[0], web3.utils.fromUtf8("ETH"));
        await dex.createMarketOrder(0, web3.utils.fromUtf8("LINK"), 1);
        let balanceAfter = await dex._balances(accounts[0], web3.utils.fromUtf8("ETH"));

        assert.equal(balanceBefore.toNumber() - 300, balanceAfter.toNumber());
    });
    it("The token balances of the limit order sellers should decrease with the filled amounts.", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        let orderbook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(orderbook.length == 0, "Sell side Orderbook should be empty at start of test");

        await link.transfer(accounts[1], 1);
        await link.transfer(accounts[2], 1);
        await link.approve(dex.address, 1, {from: accounts[1]});
        await link.approve(dex.address, 1, {from: accounts[2]});
        await dex.deposit(1, web3.utils.fromUtf8("LINK"), {from: accounts[1]});
        await dex.deposit(1, web3.utils.fromUtf8("LINK"), {from: accounts[2]});

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 1, 300, {from: accounts[1]});
        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 1, 400, {from: accounts[2]});

        let account1balanceBefore = await dex._balances(accounts[1], web3.utils.fromUtf8("LINK"));
        let account2balanceBefore = await dex._balances(accounts[2], web3.utils.fromUtf8("LINK"));

        await dex.depositEth({value:700});
        await dex.createMarketOrder(0, web3.utils.fromUtf8("LINK"), 2);

        let account1balanceAfter = await dex._balances(accounts[1], web3.utils.fromUtf8("LINK"));
        let account2balanceAfter = await dex._balances(accounts[2], web3.utils.fromUtf8("LINK"));

        assert.equal(account1balanceBefore.toNumber() - 1, account1balanceAfter.toNumber());
        assert.equal(account2balanceBefore.toNumber() - 1, account2balanceAfter.toNumber());
    });
    it("Filled limit orders should be removed from the orderbook", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        await link.approve(dex.address, 1);
        await dex.deposit(1, web3.utils.fromUtf8("LINK"));
        
        let orderbook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 1, 300);

        await dex.depositEth({value: 300, from:accounts[1]});
        await dex.createMarketOrder(0, web3.utils.fromUtf8("LINK"), 1, {from:accounts[1]});

        orderbook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(orderbook.length == 0, "Sell side Orderbook should be empty after trade");
    });
    it("Limit orders filled property should be set correctly after a trade", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        let orderbook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(orderbook.length == 0, "Sell side Orderbook should be empty at start of test");

        await link.transfer(accounts[1], 5);
        await link.approve(dex.address, 5, {from: accounts[1]});
        await dex.deposit(5, web3.utils.fromUtf8("LINK"), {from: accounts[1]});

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 5, 300, {from: accounts[1]})

        await dex.depositEth({value: 300});
        await dex.createMarketOrder(0, web3.utils.fromUtf8("LINK"), 2);

        orderbook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(orderbook[0]._filled == 2, "filled should be 2 but is: " + orderbook[0]._filled);
        assert(orderbook[0]._amount == 5, "amount should be 5 but is: " + orderbook[0]._amount);
    });
    it("Should throw an error when creating a buy market order without adequate ETH balance", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        let balance = await dex._balances(accounts[4], web3.utils.fromUtf8("ETH"));
        assert.equal(balance.toNumber(), 0, "Initial ETH balance is not 0");

        await link.transfer(accounts[2], 5);
        await link.approve(dex.address, 5, {from:accounts[2]});
        await dex.deposit(5, web3.utils.fromUtf8("LINK"), {from:accounts[2]});

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 5, 300, {from: accounts[2]});

        await truffleAssert.reverts(
            dex.createMarketOrder(0, web3.utils.fromUtf8("LINK"), 5, {from: accounts[4]})
        );
    });
});

contract("emmaDex", accounts => {
    it("Should not allow changing a limit order if none exists", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        await dex.addToken(web3.utils.fromUtf8("LINK"), link.address);

        await link.approve(dex.address, 1);
        await dex.deposit(1, web3.utils.fromUtf8("LINK"));

        await truffleAssertions.reverts(
            dex.changeExistingLimitOrder(0, web3.utils.fromUtf8("LINK"), 1, 10)
        );

        await dex.depositEth({value:10});
        await truffleAssertions.reverts(
            dex.changeExistingLimitOrder(1, web3.utils.fromUtf8("LINK"), 1, 10)
        );
    });
    it("Should update changed orders correctly", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();

        await dex.depositEth({value:40});

        let buyOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 0);
        let sellOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);

        assert(buyOrderBook.length == 0, "buyOrderBook should be empty at start of test");
        assert(sellOrderBook.length == 0, "sellOrderBook should be empty at start of test");

        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 1, 10);
        await dex.changeExistingLimitOrder(0, web3.utils.fromUtf8("LINK"), 2, 20);

        buyOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 0);
        assert(buyOrderBook.length == 1, "buyOrderBook should have 1 order");
        assert(buyOrderBook[0]._price == 20, "buy order should have price 20");
        assert(buyOrderBook[0]._amount == 2, "buy order should have amount 1");

        await link.transfer(accounts[1], 2);
        await link.approve(dex.address, 2, {from:accounts[1]});
        await dex.deposit(2, web3.utils.fromUtf8("LINK"), {from:accounts[1]});

        await dex.createLimitOrder(1, web3.utils.fromUtf8("LINK"), 1, 15, {from:accounts[1]});
        await dex.changeExistingLimitOrder(1, web3.utils.fromUtf8("LINK"), 2, 25, {from:accounts[1]});

        sellOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(sellOrderBook.length == 1, "sellOrderBook should have 1 order");
        assert(sellOrderBook[0]._price == 25, "sell order should have price 25");
        assert(sellOrderBook[0]._amount == 2, "sell order should have amount 2");

        await dex.changeExistingLimitOrder(0, web3.utils.fromUtf8("LINK"), 2, 25);
        sellOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(sellOrderBook.length == 0, "sellOrderBook should have 0 orders");
    });
    it("Should not be possible to cancel a limit order if none exits", async() => {
        let dex = await emmaDex.deployed();

        let buyOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 0);
        assert(buyOrderBook.length == 0, "buyOrderBook should have 0 orders");
        let sellOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 1);
        assert(sellOrderBook.length == 0, "sellOrderBook should have 0 orders");

        await truffleAssert.reverts(
            dex.cancelLimitOrder(0, web3.utils.fromUtf8("LINK"))
        );

        await truffleAssert.reverts(
            dex.cancelLimitOrder(1, web3.utils.fromUtf8("LINK"))
        );
    });
    it("Should cancel a limit order properly if one exists", async() => {
        let dex = await emmaDex.deployed();

        await dex.depositEth({value:50});
        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 5, 10);

        let buyOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 0);
        assert(buyOrderBook.length == 1, "buyOrderBook should have 1 order");

        await dex.cancelLimitOrder(0, web3.utils.fromUtf8("LINK"));

        buyOrderBook = await dex.getOrderBook(web3.utils.fromUtf8("LINK"), 0);
        assert(buyOrderBook.length == 0, "buyOrderBook should have 0 orders");
    });
    it("Should throw an error if it tries to get an order that doesn't exist", async() => {
        let dex = await emmaDex.deployed()

        await truffleAssert.reverts(
            dex.getMyOrder(web3.utils.fromUtf8("LINK"))
        );
    });
    it("Should get correct order", async() => {
        let dex = await emmaDex.deployed();

        await dex.depositEth({value:100});
        await dex.depositEth({value:100, from:accounts[1]});
        await dex.depositEth({value:100, from:accounts[2]});
        await dex.depositEth({value:100, from:accounts[3]});

        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 5, 20);
        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 10, 10, {from:accounts[1]});
        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 2, 50, {from:accounts[2]});
        await dex.createLimitOrder(0, web3.utils.fromUtf8("LINK"), 4, 25, {from:accounts[3]});

        let order0 = await dex.getMyOrder(web3.utils.fromUtf8("LINK"));
        let order1 = await dex.getMyOrder(web3.utils.fromUtf8("LINK"), {from:accounts[1]});
        let order2 = await dex.getMyOrder(web3.utils.fromUtf8("LINK"), {from:accounts[2]});
        let order3 = await dex.getMyOrder(web3.utils.fromUtf8("LINK"), {from:accounts[3]});

        assert(order0._amount == 5 && order0._price == 20, "order0 is not correct");
        assert(order1._amount == 10 && order1._price == 10, "order1 is not correct");
        assert(order2._amount == 2 && order2._price == 50, "order2 is not correct");
        assert(order3._amount == 4 && order3._price == 25, "order3 is not correct");  
    });
});
