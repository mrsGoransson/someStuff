pragma solidity >=0.8.0 <0.9.0;

import "@openzeppelin/contracts/access/Ownable.sol";
import "@openzeppelin/contracts/token/ERC20/IERC20.sol";
import "@openzeppelin/contracts/utils/math/SafeMath.sol";


contract Wallet is Ownable{

    using SafeMath for uint256;

    struct SToken{
        bytes32 _ticker;
        address _tokenAddress;
    }
    mapping(bytes32=>SToken) public _tokenMapping;
    bytes32[] public _tokenList;
    mapping(address=>mapping(bytes32=>uint256)) public _balances;

    modifier tokenExists(bytes32 ticker){
        require(_tokenMapping[ticker]._tokenAddress != address(0), "Token doesn't exist");
        _;
    }

    function addToken(bytes32 ticker, address tokenAddress) external onlyOwner {
        require(_tokenMapping[ticker]._tokenAddress == address(0), "Token already added");
        _tokenMapping[ticker] = SToken(ticker, tokenAddress);
        _tokenList.push(ticker);
    }

    function deposit(uint amount, bytes32 ticker) external tokenExists(ticker) {
        _balances[msg.sender][ticker] = _balances[msg.sender][ticker].add(amount);
        IERC20(_tokenMapping[ticker]._tokenAddress).transferFrom(msg.sender, address(this), amount); //needs allowance
    }

    function depositEth() payable external {
        _balances[msg.sender]["ETH"] = _balances[msg.sender]["ETH"].add(msg.value);
    }

    function withdraw(uint amount, bytes32 ticker) external tokenExists(ticker) {
        require(_balances[msg.sender][ticker] >= amount, "Balance not sufficient");

        _balances[msg.sender][ticker] = _balances[msg.sender][ticker].sub(amount);
        IERC20(_tokenMapping[ticker]._tokenAddress).transfer(msg.sender, amount);
    }

}