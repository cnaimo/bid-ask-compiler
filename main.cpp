#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

struct event_line {
    std::vector<std::string> date;
    std::vector<std::string> timestamp;
    std::vector<std::string> order_num;
    std::vector<std::string> event;
    std::vector<std::string> ticker;
    std::vector<double> price;
    std::vector<int> qty;
    std::vector<std::string> mpid;
    std::vector<std::string> exchange;
};
typedef struct event_line lvl_2;

struct bid_ask_define {
    std::vector<double> bid_price;
    std::vector<int> bid_qty;
    std::vector<std::string> bid_exchange;
    std::vector<std::string> bid_id;
    std::vector<double> ask_price;
    std::vector<int> ask_qty;
    std::vector<std::string> ask_exchange;
    std::vector<std::string> ask_id;
};
typedef struct bid_ask_define bid_ask;

template <class T>
void reorder(std::vector<T>& vA, std::vector<size_t>& vI){
    // Quoted from StackOverflow user rcgldr, under Creative Commons
    size_t i, j, k;
        for(i = 0; i < vA.size(); i++){
            while(i != (j = vI[i])){
                k = vI[j];
                std::swap(vA[j], vA[k]);
                std::swap(vI[i], vI[j]);
        }}
}

template <typename T>
std::vector<size_t> sort_indexes(const std::vector<T> &v) {
    // Quoted from StackOverflow user Lukasz Wiklendt, under Creative Commons
    // initialize original index locations
    std::vector<size_t> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);
    // sort indexes based on comparing values in v
    sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
    return idx;
}

void print_bid_ask(bid_ask data){
    std::cout<<"BID\tSIZE\tEXCHANGE\tASK\tSIZE\tEXCHANGE"<<std::endl;
    int len_bid = data.bid_price.size();
    int len_ask = data.ask_price.size();
    int max_len;
    if(len_bid > len_ask){max_len = len_bid;}
    else{max_len = len_ask;}
    for(int i = 0; i < max_len; i++){
        if(i < len_bid){std::cout<< data.bid_price[i] << "\t" << data.bid_qty[i] << "\t" << data.bid_exchange[i] << "\t\t";}
        else{for(int q = 0; q < 6; q++){std::cout<<"\t";}}
        if(i < len_ask){std::cout<< data.ask_price[i] << "\t" << data.ask_qty[i] << "\t" << data.ask_exchange[i] <<"\n";}
        else{for(int q = 0; q < 5; q++){std::cout<<"\t";}; std::cout<<"\n";}
    }
    std::cout<<"\n";
}

lvl_2 historical_data_parse(){
    lvl_2 s;
    std::ifstream historical_data;
    historical_data.open("YOUR-DATA-HERE.csv");
    // sample data available from AlgoSeek website: https://www.algoseek.com/samples/
    if(!historical_data.is_open()){
        std::cout<<"Error! Unable to open historical data!"<<std::endl;
        return s;
    }
    else{
        std::cout<<"Historical data opened!"<<std::endl;
    }
    std::string line;
    std::string current;
    std::vector<std::string> date;
    std::vector<std::string> timestamp;
    std::vector<std::string> order_num;
    std::vector<std::string> event;
    std::vector<std::string> ticker;
    std::vector<double> price;
    std::vector<int> qty;
    std::vector<std::string> mpid;
    std::vector<std::string> exchange;
    bool initial = true;
    while(getline(historical_data, line)){
        if(initial){
            initial = false;
            continue;
        }
        std::stringstream ss(line);
        getline(ss, current, ',');
        date.push_back(current);
        getline(ss, current, ',');
        timestamp.push_back(current);
        getline(ss, current, ',');
        order_num.push_back(current);
        getline(ss, current, ',');
        event.push_back(current);
        getline(ss, current, ',');
        ticker.push_back(current);
        getline(ss, current, ',');
        price.push_back(std::stod (current));
        getline(ss, current, ',');
        qty.push_back(std::stoi (current));
        getline(ss, current, ',');
        mpid.push_back(current);
        getline(ss, current, ',');
        exchange.push_back(current);
    }
    s.date = date;
    s.timestamp = timestamp;
    s.order_num = order_num;
    s.event = event;
    s.ticker = ticker;
    s.price = price;
    s.qty = qty;
    s.mpid = mpid;
    s.exchange = exchange;
    return s;
}

bid_ask sort_bid_ask(bid_ask ba){
    std::vector<long unsigned int> index;
    std::vector<long unsigned int> index_copy;
    if(ba.bid_price.size() > 0){
        index = sort_indexes(ba.bid_price);
        std::reverse(index.begin(), index.end());
        index_copy = index;
        reorder(ba.bid_price, index_copy);
        index_copy = index;
        reorder(ba.bid_qty, index_copy);
        index_copy = index;
        reorder(ba.bid_exchange, index_copy);
        index_copy = index;
        reorder(ba.bid_id, index_copy);
    }
    if(ba.ask_price.size() > 0){
        index = sort_indexes(ba.ask_price);
        index_copy = index;
        reorder(ba.ask_price, index_copy);
        index_copy = index;
        reorder(ba.ask_qty, index_copy);
        index_copy = index;
        reorder(ba.ask_exchange, index_copy);
        index_copy = index;
        reorder(ba.ask_id, index_copy);
    }
    return ba;
}

void generate_bid_ask(lvl_2 data){
    bid_ask ba;
    std::string timestamp;
    double price;
    int qty;
    std::string event;
    std::string exchange;
    std::string id;
    int i = 0;
    while((unsigned)i < data.price.size()){
        timestamp = data.timestamp[i];
        while(data.timestamp[i] == timestamp){
            price = data.price[i];
            qty = data.qty[i];
            event = data.event[i];
            exchange = data.exchange[i];
            id = data.order_num[i];
            if(data.event[i] == "ADD BID"){
                ba.bid_price.push_back(price);
                ba.bid_qty.push_back(qty);
                ba.bid_exchange.push_back(exchange);
                ba.bid_id.push_back(id);
            }
            else if(data.event[i] == "ADD ASK"){
                ba.ask_price.push_back(price);
                ba.ask_qty.push_back(qty);
                ba.ask_exchange.push_back(exchange);
                ba.ask_id.push_back(id);
            }
            else if(data.event[i] == "DELETE BID" || data.event[i] == "FILL BID"){
                auto it = std::find(ba.bid_id.begin(), ba.bid_id.end(), id);
                int index = std::distance(ba.bid_id.begin(), it);
                ba.bid_price.erase(ba.bid_price.begin() + index);
                ba.bid_qty.erase(ba.bid_qty.begin() + index);
                ba.bid_exchange.erase(ba.bid_exchange.begin() + index);
                ba.bid_id.erase(ba.bid_id.begin() + index);
            }
            else if(data.event[i] == "DELETE ASK" || data.event[i] == "FILL ASK"){
                auto it = std::find(ba.ask_id.begin(), ba.ask_id.end(), id);
                int index = std::distance(ba.ask_id.begin(), it);
                ba.ask_price.erase(ba.ask_price.begin() + index);
                ba.ask_qty.erase(ba.ask_qty.begin() + index);
                ba.ask_exchange.erase(ba.ask_exchange.begin() + index);
                ba.ask_id.erase(ba.ask_id.begin() + index);
            }
            else if(data.event[i] == "CANCEL BID" || data.event[i] == "EXECUTE BID"){
                auto it = std::find(ba.bid_id.begin(), ba.bid_id.end(), id);
                int index = std::distance(ba.bid_id.begin(), it);
                ba.bid_qty[index] -= qty;
            }
            else if(data.event[i] == "CANCEL ASK" || data.event[i] == "EXECUTE ASK"){
                auto it = std::find(ba.ask_id.begin(), ba.ask_id.end(), id);
                int index = std::distance(data.order_num.begin(), it);
                ba.ask_qty[index] -= qty;
            }
            else if(data.event[i] == "TRADE BID"){
                // executing non-displayed order
            }
            else if(data.event[i] == "TRADE ASK"){
                // executing non-displayed order
            }
            i ++;
            ba = sort_bid_ask(ba);
            }
        print_bid_ask(ba);
        }
    }


int main(){
    lvl_2 data;
    data = historical_data_parse();
    generate_bid_ask(data);
    std::cout<<"Done!"<<std::endl;
    return 0;
}
