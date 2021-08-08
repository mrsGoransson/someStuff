const emmaDex = artifacts.require("emmaDex");
const Link = artifacts.require("Link");
const truffleAssert = require('truffle-assertions');

contract("emmaDex", accounts => {
    it("Should only be possible for owner to add tokens", async() => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();
        await truffleAssert.passes(
            dex.addToken(web3.utils.fromUtf8("LINK"), link.address, {from:accounts[0]})
            );
        await truffleAssert.reverts(
                dex.addToken(web3.utils.fromUtf8("LINK"), link.address, {from:accounts[1]})
            );
    });
    it("Should handle deposits correctly", async () => {
        let dex = await emmaDex.deployed();
        let link = await Link.deployed();
        await link.approve(dex.address, 500);
        await dex.deposit(100, web3.utils.fromUtf8("LINK"));
        let balance = await dex._balances(accounts[0], web3.utils.fromUtf8("LINK"));
        assert.equal(balance.toNumber(), 100);
    });
    it("Should handle faulty withdrawals correctly", async () => {
        let dex = await emmaDex.deployed();
        await truffleAssert.reverts(dex.withdraw(500,  web3.utils.fromUtf8("LINK")));      
    });
    it("Should handle correct withdrawals correctly", async () => {
        let dex = await emmaDex.deployed();
        await truffleAssert.passes(dex.withdraw(100,  web3.utils.fromUtf8("LINK")));      
    });
} ) 