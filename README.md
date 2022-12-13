本项目底层采用了eosio链，编写c++合约来实现数据的上链、获取、更新和删除。
# 运行环境
Ubuntu 20.04

# 安装EOSIO二进制
wget https://github.com/eosio/eos/releases/download/v2.1.0/eosio_2.1.0-1-ubuntu-20.04_amd64.deb

sudo apt install ./eosio_2.1.0-1-ubuntu-20.04_amd64.deb

# 安装EOSIO.CDT二进制
wget https://github.com/eosio/eosio.cdt/releases/download/v1.8.0/eosio.cdt_1.8.0-1-ubuntu-18.04_amd64.deb

sudo apt install ./eosio.cdt_1.8.0-1-ubuntu-18.04_amd64.deb

# 创建并解锁开发钱包
cleos wallet create -f default_wallet.pwd

cleos wallet open

cleos wallet unlock(default_wallet.pwd中的密钥)

# 导入eosio开发密钥
cleos wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

# 创建目录并启动创世节点
cd /root/biosboot/genesis
vim genesis_start.sh
#!/bin/bash
DATADIR="./blockchain"

if [ ! -d $DATADIR ]; then
  mkdir -p $DATADIR;
fi

nodeos \
--signature-provider EOS_PUB_DEV_KEY=KEY:EOS_PRIV_DEV_KEY \
--plugin eosio::producer_plugin \
--plugin eosio::producer_api_plugin \
--plugin eosio::chain_plugin \
--plugin eosio::chain_api_plugin \
--plugin eosio::http_plugin \
--plugin eosio::history_api_plugin \
--plugin eosio::history_plugin \
--data-dir $DATADIR"/data" \
--blocks-dir $DATADIR"/blocks" \
--config-dir $DATADIR"/config" \
--producer-name eosio \
--http-server-address 127.0.0.1:8888 \
--p2p-listen-endpoint 127.0.0.1:9010 \
--access-control-allow-origin=* \
--contracts-console \
--http-validate-host=false \
--verbose-http-errors \
--enable-stale-production \
--p2p-peer-address localhost:9011 \
--p2p-peer-address localhost:9012 \
--p2p-peer-address localhost:9013 \
>> $DATADIR"/nodeos.log" 2>&1 & \
echo $! > $DATADIR"/eosd.pid"

# 运行文件
./genesis_start.sh

# 创建同合约的账户名metadata
cleos create key --file metadata.txt

cleos wallet import --private-key EOS_PRIV_DEV_KEY

cleos create account eosio metadata EOS_PUB_DEV_KEY

# 编译并部署合约metadata.cpp
mkdir metadata

cd metadata

vim metadata.cpp

eosio-cpp metadata.cpp -o metadata.wasm

cleos set contract metadata . -p metadata@active

