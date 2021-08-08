const Link = artifacts.require("Link");

module.exports = async function (deployer) {
  await deployer.deploy(Link);
};
