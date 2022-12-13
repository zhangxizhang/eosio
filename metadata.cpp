#include <eosio/eosio.hpp>
#include <eosio/print.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("metadata")]] metadata : public eosio::contract {
    public:
        
        // 构造函数
        metadata(name receiver, name code, datastream<const char*> ds): contract(receiver, code, ds) {}

        [[eosio::action]]
        void create(name account, string owner, string ipfs_hash, string category, string saving_time) {
             // 插入一条数据

            require_auth(account);
            records_index recordset(get_first_receiver(), get_first_receiver().value);
            
            uint64_t currentId = recordset.available_primary_key();
            recordset.emplace(account, [&](auto& row) {
                row.id = currentId;
                row.owner = owner;
                row.ipfs_hash = ipfs_hash;
                row.category = category;
                row.saving_time = saving_time;
                row.count = 0;
            });
            print("the current id is ", currentId);
        }

        [[eosio::action]]
        void get(uint64_t id) {
            // 获取对应的ipfs_hash

            records_index recordset(get_self(), get_first_receiver().value);

            auto iterator = recordset.find(id);
            check(iterator != recordset.end(), "the record does not exist");

            //找到，则相应的访问次数加一
            recordset.modify(iterator, get_self(), [&](auto& row) {
                row.count++;
	    });
            
            print("The ", id, "'s ipfs_hash is ", iterator->ipfs_hash);
            print("\n The count is ", iterator->count);
        }

        [[eosio::action]]
        void update(name account, uint64_t id, string owner, string category, string saving_time) {
            // 更新字段值
            
            require_auth(account);
            records_index recordset(get_first_receiver(), get_first_receiver().value);

            auto iterator = recordset.find(id);
            check(iterator != recordset.end(), "the record does not exist");

            recordset.modify(iterator, account, [&](auto& row) {
                row.owner = owner;
                row.category = category;
                row.saving_time = saving_time;
            });

            print("update successfully");
        }

        [[eosio::action]]
        void erase(name account, uint64_t id) {
            // 删除一条数据

            require_auth(account);
            records_index recordset(get_first_receiver(), get_first_receiver().value);
            auto iterator = recordset.find(id);
            check(iterator != recordset.end(), "the record does not exist");

            recordset.erase(iterator);
        }

    private:
        struct [[eosio::table]] records
        {
            uint64_t id;        // 数据id，主键
            string owner;       // 数据拥有者
            string ipfs_hash;   // 上传至ipfs的hash
            string category;    // 数据类型
            string saving_time; // 存储在数据库的时间
            uint64_t count;     // 链上访问次数

            uint64_t primary_key() const { return id; }
            uint64_t get_secondary_1() const { return count; }
        };

        using records_index = eosio::multi_index<"metaset"_n, records, indexed_by<"bycount"_n,
        const_mem_fun<records, uint64_t, &records::get_secondary_1>>>;
        
};
