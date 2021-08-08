const emmaDex = artifacts.require("emmaDex");

module.exports = function (deployer) {
  deployer.deploy(emmaDex);
};
